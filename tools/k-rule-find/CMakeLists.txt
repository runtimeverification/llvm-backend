kllvm_add_tool(k-rule-find
  main.cpp
)

target_link_libraries(k-rule-find 
  PUBLIC util BinaryKore Codegen Parser AST gmp mpfr numeric_strings
)

install(
  TARGETS k-rule-find
  RUNTIME DESTINATION bin
)

separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})