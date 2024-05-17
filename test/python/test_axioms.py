# RUN: %python %s

from test_bindings import kllvm, input_path

import unittest

class TestAxioms(unittest.TestCase):

    def test_is_required(self):
        parser = kllvm.parser.Parser(input_path("proof-trace.kore"))
        definition = parser.definition()
        self.assertTrue(definition.modules[1].declarations[4].is_required)
        self.assertTrue(definition.modules[1].declarations[5].is_required)
        self.assertFalse(definition.modules[2].declarations[1].is_required)
        self.assertFalse(definition.modules[4].declarations[94].is_required)
        self.assertFalse(definition.modules[4].declarations[109].is_required)
        self.assertFalse(definition.modules[4].declarations[113].is_required)
        self.assertFalse(definition.modules[4].declarations[129].is_required)
        self.assertFalse(definition.modules[4].declarations[130].is_required)
        self.assertFalse(definition.modules[4].declarations[149].is_required)
        self.assertFalse(definition.modules[4].declarations[180].is_required)
        self.assertTrue(definition.modules[4].declarations[186].is_required)
        self.assertTrue(definition.modules[4].declarations[187].is_required)
        self.assertTrue(definition.modules[4].declarations[188].is_required)


if __name__ == "__main__":
    unittest.main()
