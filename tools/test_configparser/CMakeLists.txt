set(LLVM_REQUIRES_RTTI ON)
set(LLVM_REQUIRES_EH ON)
kllvm_add_tool(test_configparser
  main.cpp
)

target_link_libraries(test_configparser PUBLIC Codegen AST gmp Parser)

llvm_config(test_configparser
  ${LLVM_TARGETS_TO_BUILD}
)

add_definitions(${LLVM_DEFINITIONS})
