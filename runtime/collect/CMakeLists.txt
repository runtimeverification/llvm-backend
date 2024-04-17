add_library(collect STATIC
  collect.cpp
  migrate_static_roots.cpp
  migrate_collection.cpp
)

install(
  TARGETS collect
  ARCHIVE DESTINATION lib/kllvm
)
