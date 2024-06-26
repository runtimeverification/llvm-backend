# Technical Documentation

* [Embedding the LLVM backend in an application](embedding.md)

## Topics to Write

This document is incomplete; these sections provide a rough overview of the
remaining topics that are not yet documented.

### KORE

* High level summary
* Language specification inc. syntax
* Parsing
* [Binary format](binary-kore.md)

### Pattern Matching

* [Existing documentation](../matching/README.md)
* Paper
* Decision tree structure
* Parsing / serializing tree

### Runtime Term Representation

* Blocks / tags / headers
* Hooked / native type representations

### Code Generation and Rewriting

* Emitted functions / lookup tables
* Printing & serialization / visitors
* Rewriting / rule application

### Garbage Collection

* Arenas
* Allocation / collection
* Static terms

### Runtime Library

* Hooks
* Generated vs. LLVM
* Main, search etc.

### Bindings

* C
* Python

### Debugging Support

* GDB script

### Interface

* Scripts installed
* KORE tools
* Ordinal lookups
