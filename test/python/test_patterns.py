# RUN: %python %s

from test_bindings import kllvm, input_path

import unittest


class TestPatterns(unittest.TestCase):

    def test_file_load(self):
        pat1 = kllvm.ast.Pattern(input_path("composite.in"))
        self.assertEqual(str(pat1), "A{}(B{}(),C{}())")

        pat2 = kllvm.ast.Pattern(input_path("string.in"))
        self.assertEqual(str(pat2), '"string pattern"')

        pat3 = kllvm.ast.Pattern(input_path("var.in"))
        self.assertEqual(str(pat3), "XYZ : ABC")

    def test_composite(self):
        pat = kllvm.ast.CompositePattern("ABC")
        self.assertEqual(str(pat), "ABC{}()")

        pat.add_argument(kllvm.ast.CompositePattern("X"))
        self.assertEqual(str(pat), "ABC{}(X{}())")
        self.assertEqual(str(pat.arguments[0]), "X{}()")

        pat.add_argument(kllvm.ast.VariablePattern(
            "Z", kllvm.ast.CompositeSort("S")))
        self.assertEqual(str(pat), "ABC{}(X{}(),Z : S{})")
        self.assertEqual(str(pat.arguments[1]), "Z : S{}")

        subbed = pat.substitute({"Z": kllvm.ast.CompositePattern("Target")})
        self.assertEqual(str(subbed), "ABC{}(X{}(),Target{}())")

    def test_string(self):
        pat = kllvm.ast.StringPattern("abc")
        self.assertEqual(str(pat), '"abc"')
        self.assertEqual(pat.contents, "abc")

    def test_variable(self):
        pat = kllvm.ast.VariablePattern("A", kllvm.ast.CompositeSort("S"))
        sub = {"A": kllvm.ast.CompositePattern("Target")}
        self.assertEqual(str(pat.substitute(sub)), "Target{}()")


if __name__ == "__main__":
    unittest.main()
