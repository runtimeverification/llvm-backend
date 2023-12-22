# RUN: mkdir -p %t
# RUN: export IN=$(realpath Inputs/simplify.kore)
# RUN: cd %t && %kompile "$IN" python --python %py-interpreter --python-output-dir .
# RUN: KLLVM_DEFINITION=%t %python -u %s

from test_bindings import kllvm

import unittest


class TestSimplify(unittest.TestCase):

    def _check_int_simplification(self, label, value):
        p = kllvm.parser.Parser.from_string(f'Lbl{label}{{}}()').pattern()
        s = kllvm.ast.CompositeSort('SortInt')
        result = kllvm.runtime.simplify_pattern(p, s)
        self.assertEqual(str(result), f'\dv{{SortInt{{}}}}("{value}")')

    def _check_bool_simplification(self, label, value):
        p = kllvm.parser.Parser.from_string(f'Lbl{label}{{}}()').pattern()
        result = kllvm.runtime.simplify_bool_pattern(p)
        self.assertTrue(result == value)

    def test_simplify_foo(self):
        self._check_int_simplification('foo', 328)

    def test_simplify_bar(self):
        self._check_int_simplification('bar', 562)

    def test_simplify_baz(self):
        self._check_bool_simplification('baz', True)

    def test_simplify_qux(self):
        self._check_bool_simplification('qux', False)

if __name__ == "__main__":
    unittest.main()
