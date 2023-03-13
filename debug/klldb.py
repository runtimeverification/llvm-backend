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


def lookup_function(exe_ctx, f_name):
    fns = exe_ctx.target.FindFunctions(f_name).functions

    if len(fns) != 1:
        raise RuntimeError(
            f'Failed to look up function: {f_name}')

    return fns[0]


def target_call(exe_ctx, f_name, ret_ty, arg_tys=None, args=None):
    if arg_tys is None:
        arg_tys = []

    if args is None:
        args = []

    assert len(arg_tys) == len(args)

    fn = lookup_function(exe_ctx, f_name)
    match_fn = lookup_function(exe_ctx, f_name)
    fn_addr = match_fn.addr.GetLoadAddress(exe_ctx.target)

    fp_arg_tys = ', '.join(arg_tys)
    fp_cast_ty = f'({ret_ty} (*)({fp_arg_tys}))'
    fp_cast_exp = f'({fp_cast_ty}{fn_addr})'

    args_cast_exp = ', '.join(
        f'({ty})({to_address(arg)})' for ty, arg in zip(arg_tys, args))
    call_exp = f'{fp_cast_exp}({args_cast_exp})'
    return exe_ctx.target.EvaluateExpression(call_exp)


def block_summary(value, unused=None):
    kore = term_to_kore(value)
    return pretty_print_kore(read_k_string(kore), value.GetTarget())


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


class LogEntry:
    typename = 'MatchLog'

    def __init__(self, exe_ctx, root, idx):
        self.exe_ctx = exe_ctx
        self.entry_type = self.exe_ctx.target.FindFirstType(self.typename)

        index_exp = f'&(({self.typename} *){to_address(root)})[{idx}]'
        self.base_addr = to_address(
            self.exe_ctx.target.EvaluateExpression(index_exp))
        print(index_exp)

    def _field_expr(self, field):
        ptr_exp = f'(({self.typename} *){self.base_addr})->{field}'
        return self.exe_ctx.target.EvaluateExpression(ptr_exp)

    def get_match_function_args(self):
        pass

    @property
    def kind(self):
        return self._field_expr('kind').data.sint32[0]

    @property
    def pattern(self):
        err = lldb.SBError()
        kore = self.exe_ctx.process.ReadCStringFromMemory(
            to_address(self._field_expr('pattern')), 65536, err)
        return pretty_print_kore(kore.encode(ENCODING), self.exe_ctx.target).strip()

    @property
    def sort(self):
        err = lldb.SBError()
        return self.exe_ctx.process.ReadCStringFromMemory(
            to_address(self._field_expr('sort')), 65536, err)

    @property
    def subject(self):
        cast_to = self.exe_ctx.target.FindFirstType(self.sort)
        print(cast_to)
        return self._field_expr('subject').Cast(cast_to)

    @property
    def debug_name(self):
        err = lldb.SBError()
        return self.exe_ctx.process.ReadCStringFromMemory(
            to_address(self._field_expr('debugName')), 65536, err)

    @property
    def function(self):
        return lookup_function(self.exe_ctx, self.debug_name)


class RuleMatcher:
    SUCCESS = 0
    FUNCTION = 1
    FAIL = 2

    def __init__(self, exe_ctx):
        self.exe_ctx = exe_ctx

    def _reset_match_reason(self):
        target_call(self.exe_ctx, 'resetMatchReason', 'void')

    def _try_match(self, rule_name, subject):
        expr = self.exe_ctx.target.EvaluateExpression(subject)  # check eval?
        target_call(self.exe_ctx, f'{rule_name}.match',
                    'void', ['block *'], [expr])

    def _get_match_log_size(self):
        return target_call(self.exe_ctx, 'getMatchLogSize', 'size_t').data.uint64[0]

    def _get_match_log(self):
        return target_call(self.exe_ctx, 'getMatchLog', 'MatchLog *')

    def __call__(self, rule_name, subject):
        self._reset_match_reason()
        self._try_match(rule_name, subject)

        log = self._get_match_log()
        for i in range(self._get_match_log_size()):
            entry = LogEntry(self.exe_ctx, log, i)
            if entry.kind == self.SUCCESS:
                print('Match succeeds')
            elif entry.kind == self.FUNCTION:
                print(entry.debug_name)
                for t in entry.function.type.GetFunctionArgumentTypes():
                    print(f'  {t.GetName()}')
            elif entry.kind == self.FAIL:
                print('Subject:')
                print(block_summary(entry.subject).strip())
                print('does not match pattern:')
                print(entry.pattern)
            else:
                raise RuntimeError(f'Invalid match type: {entry.kind}')


class MatchCommand:
    program = 'match'

    @ classmethod
    def register_lldb_command(cls, debugger, container, module_name):
        command = f'command script add -o -c {module_name}.{cls.__name__} {container} {cls.program}'
        debugger.HandleCommand(command)

    @ classmethod
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

    def __call__(self, debugger, command, exe_ctx, result):
        args = self.parser.parse_args(shlex.split(command))
        matcher = RuleMatcher(exe_ctx)
        matcher(args.rule, args.term)


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
