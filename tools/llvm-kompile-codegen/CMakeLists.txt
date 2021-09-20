kllvm_add_tool(llvm-kompile-codegen
  main.cpp
)

target_link_libraries(llvm-kompile-codegen PUBLIC Codegen Parser AST gmp mpfr yaml ${llvm_libs})

install(
  TARGETS llvm-kompile-codegen
  RUNTIME DESTINATION bin
)

separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})
