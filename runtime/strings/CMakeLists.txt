set(LLVM_REQUIRES_EH ON)

add_library(strings STATIC
  strings.cpp
  bytes.cpp
)

install(
  TARGETS strings
  ARCHIVE DESTINATION lib/kllvm
)
