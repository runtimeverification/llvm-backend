set(LLVM_REQUIRES_EH ON)

add_kllvm_runtime_library(strings STATIC
  strings.cpp
  bytes.cpp
)

