add_library(BindingsCore
  src/core.cpp
)

target_link_libraries(BindingsCore
  PUBLIC AST Parser fmt::fmt-header-only
)

install(
  TARGETS BindingsCore
  ARCHIVE DESTINATION lib/kllvm
)
