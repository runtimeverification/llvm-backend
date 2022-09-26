# RUN: %python %s

from test_bindings import kllvm

assert kllvm
assert kllvm.ast
