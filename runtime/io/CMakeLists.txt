set(LLVM_REQUIRES_EH ON)

add_library(io STATIC
  io.cpp
)

install(
  TARGETS io
  ARCHIVE DESTINATION lib/kllvm
)
