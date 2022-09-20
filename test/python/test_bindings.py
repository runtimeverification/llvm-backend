import glob
import importlib
import os
import sys


def __load_bindings(path):
    old_path = sys.path
    sys.path = [path]
    kllvm = importlib.import_module('kllvm')
    sys.path = old_path
    return kllvm


kllvm = __load_bindings(os.environ['BINDINGS_PATH'])
