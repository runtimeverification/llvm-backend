# RUN: mkdir -p %t
# RUN: export IN=$(realpath Inputs/raw-term.kore)
# RUN: cd %t && %kompile "$IN" python --python %py-interpreter --python-output-dir .
# RUN: KLLVM_DEFINITION=%t %python %s

from test_bindings import kllvm, input_path

import tempfile
import unittest


class TestRawTerms(unittest.TestCase):

    def int_token(self, v):
        return kllvm.parser.Parser.from_string(f'\\dv{{SortInt{{}}}}("{v}")').pattern()

    def test_raw_terms(self):
        token = self.int_token(76)
        term = kllvm.runtime.Term(token)

        with tempfile.NamedTemporaryFile() as fp:
            term._serialize_raw(fp.name, "SortInt{}")
            data = fp.read()
            
            with_raw = kllvm.ast.Pattern.deserialize(data, strip_raw_term = False)
            without_raw = kllvm.ast.Pattern.deserialize(data)

            self.assertEqual(str(token), str(without_raw))
            self.assertNotEqual(str(token), str(with_raw))
            self.assertTrue(str(token) in str(with_raw))

if __name__ == "__main__":
    unittest.main()
