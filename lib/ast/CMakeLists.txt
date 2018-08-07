set(LLVM_LINK_COMPONENTS
  ${LLVM_TARGETS_TO_BUILD}
  Support
)

set(LLVM_REQUIRES_RTTI ON)

add_kllvm_library(AST
  AST.cpp
)

add_definitions(${LLVM_DEFINITIONS})
