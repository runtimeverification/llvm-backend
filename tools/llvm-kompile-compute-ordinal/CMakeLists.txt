kllvm_add_tool(llvm-kompile-compute-ordinal
  main.cpp
)

target_link_libraries(llvm-kompile-compute-ordinal
  PUBLIC Parser AST Codegen
)

install(
  TARGETS llvm-kompile-compute-ordinal
  RUNTIME DESTINATION bin
)
