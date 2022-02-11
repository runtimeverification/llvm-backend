import gdb.printing
path = "@INSTALL_DIR_ABS_PATH@/lib/kllvm/gdb"
if not path in sys.path:
    sys.path.append(path)
from kllvm import kllvm_lookup_function
gdb.printing.register_pretty_printer(
    gdb.current_objfile(),
    kllvm_lookup_function)
