import lldb
import sys
import inspect


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

    @classmethod
    def register_lldb_summary(cls, debugger, module_name):
        command = f'type summary add -F {module_name}.{cls.__name__}.summary {cls.typename}'
        debugger.HandleCommand(command)

    @staticmethod
    def summary(value, unused):
        return f'<block@{value.addr}>'


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
