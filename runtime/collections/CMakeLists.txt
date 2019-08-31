set(LLVM_REQUIRES_EH ON)

add_library(collections STATIC
  lists.cpp
  maps.cpp
  sets.cpp
  hash.cpp
)

install(
  TARGETS collections
  ARCHIVE DESTINATION lib/kllvm
)
