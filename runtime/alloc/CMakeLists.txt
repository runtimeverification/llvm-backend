add_library(alloc STATIC
  alloc.c
  collect.c
)

install(
  TARGETS alloc
  ARCHIVE DESTINATION lib/kllvm
)
