# RUN: %python %s

from test_bindings import kllvm, input_path

import unittest


class TestParser(unittest.TestCase):

    def test_file(self):
        parser = kllvm.parser.Parser(input_path("composite.in"))
        pat = parser.pattern()
        self.assertEqual(str(pat), "A{}(B{}(),C{}())")

    def test_string(self):
        pat = kllvm.parser.Parser.from_string(
            "A{}(X:S, Y:Z, Int{}())").pattern()
        self.assertEqual(str(pat), "A{}(X : S,Y : Z,Int{}())")

    def test_composite_sort(self):
        sort = kllvm.parser.Parser.from_string("SortInt{}").sort()
        self.assertEqual(str(sort), 'SortInt{}')

    def test_sort_variable(self):
        sort = kllvm.parser.Parser.from_string('X { A }').sort()
        self.assertEqual(str(sort), 'X{A}')


if __name__ == "__main__":
    unittest.main()
