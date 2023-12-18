kllvm_add_tool(kore-proof-trace-test
  main.cpp
)

target_link_libraries(kore-proof-trace-test
  PUBLIC BinaryKore
)

install(
  TARGETS kore-proof-trace-test
  RUNTIME DESTINATION bin
)
