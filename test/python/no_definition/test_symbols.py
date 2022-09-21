# RUN: %python %s

from test_bindings import kllvm

import unittest


class TestSymbols(unittest.TestCase):

    def test_str(self):
        sym = kllvm.ast.Symbol("Lbl'Plus")
        self.assertEqual(str(sym), "Lbl'Plus{}")

        sym.add_formal_argument(kllvm.ast.CompositeSort("A"))
        self.assertEqual(str(sym), "Lbl'Plus{A{}}")

    def test_equal(self):
        a1 = kllvm.ast.Symbol("A")
        a2 = kllvm.ast.Symbol("A")
        self.assertEqual(a1, a2)
        self.assertIsNot(a1, a2)

        b1 = kllvm.ast.Symbol("B")
        self.assertNotEqual(a1, b1)


class TestVariables(unittest.TestCase):

    def test_name(self):
        a = kllvm.ast.Variable("A")
        self.assertEqual(a.name, "A")

    def test_str(self):
        a = kllvm.ast.Variable("XYZ")
        self.assertEqual(str(a), "XYZ")


if __name__ == "__main__":
    unittest.main()
