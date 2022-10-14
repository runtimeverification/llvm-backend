add_library(strings STATIC
  strings.cpp
  bytes.cpp
)

target_link_libraries(strings
  util)

install(
  TARGETS strings
  ARCHIVE DESTINATION lib/kllvm
)
