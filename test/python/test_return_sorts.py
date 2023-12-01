# RUN: mkdir -p %t
# RUN: export IN=$(realpath Inputs/sorts.kore)
# RUN: cd %t && %kompile "$IN" python --python %py-interpreter --python-output-dir .
# RUN: KLLVM_DEFINITION=%t %python -u %s

from test_bindings import kllvm

import unittest

class TestReturnSorts(unittest.TestCase):

    def _check_sort(self, label, sort):
        self.assertEqual(kllvm.runtime.return_sort_for_label(label), sort)

    def test_function(self):
        self._check_sort('Lblfunc{}', 'SortInt{}')

    def test_constructor(self):
        self._check_sort('Lblfoo{}', 'SortFoo{}')

    def test_subsort(self):
        self._check_sort('Lblbar{}', 'SortBar{}')


if __name__ == "__main__":
    unittest.main()
