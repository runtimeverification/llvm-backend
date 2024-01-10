add_library(AST
  AST.cpp
  pattern_matching.cpp
)

target_link_libraries(AST
  PUBLIC fmt::fmt-header-only Parser BinaryKore)

install(
  TARGETS AST
  ARCHIVE DESTINATION lib/kllvm
)
