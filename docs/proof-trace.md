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

Here is a BNF styled description of the format:
```
proof_trace ::= event*

event       ::= hook
              | function
              | rule
              | config

argument    ::= hook
              | function
              | rule
              | kore_term

name        ::= string
location    ::= string
function    ::= WORD(0xDD) name location arg* WORD(0x11)

hook        ::= WORD(0xAA) name arg* WORD(0xBB) kore_term

ordinal     ::= uint64
arity       ::= uint64
variable    ::= name kore_term WORD(0xCC)
rule        ::= ordinal arity variable*

config      ::= WORD(0xFF) kore_term WORD(0xCC)

string      ::= <c-style null terminated string>
uint64      ::= <64-bit unsigned little endian integer>
```

## Notes

- The `rule_arity` should be used to determine how many variable substitutions
  to read.
- Events at the beginning of the trace (i.e. before the first `config` event)
  are related to configuration initialization.
- The `relative_position` is a null terminated string of positive integers
  separated by `:` (ie. `0:1:1`)
