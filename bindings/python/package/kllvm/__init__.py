from ._kllvm import *

import importlib
import sys


def load_definition(path):
    global runtime
    old_path = sys.path
    sys.path = [path]
    runtime = importlib.import_module("_kllvm_runtime")
    sys.path = old_path
