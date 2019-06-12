set(LLVM_REQUIRES_EH ON)

add_library(alloc STATIC
  alloc.c
  arena.c
  collect.c
  migrate_roots.cpp
)

install(
  TARGETS alloc
  ARCHIVE DESTINATION lib/kllvm
)
