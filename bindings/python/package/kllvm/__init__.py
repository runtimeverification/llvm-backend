from ._kllvm import *

import importlib
import sys


# We can't achieve this API purely using the Pybind-generated code, so we need
# to wrap the internal Term class up into a wrapper here by injecting the
# runtime module.
def make_term_class(mod):
    class Term:
        def __init__(self, pattern):
            self._block = mod.InternalTerm(pattern)

        def __str__(self):
            return str(self._block)

        def step(self, n=1):
            self._block = self._block.step(n)

        def run(self):
            self.step(-1)

        def copy(self):
            other = self
            other._block = self._block.step(0)
            return other

        def to_pattern(self):
            return self._block.to_pattern()

        def serialize(self, emit_size=False):
            return self._block.serialize(emit_size=emit_size)

        # Used to implement backend integration tests; should not be bound
        # onwards to Pyk without rethinking the underlying API.
        def _serialize_raw(self, filename, sort):
            self._block._serialize_raw(filename, sort)

        @staticmethod
        def deserialize(bs):
            return mod.InternalTerm.deserialize(bs)

    return Term


def load_definition(path):
    global runtime
    old_path = sys.path
    sys.path = [path]
    runtime = importlib.import_module("_kllvm_runtime")
    runtime.Term = make_term_class(runtime)
    sys.path = old_path
