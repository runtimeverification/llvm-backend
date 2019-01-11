set(LLVM_REQUIRES_RTTI ON)
set(LLVM_REQUIRES_EH ON)
kllvm_add_tool(test_rhs
  main.cpp
)

target_link_libraries(test_rhs PUBLIC Codegen Parser AST gmp mpfr)

llvm_config(test_rhs
  ${LLVM_TARGETS_TO_BUILD}
)

add_definitions(${LLVM_DEFINITIONS})
