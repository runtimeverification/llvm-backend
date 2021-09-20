add_library(alloc STATIC
  alloc.cpp
  arena.cpp
  register_gc_roots_enum.cpp
)

install(
  TARGETS alloc
  ARCHIVE DESTINATION lib/kllvm
)
