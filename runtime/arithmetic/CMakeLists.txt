set(LLVM_REQUIRES_EH ON)

add_kllvm_runtime_library(arithmetic STATIC
  int.cpp
)

