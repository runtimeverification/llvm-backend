set(LLVM_REQUIRES_RTTI ON)
set(LLVM_REQUIRES_EH ON)

add_library(Codegen
  CreateTerm.cpp
  Debug.cpp
  Decision.cpp
  DecisionParser.cpp
  EmitConfigParser.cpp
  GenAlloc.cpp
)

target_link_libraries(Codegen PUBLIC AST)

add_definitions(${LLVM_DEFINITIONS})
