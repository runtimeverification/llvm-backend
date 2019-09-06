set(LLVM_REQUIRES_EH ON)

add_library(collect STATIC
  collect.cpp
  migrate_roots.cpp
  migrate_collection.cpp
)

install(
  TARGETS collect
  ARCHIVE DESTINATION lib/kllvm
)
