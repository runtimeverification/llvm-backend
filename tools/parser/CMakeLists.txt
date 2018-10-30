set(LLVM_REQUIRES_RTTI ON)
set(LLVM_REQUIRES_EH ON)


kllvm_add_tool(parser
  main.cpp
)

llvm_config(parser
  ${LLVM_TARGETS_TO_BUILD}
)
target_link_libraries(parser PUBLIC Parser AST)

add_definitions(${LLVM_DEFINITIONS})
