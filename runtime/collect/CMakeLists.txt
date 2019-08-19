set(LLVM_REQUIRES_EH ON)

add_library(collect STATIC
  collect.c
  migrate_roots.cpp
)

install(
  TARGETS collect
  ARCHIVE DESTINATION lib/kllvm
)
