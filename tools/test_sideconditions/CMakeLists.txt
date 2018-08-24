set(LLVM_REQUIRES_RTTI ON)
set(LLVM_REQUIRES_EH ON)
add_llvm_tool(test_sideconditions
  main.cpp
)

target_link_libraries(test_sideconditions Codegen Parser AST gmp)

llvm_config(test_sideconditions
  ${LLVM_TARGETS_TO_BUILD}
)

add_definitions(${LLVM_DEFINITIONS})
