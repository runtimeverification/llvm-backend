add_library(alloc STATIC
  alloc.c
)

install(
  TARGETS alloc
  ARCHIVE DESTINATION lib/kllvm
)
