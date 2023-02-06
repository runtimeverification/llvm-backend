# LLVM Backend

## Technical Documentation

* Documentation for all the major components of the backend

### Overview

* Summary of project
* Diagram showing components

### KORE

* High level summary
* Language specification inc. syntax
* Parsing
* [Binary format](binary_kore.md)

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

## Developer Guide

* How do we work on the backend?
* Tools, environment, best practice etc.

### Environment Setup

* [Dependencies](../INSTALL.md)
* Submodules

### Style and Code Checks

* Shellcheck
* Clang format
* IWYU
* Style guides?

### Testing

* Adding tests
* `lit`, `FileCheck` etc.
* Path assumptions

### Using the Backend

* Building a semantics that uses the backend directly
* Embedding / library mode
* Advantages / disadvantages

### Nix

* Differences to traditional setup
