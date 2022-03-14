kllvm_add_tool(kore-convert
  main.cpp
)

target_link_libraries(kore-convert
  PUBLIC Parser AST ${llvm_libs}
)

install(
  TARGETS kore-convert
  RUNTIME DESTINATION bin
)
