set(LLVM_REQUIRES_RTTI ON)
set(LLVM_REQUIRES_EH ON)

add_library(Codegen
  CreateTerm.cpp
  Decision.cpp
  DecisionParser.cpp
  EmitConfigParser.cpp
)

target_link_libraries(Codegen PUBLIC Alloc AST)

add_definitions(${LLVM_DEFINITIONS})
