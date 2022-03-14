kllvm_add_tool(kore-strip
  main.cpp
)

target_link_libraries(kore-strip
  PUBLIC BinaryKore ${llvm_libs}
)

install(
  TARGETS kore-strip
  RUNTIME DESTINATION bin
)
