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


kllvm = __load_bindings(os.environ['BINDINGS_INSTALL_PATH'])

__INPUTS = os.path.join(os.path.dirname(os.path.realpath(__file__)), "Inputs")


def input_path(name):
    return os.path.join(__INPUTS, name)
