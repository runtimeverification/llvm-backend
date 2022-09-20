# RUN: %python %s

from test_bindings import kllvm

import unittest


class TestSymbols(unittest.TestCase):

    def test_str(self):
        sym = kllvm.ast.Symbol("Lbl'Plus")
        self.assertEqual(str(sym), "Lbl'Plus{}")

        sym.add_argument(kllvm.ast.CompositeSort("X"))
        self.assertEqual(str(sym), "Lbl'Plus{}")


class TestVariables(unittest.TestCase):

    def test_name(self):
        a = kllvm.ast.Variable("A")
        self.assertEqual(a.name, "A")

    def test_str(self):
        a = kllvm.ast.Variable("XYZ")
        self.assertEqual(str(a), "XYZ")


if __name__ == "__main__":
    unittest.main()
