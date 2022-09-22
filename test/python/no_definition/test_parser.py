# RUN: %python %s

from test_bindings import kllvm, input_path

import unittest


class TestParser(unittest.TestCase):

    def test_file(self):
        parser = kllvm.parser.Parser(input_path("composite.in"))
        pat = parser.pattern()
        self.assertEqual(str(pat), "A{}(B{}(),C{}())")

    def test_string(self):
        pat = kllvm.parser.Parser.from_string("A{}(X:S, Y:Z, Int{}()")
        print(pat)


if __name__ == "__main__":
    unittest.main()
