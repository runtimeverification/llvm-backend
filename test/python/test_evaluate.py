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

        call = kllvm.ast.CompositePattern('Lblf')
        call.add_argument(left)
        call.add_argument(right)

        result = kllvm.runtime.evaluate_function(call)
        self.assertEqual(str(result), 'Lblfoo{}(\dv{SortInt{}}("79"))')

    def test_int_function(self):
        call = kllvm.ast.CompositePattern('Lblbaz')

        result = kllvm.runtime.evaluate_function(call)
        self.assertEqual(str(result), '\dv{SortInt{}}("78")')

    def test_true_function(self):
        arg_t = kllvm.parser.Parser.from_string('\\dv{SortInt{}}("34")').pattern()

        call = kllvm.ast.CompositePattern('Lblqux')
        call.add_argument(arg_t)

        result_t = kllvm.runtime.evaluate_function(call)
        self.assertEqual(str(result_t), '\dv{SortBool{}}("true")')

    def test_false_function(self):
        arg_f = kllvm.parser.Parser.from_string('\\dv{SortInt{}}("98")').pattern()

        call = kllvm.ast.CompositePattern('Lblqux')
        call.add_argument(arg_f)

        result_f = kllvm.runtime.evaluate_function(call)
        self.assertEqual(str(result_f), '\dv{SortBool{}}("false")')


if __name__ == "__main__":
    unittest.main()
