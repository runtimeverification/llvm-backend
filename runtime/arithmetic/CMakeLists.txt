add_library(arithmetic STATIC
  int.cpp
  float.cpp
)

target_link_libraries(arithmetic
  numeric_strings
)

install(
  TARGETS arithmetic
  ARCHIVE DESTINATION lib/kllvm
)
