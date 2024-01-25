add_library(strings STATIC
  strings.cpp
  bytes.cpp
  copy_on_write.cpp
)

target_link_libraries(strings
  fmt::fmt-header-only
  numeric_strings
  util)

add_library(numeric_strings STATIC
  numeric.cpp
)

install(
  TARGETS strings numeric_strings
  ARCHIVE DESTINATION lib/kllvm
)
