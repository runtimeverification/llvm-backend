# RUN: mkdir -p %t
# RUN: export KORE_DEF=$(realpath Inputs/proof-trace.kore)
# RUN: export IN=$(realpath Inputs/proof-trace.in)
# RUN: cd %t && %kompile "$KORE_DEF" main --proof-hint-instrumentation-slow -o interpreter
# RUN: rm -f proof_trace.bin && ./interpreter "$IN" -1 proof_trace.bin --proof-output


# RUN: %python %s

from test_bindings import kllvm

import os
import unittest


class TestParser(unittest.TestCase):
    def get_pattern_from_ordinal(self, definition_text, ordinal):
        axiom_ordinal = 'ordinal{}("' + str(ordinal) + '")'
        line = next((i+1 for i, l in enumerate(definition_text) if axiom_ordinal in l), None)
        return definition_text[line-1].strip()

    def test_file(self):
        binary_proof_trace = os.path.join(
            os.path.dirname(os.path.realpath(__file__)),
            "Output", "test_proof_trace_slow.py.tmp", "proof_trace.bin")
        definition_file = os.path.join(
            os.path.dirname(os.path.realpath(__file__)),
            "Inputs", "proof-trace.kore")
       
        with open(definition_file, 'rb') as f:
            data = f.read()
            definition = kllvm.parser.Parser.from_string(data).definition()
        
            definition.preprocess()
            definition_text = definition.__repr__().split("\n")
        
        with open(binary_proof_trace, 'rb') as f:
           data = f.read()
           trace = kllvm.prooftrace.llvm_rewrite_trace.parse(data)
           self.assertFalse(trace is None)

           # check that there is a initial configuration
           self.assertTrue(trace.initial_config.is_kore_pattern())

           # check that the trace after the initial configuration is 4 events long
           self.assertEqual(len(trace.trace), 4)

           # check that the first event is the rewrite a() => b()
           self.assertTrue(trace.trace[0].is_step_event())
           rule_ordinal = trace.trace[0].step_event.rule_ordinal
           axiom = definition.get_axiom_by_ordinal(rule_ordinal).__repr__()
           axiom_expected = self.get_pattern_from_ordinal(definition_text, rule_ordinal)
           self.assertEqual(axiom, axiom_expected)

           # check that the second event is a configuration
           self.assertTrue(trace.trace[1].is_kore_pattern())

           # check that the third event is the rewrite b() => c()
           self.assertTrue(trace.trace[2].is_step_event())
           rule_ordinal = trace.trace[2].step_event.rule_ordinal
           axiom = definition.get_axiom_by_ordinal(rule_ordinal).__repr__()
           axiom_expected = self.get_pattern_from_ordinal(definition_text, rule_ordinal)
           self.assertEqual(axiom, axiom_expected)

           # check that the fourth event is a configuration
           self.assertTrue(trace.trace[3].is_kore_pattern())


if __name__ == "__main__":
    unittest.main()
