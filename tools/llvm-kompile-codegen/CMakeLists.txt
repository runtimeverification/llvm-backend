set(LLVM_REQUIRES_RTTI ON)
set(LLVM_REQUIRES_EH ON)
kllvm_add_tool(llvm-kompile-codegen
  main.cpp
)

target_link_libraries(llvm-kompile-codegen PUBLIC Codegen Parser AST gmp mpfr yaml)

llvm_config(llvm-kompile-codegen
  ${LLVM_TARGETS_TO_BUILD}
)

install(
  TARGETS llvm-kompile-codegen
  RUNTIME DESTINATION bin
)

add_definitions(${LLVM_DEFINITIONS})
