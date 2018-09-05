set(LLVM_REQUIRES_RTTI ON)
set(LLVM_REQUIRES_EH ON)

add_kllvm_library(Codegen
  CreateTerm.cpp
  Decision.cpp
  DecisionParser.cpp
  EmitConfigParser.cpp
)

add_definitions(${LLVM_DEFINITIONS})
