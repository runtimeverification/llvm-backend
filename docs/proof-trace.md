# Proof Trace Format

This document describes the format for the binary proof trace that gets emitted
when the `--proof-output` flag gets passed to the interpreter. Note that this trace
format is in its early stages and will probably change quite a bit as development on it
continues.

## Overview

The information presented by the trace starts with the initial configuration of the execution,
followed by a sequence of rewrite steps consisting of which rule applied, the variable substitutions
for that rule, and the resulting configuration after the rewrite, and then finally the configuration
at the end of the execution.

The format of the kore terms themselves are in binary format, and in the proof trace we delimit
them with 64-bit sentinel values of 0xffffffffffffffff at the beginning and 0xcccccccccccccccc
at the end. Recent changes to the binary kore format that include the length of the binary term
have rendered this unnecessary, but the change hasn't been implemented yet.

## Grammar

Here is a BNF styled description of the format

```
delimited_serial_kore := 0xffffffffffffffff serialized_term 0xcccccccccccccccc

null_terminated_name := <c-style null terminated string>
relative_position := [0-9]+(:[0-9]+)* 0x00

function_event := 0xdddddddddddddddd null_terminated_name relative_position

variable := null_terminated_name serialized_term 0xcccccccccccccccc

rule_ordinal := <64-bit unsigned little endian integer>
rule_arity := <64-bit unsigned little endian integer>

rewrite_trace := rule_ordinal rule_arity variable* delimited_serial_kore

initial_config := delimited_serial_kore

proof_trace := initial_config (function_event|rewrite_trace)*
```

## Notes

- The `rule_arity` should be used to determine how many variable substitutions to read
- The serialized term for a variable substitution does not begin with the sentinel delimiter.
  This is because the null terminated variable name can act as the sentinel.
