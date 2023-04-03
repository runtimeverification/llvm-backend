# RUN: mkdir -p %t
# RUN: export IN=$(realpath Inputs/test_steps.kore)
# RUN: cd %t && %kompile "$IN" python --python %py-interpreter --python-output-dir .
# RUN: KLLVM_DEFINITION=%t %python -u %s

from test_bindings import kllvm

import unittest

"""
module STEPS
  imports INT

  syntax Foo ::= foo(Int) | bar()
  rule foo(I) => foo(I -Int 1) requires I >Int 0
  rule foo(_) => bar() [owise]
endmodule
"""


def start_pattern():
    """
    <k> foo(100) </k>
    """
    text = "LblinitGeneratedTopCell{}(Lbl'Unds'Map'Unds'{}(Lbl'Stop'Map{}(),Lbl'UndsPipe'-'-GT-Unds'{}(inj{SortKConfigVar{}, SortKItem{}}(\\dv{SortKConfigVar{}}(\"$PGM\")),inj{SortFoo{}, SortKItem{}}(inj{SortFoo{}, SortKItem{}}(Lblfoo'LParUndsRParUnds'STEPS'Unds'Foo'Unds'Int{}(\\dv{SortInt{}}(\"100\")))))))"
    return kllvm.parser.Parser.from_string(text).pattern()


def foo_output(n):
    """
    <k> foo(n) </k>
    """
    return f"Lbl'-LT-'generatedTop'-GT-'{{}}(Lbl'-LT-'k'-GT-'{{}}(kseq{{}}(inj{{SortFoo{{}}, SortKItem{{}}}}(Lblfoo'LParUndsRParUnds'STEPS'Unds'Foo'Unds'Int{{}}(\dv{{SortInt{{}}}}(\"{100-n}\"))),dotk{{}}())),Lbl'-LT-'generatedCounter'-GT-'{{}}(\dv{{SortInt{{}}}}(\"0\")))"


def bar_output():
    """
    <k> bar() </k>
    """
    return "Lbl'-LT-'generatedTop'-GT-'{}(Lbl'-LT-'k'-GT-'{}(kseq{}(inj{SortFoo{}, SortKItem{}}(Lblbar'LParRParUnds'STEPS'Unds'Foo{}()),dotk{}())),Lbl'-LT-'generatedCounter'-GT-'{}(\dv{SortInt{}}(\"0\")))"


class TestSteps(unittest.TestCase):

    def test_steps_1(self):
        t = kllvm.runtime.Term(start_pattern())

        t.step(0)
        self.assertEqual(str(t), foo_output(0))
        t.step()
        t.step()
        self.assertEqual(str(t), foo_output(2))
        t.step(200)
        self.assertEqual(str(t), bar_output())

    def test_steps_2(self):
        t = kllvm.runtime.Term(start_pattern())

        self.assertEqual(str(t), foo_output(0))
        t.step(50)
        self.assertEqual(str(t), foo_output(50))
        t.step(-1)
        self.assertEqual(str(t), bar_output())

    def test_steps_3(self):
        t = kllvm.runtime.Term(start_pattern())
        t.run()
        self.assertEqual(str(t), bar_output())

    def test_steps_to_pattern(self):
        t = kllvm.runtime.Term(start_pattern())
        t.run()
        pat = t.to_pattern()
        self.assertEqual(str(pat), bar_output())


if __name__ == "__main__":
    unittest.main()
