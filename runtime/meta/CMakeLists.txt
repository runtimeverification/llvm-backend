add_library(meta STATIC
  substitution.cpp
  ffi.cpp
)

target_link_libraries(meta
  fmt::fmt-header-only)

install(
  TARGETS meta
  ARCHIVE DESTINATION lib/kllvm
)
