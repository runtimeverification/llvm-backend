# RUN: mkdir -p %t
# RUN: export KORE_DEF=$(realpath Inputs/proof-trace.kore)
# RUN: export IN=$(realpath Inputs/proof-trace.in)
# RUN: cd %t && %kompile "$KORE_DEF" main --proof-hint-instrumentation -o interpreter
# RUN: rm -f proof_trace.bin && ./interpreter "$IN" -1 proof_trace.bin --proof-output
# RUN: kore-rich-header "$KORE_DEF" > %t/header.bin


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
            "Output", "test_proof_trace.py.tmp", "proof_trace.bin")
        binary_header_path = os.path.join(
            os.path.dirname(os.path.realpath(__file__)),
            "Output", "test_proof_trace.py.tmp", "header.bin")
        header = kllvm.prooftrace.kore_header(binary_header_path)

        definition_file = os.path.join(
            os.path.dirname(os.path.realpath(__file__)),
            "Inputs", "proof-trace.kore")

        with open(definition_file, 'rb') as f:
            data = f.read()
            definition = kllvm.parser.Parser.from_string(data).definition()

            definition.preprocess()
            definition_text = repr(definition).split("\n")

        with open(binary_proof_trace, 'rb') as f:
           data = f.read()
           trace = kllvm.prooftrace.llvm_rewrite_trace.parse(data, header)
           self.assertFalse(trace is None)

           # check that there is a initial configuration
           self.assertTrue(trace.initial_config.is_kore_pattern())

           # check that the trace after the initial configuration is 3 events long
           self.assertEqual(len(trace.trace), 3)

           # check that the first event is the rewrite a() => b()
           self.assertTrue(trace.trace[0].is_step_event())
           rule_ordinal = trace.trace[0].step_event.rule_ordinal
           axiom = repr(definition.get_axiom_by_ordinal(rule_ordinal))
           axiom_expected = self.get_pattern_from_ordinal(definition_text, rule_ordinal)
           self.assertEqual(axiom, axiom_expected)

           # check that the second event is the rewrite b() => c()
           self.assertTrue(trace.trace[1].is_step_event())
           rule_ordinal = trace.trace[1].step_event.rule_ordinal
           axiom = repr(definition.get_axiom_by_ordinal(rule_ordinal))
           axiom_expected = self.get_pattern_from_ordinal(definition_text, rule_ordinal)
           self.assertEqual(axiom, axiom_expected)

           # check that the third event is a configuration
           self.assertTrue(trace.trace[2].is_kore_pattern())

        it = kllvm.prooftrace.llvm_rewrite_trace_iterator.from_file(binary_proof_trace, header)

        while True:
            event0 = it.get_next_event()
            if event0.type != kllvm.prooftrace.EventType.PreTrace:
                break

        self.assertEqual(event0.type, kllvm.prooftrace.EventType.InitialConfig)
        self.assertTrue(event0.event.is_kore_pattern())

        event1 = it.get_next_event()
        self.assertEqual(event1.type, kllvm.prooftrace.EventType.Trace)
        self.assertTrue(event1.event.is_step_event())
        rule_ordinal = event1.event.step_event.rule_ordinal
        axiom = repr(definition.get_axiom_by_ordinal(rule_ordinal))
        axiom_expected = self.get_pattern_from_ordinal(definition_text, rule_ordinal)
        self.assertEqual(axiom, axiom_expected)

        event2 = it.get_next_event()
        self.assertEqual(event2.type, kllvm.prooftrace.EventType.Trace)
        self.assertTrue(event2.event.is_step_event())
        rule_ordinal = event2.event.step_event.rule_ordinal
        axiom = repr(definition.get_axiom_by_ordinal(rule_ordinal))
        axiom_expected = self.get_pattern_from_ordinal(definition_text, rule_ordinal)
        self.assertEqual(axiom, axiom_expected)

        event3 = it.get_next_event()
        self.assertEqual(event3.type, kllvm.prooftrace.EventType.Trace)
        self.assertTrue(event3.event.is_kore_pattern())

        self.assertEqual(it.get_next_event(), None)


if __name__ == "__main__":
    unittest.main()
