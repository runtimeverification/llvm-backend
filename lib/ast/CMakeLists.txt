set(LLVM_LINK_COMPONENTS
  ${LLVM_TARGETS_TO_BUILD}
  Support
)

set(LLVM_REQUIRES_RTTI ON)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

add_library(AST
  AST.cpp
)

install(
  TARGETS AST
  ARCHIVE DESTINATION lib/kllvm
)

add_definitions(${LLVM_DEFINITIONS})
