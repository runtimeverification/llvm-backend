# Using the LLVM Backend as a Library

By default, `llvm-kompile` generates a standalone executable from a KORE
definition. These executables take an input file containing a KORE term, apply
rewrite steps (up to an upper bound, or until rewriting finishes), and generate
an output file containing the resulting KORE term. For example:
```console
$ llvm-kompile definition.kore dt_dir main -- -o interpreter
$ ./interpreter in.kore -1 out.kore
```

This mode is used by the K tools (`krun` etc.) to perform concrete execution.
For products that need to be deployed and run separately from the K tool
scripts, the LLVM backend can be used as a library and embedded into application
code directly. The parent application is then responsible for performing some of
the boilerplate code implemented by `krun` and the default interpreter binaries.

Currently, two main ways of embedding the LLVM backend are supported:
* As a dynamic library that contains support for the KORE AST data structures,
  and optionally the generated runtime code for a specific definition.
* As a static library that contains _only_ the generated runtime code for a
  specific definition.

Depending on your specific application's needs, one of these modes will be more
appropriate than the other.

## Shared Library Bindings

The LLVM backend implements two shared library language bindings:
* Python via [`pybind11`][pybind]
* C (and therefore any language with a C FFI)

These bindings are most appropriate for implementing features that
programmatically maniulate the KORE data structures in the context of a larger
tool. For example, the C bindings can be used to implement efficient evaluation
of concrete side conditions for symbolic terms.

### Python

Most uses of the backend's Python bindings should be made via the `pyk.kllvm`
module of the [Pyk library][pyk], rather than directly from your own Python
code. There are Python-specific wrappers in Pyk that make using the generated
bindings simpler and less error-prone. Additionally, Pyk contains utilities that
abstract the low-level details of `llvm-kompile` described in this section.

The Python bindings can be generated in two different ways using `llvm-kompile`.
If no execution of KORE terms is required (i.e. only KORE AST manipulation is
needed), then the `pythonast` mode generates a Python module that does not
depend on a KORE definition:
```console
$ llvm-kompile pythonast
$ ls
_kllvm.cpython-310-x86_64-linux-gnu.so
```

If execution is required, the `python` mode produces a shared library that
includes generated runtime code for a definition:
```console
$ llvm-kompile definition.kore dt_dir python
$ ls
_kllvm_runtime.cpython-310-x86_64-linux-gnu.so
```

Note that the filename of these librarys must be in a particular format for
Python to load the bindings; this means that you **should not use the -o flag**
when generating Python bindings. Instead, use `--python-output-dir` to specify
the directory in which the generated bindings should be placed. If the bindings
need to be compatible with a specific Python interpreter (for example, if you
are building inside Nix), pass `--python PATH` to specify the path to that
Python interpreter.

### C

## Static Library

[pybind]: https://github.com/pybind/pybind11
[pyk]: https://github.com/runtimeverification/pyk
