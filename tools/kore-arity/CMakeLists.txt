kllvm_add_tool(kore-arity
  main.cpp
)

target_link_libraries(kore-arity
  PUBLIC BinaryKore
)

install(
  TARGETS kore-arity
  RUNTIME DESTINATION bin
)
