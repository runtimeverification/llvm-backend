# RUN: %python %s

from test_bindings import kllvm

import unittest


class TestSymbols(unittest.TestCase):

    def test_str(self):
        sym = kllvm.ast.Symbol("Lbl'Plus")
        self.assertEqual(str(sym), "Lbl'Plus{}")
        self.assertTrue(sym.is_concrete)
        self.assertFalse(sym.is_builtin)

        sym.add_formal_argument(kllvm.ast.CompositeSort("A"))
        self.assertEqual(str(sym), "Lbl'Plus{A{}}")
        self.assertTrue(sym.is_concrete)
        self.assertFalse(sym.is_builtin)
        self.assertEqual(sym.formal_arguments[0], kllvm.ast.CompositeSort("A"))

    def test_equal(self):
        a1 = kllvm.ast.Symbol("A")
        a2 = kllvm.ast.Symbol("A")
        self.assertEqual(a1, a2)
        self.assertIsNot(a1, a2)

        b1 = kllvm.ast.Symbol("B")
        self.assertNotEqual(a1, b1)

    def test_parse_symbol_name(self):
        sym_base = kllvm.ast.Symbol("Lbl'Plus")
        self.assertEqual(str(sym_base), "Lbl'Plus{}")
        
        parser = kllvm.parser.Parser.from_string(str(sym_base))
        sym_parsed = parser.symbol()
        self.assertEqual(sym_base, sym_parsed)
        
        self.assertEqual(sym_parsed.name, "Lbl'Plus")
        self.assertTrue(sym_parsed.is_concrete)
        self.assertFalse(sym_parsed.is_builtin)

class TestVariables(unittest.TestCase):

    def test_name(self):
        a = kllvm.ast.Variable("A")
        self.assertEqual(a.name, "A")

    def test_str(self):
        a = kllvm.ast.Variable("XYZ")
        self.assertEqual(str(a), "XYZ")


if __name__ == "__main__":
    unittest.main()
