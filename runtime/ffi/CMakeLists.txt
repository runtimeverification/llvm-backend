set(LLVM_REQUIRES_EH ON)

add_library(ffi STATIC
  ffi.cpp
)

install(
  TARGETS ffi
  ARCHIVE DESTINATION lib/kllvm
)
