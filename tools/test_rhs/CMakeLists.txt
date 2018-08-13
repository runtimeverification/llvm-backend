set(LLVM_REQUIRES_RTTI ON)
set(LLVM_REQUIRES_EH ON)
add_llvm_tool(test_rhs
  main.cpp
)

target_link_libraries(test_rhs Codegen Parser AST)

llvm_config(test_rhs
  ${LLVM_TARGETS_TO_BUILD}
)

add_definitions(${LLVM_DEFINITIONS})
