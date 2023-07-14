# RUN: mkdir -p %t
# RUN: export IN=$(realpath Inputs/labels.kore)
# RUN: cd %t && %kompile "$IN" python --python %py-interpreter --python-output-dir .
# RUN: KLLVM_DEFINITION=%t %python %s

from test_bindings import kllvm, input_path

import unittest
import tempfile


def pattern():
    label = f"Lblone'LParRParUnds'LABELS'Unds'Foo"
    return kllvm.ast.CompositePattern(label)


class TestTermReadFromFile(unittest.TestCase):

    def setUp(self):
        self.temp_file = tempfile.NamedTemporaryFile()
        self.start_garbage = b'start_garbage'
        self.end_garbage = b'end_garbage'

        with open(self.temp_file.name, 'wb') as f:
            f.write(self.start_garbage)
            f.write(pattern().serialize(emit_size=True))
            f.write(self.end_garbage)

    def test_read_from_file(self):
        with open(self.temp_file.name, 'rb') as f:
            self.assertEqual(f.read(len(self.start_garbage)), self.start_garbage)

            file_pat = kllvm.ast.Pattern.read_from(f)
            self.assertEqual(str(file_pat), str(pattern()))

            self.assertEqual(f.read(), self.end_garbage)

if __name__ == "__main__":
    unittest.main()
