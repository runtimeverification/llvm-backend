set(LLVM_REQUIRES_EH ON)

add_library(io STATIC
  io.cpp
  logTerm.cpp
  parseKORE.cpp
)

install(
  TARGETS io
  ARCHIVE DESTINATION lib/kllvm
)
