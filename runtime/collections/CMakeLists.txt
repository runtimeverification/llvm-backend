add_library(collections STATIC
  kelemle.cpp
  lists.cpp
  maps.cpp
  rangemaps.cpp
  sets.cpp
  hash.cpp
)

install(
  TARGETS collections
  ARCHIVE DESTINATION lib/kllvm
)

target_link_libraries(collections
  numeric_strings fmt::fmt-header-only)
