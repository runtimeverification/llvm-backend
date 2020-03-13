set(LLVM_REQUIRES_EH ON)

add_library(json STATIC
  json.cpp
)

install(
  TARGETS json
  ARCHIVE DESTINATION lib/kllvm
)
