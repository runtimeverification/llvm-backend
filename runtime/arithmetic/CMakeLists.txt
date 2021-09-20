add_library(arithmetic STATIC
  int.cpp
  float.cpp
)

install(
  TARGETS arithmetic
  ARCHIVE DESTINATION lib/kllvm
)
