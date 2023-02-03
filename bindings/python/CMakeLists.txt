# We build two libraries from the same source code here so that we can use the
# KLLVM bindings in two different contexts:
#   * Before a definition is kompiled, we'd still like to be able to manipulate
#     KORE ASTs using the LLVM data structures, even though we can't actually
#     execute them yet without the code-generated components.
#   * Once a definition is available, we want to be able to generate runtime
#     structures from an AST and take rewrite steps on that structure.
#
# The static library is designed to be linked with a generated interpreter by
# llvm-kompile to produce a shared library that Python can dynamically load.

pybind11_add_module(kllvm-python-static STATIC NO_EXTRAS
  runtime.cpp
)

pybind11_add_module(_kllvm       SHARED NO_EXTRAS
  ast.cpp
)

target_link_libraries(_kllvm PUBLIC
  AST
  Parser
)

target_link_libraries(kllvm-python-static PUBLIC
  AST
  util
)

install(FILES $<TARGET_FILE:kllvm-python-static>
  DESTINATION lib/kllvm
  RENAME libkllvmpythonruntime.a)

install(FILES
    ast.cpp
    runtime.cpp
  DESTINATION lib/kllvm/python_src)

install(DIRECTORY package/
  DESTINATION lib/kllvm/python)

install(TARGETS _kllvm
  DESTINATION lib/kllvm/python/kllvm
  LIBRARY)
