add_library(AST
  AST.cpp
  attribute_set.cpp
  pattern_matching.cpp
  util.cpp
  definition.cpp
)

target_link_libraries(AST
  PUBLIC fmt::fmt-header-only Parser BinaryKore)

install(
  TARGETS AST
  ARCHIVE DESTINATION lib/kllvm
)
