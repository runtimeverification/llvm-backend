kllvm_add_tool(kore-proof-trace
  main.cpp
)

target_link_libraries(kore-proof-trace
  PUBLIC BinaryKore AST
)

install(
  TARGETS kore-proof-trace
  RUNTIME DESTINATION bin
)
