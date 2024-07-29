kllvm_add_tool(kore-proof-trace-shm-writer
  main.cpp
)

target_link_libraries(kore-proof-trace-shm-writer
  PUBLIC BinaryKore
)

install(
  TARGETS kore-proof-trace-shm-writer
  RUNTIME DESTINATION bin
)
