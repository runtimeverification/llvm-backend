kllvm_add_tool(kore-split
  main.cpp
)

target_link_libraries(kore-split PUBLIC Parser AST)
target_compile_options(kore-split PUBLIC -O3)

install(
  TARGETS kore-split
  RUNTIME DESTINATION bin
)
