kllvm_add_tool(llvm-kompile-compute-loc
  main.cpp
)

target_link_libraries(llvm-kompile-compute-loc
  PUBLIC Parser AST Codegen
)

install(
  TARGETS llvm-kompile-compute-loc
  RUNTIME DESTINATION bin
)
