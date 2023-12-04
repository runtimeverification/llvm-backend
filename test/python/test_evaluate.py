# RUN: mkdir -p %t
# RUN: export IN=$(realpath Inputs/evaluate.kore)
# RUN: cd %t && %kompile "$IN" python --python %py-interpreter --python-output-dir .
# RUN: KLLVM_DEFINITION=%t %python -u %s

from test_bindings import kllvm

import unittest


class TestEvaluate(unittest.TestCase):

    def test_ctor(self):
        left = kllvm.parser.Parser.from_string('Lblfoo{}(\\dv{SortInt{}}("23"))').pattern()
        right = kllvm.parser.Parser.from_string('Lblbar{}(\\dv{SortInt{}}("56"))').pattern()

        result = kllvm.runtime.evaluate_function('Lblf{}', [left, right])
        self.assertEqual(str(result), 'Lblfoo{}(\dv{SortInt{}}("79"))')

    def test_int_function(self):
        result = kllvm.runtime.evaluate_function('Lblbaz{}', [])
        self.assertEqual(str(result), '\dv{SortInt{}}("78")')

    def test_bool_function(self):
        arg_t = kllvm.parser.Parser.from_string('\\dv{SortInt{}}("34")').pattern()
        result_t = kllvm.runtime.evaluate_function('Lblqux{}', [arg_t])
        self.assertEqual(str(result_t), '\dv{SortBool{}}("true")')

        arg_f = kllvm.parser.Parser.from_string('\\dv{SortInt{}}("98")').pattern()
        result_f = kllvm.runtime.evaluate_function('Lblqux{}', [arg_f])
        self.assertEqual(str(result_f), '\dv{SortBool{}}("false")')


if __name__ == "__main__":
    unittest.main()
