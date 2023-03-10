import inspect
import lldb
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
        print(f'{self.program} is not implemented')


class StepCommand:
    program = 'step'

    @classmethod
    def register_lldb_command(cls, debugger, container, module_name):
        command = f'command script add -o -c {module_name}.{cls.__name__} {container} {cls.program}'
        debugger.HandleCommand(command)

    def __init__(self, debugger, unused):
        pass

    def __call__(self, debugger, command, exe_ctx, result):
        print(f'{self.program} is not implemented')


class MatchCommand:
    program = 'match'

    @classmethod
    def register_lldb_command(cls, debugger, container, module_name):
        command = f'command script add -o -c {module_name}.{cls.__name__} {container} {cls.program}'
        debugger.HandleCommand(command)

    def __init__(self, debugger, unused):
        pass

    def __call__(self, debugger, command, exe_ctx, result):
        print(f'{self.program} is not implemented')


class BlockSummary:
    typename = 'block'
    debugger = None

    @classmethod
    def register_lldb_summary(cls, debugger, module_name):
        command = f'type summary add -F {module_name}.{cls.__name__}.summary {cls.typename}'
        debugger.HandleCommand(command)

    @staticmethod
    def summary(value, unused):
        kore = term_to_kore(value)
        return pretty_print_kore(read_k_string(kore), value.GetTarget())


def __lldb_init_module(debugger, internal_dict):
    debugger.HandleCommand('command container add -h "K commands" k')

    # Adapted from llvm-project/lldb/examples/python/cmdtemplate.py
    # Register all classes that have a register_lldb_command method
    for _name, cls in inspect.getmembers(sys.modules[__name__]):
        if inspect.isclass(cls):
            if callable(getattr(cls, "register_lldb_command", None)):
                cls.register_lldb_command(debugger, 'k', __name__)

            if callable(getattr(cls, "register_lldb_summary", None)):
                cls.register_lldb_summary(debugger, __name__)
