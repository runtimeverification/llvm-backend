kllvm_add_tool(treemap-test
  TreeTest.cpp
)

install(
  TARGETS treemap-test
  RUNTIME DESTINATION bin
)
