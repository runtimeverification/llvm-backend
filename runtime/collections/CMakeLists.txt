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

target_link_libraries(collections
  numeric_strings fmt::fmt-header-only)
