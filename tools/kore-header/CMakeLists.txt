kllvm_add_tool(kore-header
  main.cpp
)

target_link_libraries(kore-header
  PUBLIC BinaryKore
)

install(
  TARGETS kore-header
  RUNTIME DESTINATION bin
)
