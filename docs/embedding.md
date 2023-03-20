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
programmatically manipulate the KORE data structures in the context of a larger
tool. For example, the C bindings can be used to implement efficient evaluation
of concrete side conditions for symbolic terms.

### Python

**Note:** for specific API documentation for the backend's Python bindings,
please visit the [Pyk][pyk] repository.

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

For languages other than Python, the backend provides bindings to C that can be
consumed from any language with a C FFI. Generating the C bindings requires a
KORE definition; the bindings include features for both KORE AST manipulation
and concrete execution of KORE terms. To generate C bindings from a definition:
```console
$ llvm-kompile definition.kore dt_dir c -- -o liblang.so
```

The resulting shared library can either be linked into a C or C++ program at
compile time, or dynamically loaded at runtime with `dlopen`.

#### API

The API for the C bindings is installed at [`include/kllvm-c/kllvm-c.h`][header]
when the backend is built. This header fully specifies the public C API exposed
by the library. For full details of anything mentioned in this section, please
consult that header; the examples in this document are currently a high-level
overview of how to use the bindings, rather than a detailed reference.

At a high level, the C bindings implement opaque wrappers over the internal KORE
AST data structures (sorts, patterns, symbols) and runtime term blocks.

Instances of these opaque wrappers are created by a family of `*_new` functions;
callers are responsible for cleaning up by calling the corresponding `*_free`
function on the returned object when its lifetime ends. Failing to do so will
leak memory. For example, to create and then immediately free an object
representing the `Int` sort:
```c
kore_sort *int_sort = kore_composite_sort_new("SortInt");
kore_sort_free(int_sort);
```

The API contains functions to manipulate the KORE AST. For example, to construct
the pattern `a(b())` in KORE:
```c
kore_pattern *b = kore_composite_pattern_new("Lblb");
kore_pattern *a = kore_composite_pattern_new("Lbla");
kore_composite_pattern_add_argument(a, b);
kore_pattern_free(b); // a is valid here as it holds an internal reference to b
// use a ...
kore_pattern_free(a);
```

Limited support for concrete execution is provided by this API. Currently, only
evaluation of KORE function terms is implemented via the `kore_simplify_*`
functions.

## Static Library

If an application does not require KORE AST manipulation, but needs to perform
concrete rewriting of KORE terms beyond function evaluation, it may be more
appropriate to link the generated interpreter directly into your application and
provide your own entrypoint.

To build an object file that contains a compiled definition's full generated
rewriting code, `llvm-kompile` can be invoked as:
```console
$ llvm-kompile definition.kore dt_dir library -c -- -o interpreter.o
```

The generated object file can be linked into any larger application's build
system at this point. However, it does not have a definition of `main`, and so
an application that embeds the interpreter must supply one. The [default
implementations][mains] used by `llvm-kompile` in `main` mode are a useful
reference for what steps need to be taken to interpret a KORE term:
* Extract information from command line arguments
* Parse (or deserialize) a textual (or binary) KORE term from a file
* Instantiate an initial runtime term from the parsed KORE
* Take rewriting steps
* Print the final KORE term.

The API available to applications using interpreter object files in this way is
the [full internal API][runtime-header] used by the LLVM backend. Currently,
this interface is not fully documented, and many details relevant only to
internal usage are exposed. Additionally, several important functions are not
listed in this header:
* `void initStaticObjects(void)`
* `block *take_steps(uint64_t steps, block *term)`
* `block *k_step(block *term)`

Improving this API documentation for end users is a priority for the LLVM
backend.

[pybind]: https://github.com/pybind/pybind11
[pyk]: https://github.com/runtimeverification/pyk
[header]: https://github.com/runtimeverification/llvm-backend/blob/master/bindings/c/include/kllvm-c/kllvm-c.h
[mains]: https://github.com/runtimeverification/llvm-backend/tree/master/runtime/main
[runtime-header]: https://github.com/runtimeverification/llvm-backend/blob/master/include/runtime/header.h
