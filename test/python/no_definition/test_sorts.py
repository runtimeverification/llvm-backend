# RUN: %python %s

from test_bindings import kllvm

import unittest


class TestSorts(unittest.TestCase):

    def test_is_concrete(self):
        a = kllvm.ast.CompositeSort("A")
        self.assertTrue(a.is_concrete)

        b = kllvm.ast.SortVariable("B")
        self.assertFalse(b.is_concrete)

    def test_name(self):
        names = ["A", "SortInt", ""]
        for name in names:
            sort = kllvm.ast.CompositeSort(name)
            self.assertEqual(sort.name, name)

    def test_str(self):
        a = kllvm.ast.CompositeSort("A")
        self.assertEqual(str(a), "A{}")

        b = kllvm.ast.SortVariable("B")
        self.assertEqual(str(b), "B")

    def test_add_argument(self):
        top = kllvm.ast.CompositeSort("Top")
        a = kllvm.ast.CompositeSort("A")
        b = kllvm.ast.SortVariable("B")

        top.add_argument(a)
        top.add_argument(b)

        self.assertEqual(str(top), "Top{A{},B}")

    def test_substitution(self):
        a = kllvm.ast.SortVariable("A")
        target = kllvm.ast.CompositeSort("Target")
        self.assertEqual(str(a.substitute({a: target})), str(target))

        # Construct "Top{First{X,X,A{},Y}}"
        top = kllvm.ast.CompositeSort("Top")
        first = kllvm.ast.CompositeSort("First")
        x = kllvm.ast.SortVariable("X")
        a = kllvm.ast.CompositeSort("A")
        y = kllvm.ast.SortVariable("Y")

        first.add_argument(x)
        first.add_argument(x)
        first.add_argument(a)
        first.add_argument(y)

        top.add_argument(first)

        # Sanity check
        self.assertEqual(str(top), "Top{First{X,X,A{},Y}}")
        self.assertFalse(top.is_concrete)

        left = kllvm.ast.CompositeSort("Left")
        right = kllvm.ast.CompositeSort("Right")

        subbed = top.substitute({x: left, y: right})
        self.assertEqual(str(subbed), "Top{First{Left{},Left{},A{},Right{}}}")
        self.assertTrue(subbed.is_concrete)

    def test_equality(self):
        a1 = kllvm.ast.CompositeSort("A")
        a2 = kllvm.ast.CompositeSort("A")
        self.assertIsNot(a1, a2)
        self.assertEqual(a1, a2)

        b1 = kllvm.ast.SortVariable("B")
        b2 = kllvm.ast.SortVariable("B")
        self.assertIsNot(b1, b2)
        self.assertEqual(b1, b2)

        self.assertNotEqual(a1, b1)
        self.assertNotEqual(a2, b2)


if __name__ == "__main__":
    unittest.main()
