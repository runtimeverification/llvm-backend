add_library(collect STATIC
  collect.cpp
  migrate_roots.cpp
  migrate_collection.cpp
  StackMapParser.cpp
)

install(
  TARGETS collect
  ARCHIVE DESTINATION lib/kllvm
)
