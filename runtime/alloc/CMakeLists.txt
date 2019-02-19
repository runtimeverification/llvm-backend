add_library(alloc STATIC
  alloc.c
  arena.c
  collect.c
)

install(
  TARGETS alloc
  ARCHIVE DESTINATION lib/kllvm
)
