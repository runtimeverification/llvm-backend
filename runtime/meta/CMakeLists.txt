set(LLVM_REQUIRES_EH ON)

add_library(meta STATIC
  substitution.cpp
  ffi.cpp
)

install(
  TARGETS meta
  ARCHIVE DESTINATION lib/kllvm
)
