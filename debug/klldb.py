import argparse
import inspect
import lldb
import shlex
import subprocess
import sys

LENGTH_MASK = @LENGTH_MASK@
ENCODING = 'iso-8859-1'


def value_or_raise(err, val):
    if err.success:
        return val
    else:
        raise RuntimeError(err.description)


def to_address(value):
    err = lldb.SBError()
    a = value.data.GetAddress(err, 0)

    return value_or_raise(err, a)


def read_k_string(string):
    hdr = string.deref.EvaluateExpression(
        "h").EvaluateExpression("hdr").unsigned & LENGTH_MASK
    data = string.deref.EvaluateExpression('data[0]').load_addr

    err = lldb.SBError()
    mem = string.GetProcess().ReadMemory(data, hdr, err)

    return value_or_raise(err, mem)


def kompiled_dir(target):
    addr = target.FindFirstGlobalVariable('kompiled_directory').load_addr
    err = lldb.SBError()
    return target.GetProcess().ReadCStringFromMemory(addr, 4096, err)


def pretty_print_kore(kore, target):
    return subprocess.check_output(['kprint', kompiled_dir(target), '/dev/stdin', 'false'], input=kore).decode(ENCODING)


def term_to_kore(value):
    return value.GetFrame().EvaluateExpression(f"printConfigurationToString((block *){to_address(value)})")


class StartCommand:
    program = 'start'

    @classmethod
    def register_lldb_command(cls, debugger, container, module_name):
        command = f'command script add -o -c {module_name}.{cls.__name__} {container} {cls.program}'
        debugger.HandleCommand(command)

    def __init__(self, debugger, unused):
        pass

    def __call__(self, debugger, command, exe_ctx, result):
        print(f'{self.program} is not implemented.')
        print('Run `process launch --stop-at-entry` instead.')


class StepCommand:
    program = 'step'

    @classmethod
    def register_lldb_command(cls, debugger, container, module_name):
        command = f'command script add -o -c {module_name}.{cls.__name__} {container} {cls.program}'
        debugger.HandleCommand(command)

    def __init__(self, debugger, unused):
        pass

    def __call__(self, debugger, command, exe_ctx, result):
        print(f'{self.program} is not implemented.')
        print('Run `rbreak k_step --one-shot true`, then `continue` instead.')


class MatchCommand:
    program = 'match'

    SUCCESS = 0
    FUNCTION = 1
    FAIL = 2

    @classmethod
    def register_lldb_command(cls, debugger, container, module_name):
        command = f'command script add -o -c {module_name}.{cls.__name__} {container} {cls.program}'
        debugger.HandleCommand(command)

    @classmethod
    def make_parser(cls):
        description = """
Attempt to match a particular rule against a particular configuration.

Takes two arguments: The first is a top-level rule label.
The second is a term of sort GeneratedTopCell. For example, from the "step"
function, you could say something like "k match MODULE.LABEL subject"
to try to match the specified rule against the current configuration.

Does not actually take a step if matching succeeds.
        """.strip()

        parser = argparse.ArgumentParser(
            prog=cls.program,
            description=description,
            add_help=False)

        parser.add_argument(
            'rule', help='K rule label to attempt matching for')
        parser.add_argument('term', help='Term of sort GeneratedTopCell')

        return parser

    def get_short_help(self):
        return 'Attempt to match a particular rule against a particular configuration.'

    def get_long_help(self):
        return self.help_text

    def __init__(self, debugger, unused):
        self.parser = self.make_parser()
        self.help_text = self.parser.format_help()

    def _lookup_function(self, exe_ctx, f_name):
        fns = exe_ctx.target.FindFunctions(f_name).functions

        if len(fns) != 1:
            raise RuntimeError(
                f'Failed to look up function: {f_name}')

        return fns[0]

    def _call(self, exe_ctx, f_name, ret_ty, arg_tys=None, args=None):
        if arg_tys is None:
            arg_tys = []

        if args is None:
            args = []

        assert len(arg_tys) == len(args)
        assert all(ty == 'block *' for ty in arg_tys)

        fn = self._lookup_function(exe_ctx, f_name)
        match_fn = self._lookup_function(exe_ctx, f_name)
        fn_addr = match_fn.addr.GetLoadAddress(exe_ctx.target)

        fp_arg_tys = ', '.join(arg_tys)
        fp_cast_ty = f'({ret_ty} (*)({fp_arg_tys}))'
        fp_cast_exp = f'({fp_cast_ty}{fn_addr})'

        args_cast_exp = ', '.join(
            f'({ty})({to_address(arg)})' for ty, arg in zip(arg_tys, args))
        call_exp = f'{fp_cast_exp}({args_cast_exp})'
        return exe_ctx.target.EvaluateExpression(call_exp)

    def _reset_match_reason(self, exe_ctx):
        self._call(exe_ctx, 'resetMatchReason', 'void')

    def _try_match(self, exe_ctx, rule_name, subject):
        expr = exe_ctx.target.EvaluateExpression(subject)  # check eval?
        self._call(exe_ctx, f'{rule_name}.match', 'void', ['block *'], [expr])

    def _get_match_log_size(self, exe_ctx):
        return self._call(exe_ctx, 'getMatchLogSize', 'size_t').data.uint64[0]

    def _get_match_log(self, exe_ctx):
        return self._call(exe_ctx, 'getMatchLog', 'MatchLog *')

    def __call__(self, debugger, command, exe_ctx, result):
        args = self.parser.parse_args(shlex.split(command))
        self._reset_match_reason(exe_ctx)
        self._try_match(exe_ctx, args.rule, args.term)
        print(self._get_match_log_size(exe_ctx))


def block_summary(value, unused=None):
    kore = term_to_kore(value)
    return pretty_print_kore(read_k_string(kore), value.GetTarget())


def __lldb_init_module(debugger, internal_dict):
    debugger.SetAsync(False)
    debugger.HandleCommand('command container add -h "K commands" k')
    debugger.HandleCommand(
        f'type summary add -F {__name__}.block_summary block')

    # Adapted from llvm-project/lldb/examples/python/cmdtemplate.py
    # Register all classes that have a register_lldb_command method
    for _name, cls in inspect.getmembers(sys.modules[__name__]):
        if inspect.isclass(cls):
            if callable(getattr(cls, "register_lldb_command", None)):
                cls.register_lldb_command(debugger, 'k', __name__)
