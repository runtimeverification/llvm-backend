# RUN: mkdir -p %t
# RUN: export IN=$(realpath Inputs/evaluate.kore)
# RUN: cd %t && %kompile "$IN" python --python %py-interpreter --python-output-dir .
# RUN: KLLVM_DEFINITION=%t %python -u %s

from test_bindings import kllvm

import unittest


class TestEvaluate(unittest.TestCase):

    def test_eval(self):
        left = kllvm.parser.Parser.from_string('Lblfoo{}(\\dv{SortInt{}}("23"))').pattern()
        right = kllvm.parser.Parser.from_string('Lblbar{}(\\dv{SortInt{}}("56"))').pattern()

        result = kllvm.runtime.evaluate_function('Lblf{}', [left, right])
        print(result)


if __name__ == "__main__":
    unittest.main()
