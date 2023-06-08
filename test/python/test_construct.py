# RUN: mkdir -p %t
# RUN: export IN=$(realpath Inputs/labels.kore)
# RUN: cd %t && %kompile "$IN" python --python %py-interpreter --python-output-dir .
# RUN: KLLVM_DEFINITION=%t %python %s

from test_bindings import kllvm, input_path

import unittest


def make_label(ctor):
    return f"Lbl{ctor}'LParRParUnds'LABELS'Unds'Foo"


class TestTermConstruct(unittest.TestCase):

    def _pattern_to_term(self, pattern):
        term = kllvm.runtime.Term(pattern)
        self.assertEqual(str(pattern), str(term))

    def _term_to_pattern(self, term):
        pattern = term.to_pattern()
        self.assertEqual(str(term), str(pattern))

    def _pattern_binary_round_trip(self, pattern):
        binary = pattern.serialize()
        back = kllvm.ast.Pattern.deserialize(binary)
        self.assertEqual(str(pattern), str(back))

    def _term_binary_round_trip(self, term):
        binary = term.serialize()
        back = kllvm.runtime.Term.deserialize(binary)
        self.assertEqual(str(term), str(back))

    def test_construct(self):
        """
        syntax Foo ::= one() | two() | three()
        """
        for ctor in ["one", "two", "three"]:
            pat = kllvm.ast.CompositePattern(make_label(ctor))
            term = kllvm.runtime.Term(pat)

            self._pattern_to_term(pat)
            self._term_to_pattern(term)
            self._pattern_binary_round_trip(pat)
            self._term_binary_round_trip(term)

if __name__ == "__main__":
    unittest.main()
