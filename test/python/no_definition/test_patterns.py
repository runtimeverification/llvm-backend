# RUN: %python %s

from test_bindings import kllvm

import os
import unittest

INPUTS = os.path.join(os.path.dirname(os.path.realpath(__file__)), "Inputs")


def input_path(name):
    return os.path.join(INPUTS, name)


class TestPatterns(unittest.TestCase):

    def test_file_load(self):
        pat1 = kllvm.ast.Pattern(input_path("composite.in"))
        self.assertEqual(str(pat1), "A{}(B{}(),C{}())")

        pat2 = kllvm.ast.Pattern(input_path("string.in"))
        self.assertEqual(str(pat2), '"string pattern"')

        pat3 = kllvm.ast.Pattern(input_path("var.in"))
        self.assertEqual(str(pat3), "XYZ : ABC")


if __name__ == "__main__":
    unittest.main()
