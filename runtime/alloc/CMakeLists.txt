set(LLVM_REQUIRES_EH ON)

add_library(alloc STATIC
  alloc.cpp
  arena.cpp
  collect.cpp
  migrate_roots.cpp
  register_gc_roots_enum.cpp
)

install(
  TARGETS alloc
  ARCHIVE DESTINATION lib/kllvm
)
