# def dispatch_k(debugger, command, result, internal_dict):
#     print('K-specific support for LLDB is not yet implemented')


def __lldb_init_module(debugger, internal_dict):
    print("KLLDB")
    # debugger.HandleCommand("command script add -f klldb.dispatch_k k")
