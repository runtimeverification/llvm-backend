set(LLVM_REQUIRES_EH ON)

add_library(collections STATIC
  lists.cpp
)

install(
  TARGETS collections
  ARCHIVE DESTINATION lib/kllvm
)
