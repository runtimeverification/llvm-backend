kllvm_add_tool(kore-rich-header
  main.cpp
)

target_link_libraries(kore-rich-header
  PUBLIC Parser AST
)

install(
  TARGETS kore-rich-header
  RUNTIME DESTINATION bin
)
