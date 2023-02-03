# RUN: mkdir -p %t
# RUN: export IN=$(realpath Inputs/test_labels.kore)
# RUN: cd %t && %kompile "$IN" python --python %py-interpreter
# RUN: KLLVM_DEFINITION=%t %python %s

from test_bindings import kllvm, input_path

import unittest


def make_label(ctor):
    return f"Lbl{ctor}'LParRParUnds'LABELS'Unds'Foo"


class TestTermConstruct(unittest.TestCase):

    def test_construct(self):
        """
        syntax Foo ::= one() | two() | three()
        """
        for ctor in ["one", "two", "three"]:
            pat = kllvm.ast.CompositePattern(make_label(ctor))
            term = kllvm.runtime.Term(pat)
            self.assertEqual(str(pat), str(term))


if __name__ == "__main__":
    unittest.main()
