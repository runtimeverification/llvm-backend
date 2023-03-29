kllvm_add_tool(k-rule-find
  main.cpp
)

target_link_libraries(k-rule-find 
  PUBLIC Parser AST Codegen
)

install(
  TARGETS k-rule-find
  RUNTIME DESTINATION bin
)
