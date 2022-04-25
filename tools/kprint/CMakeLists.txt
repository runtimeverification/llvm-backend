kllvm_add_tool(kprint
  main.cpp
)

target_link_libraries(kprint PUBLIC KOREPrinter)
target_compile_options(kprint PUBLIC -O3)

install(
  TARGETS kprint
  RUNTIME DESTINATION bin
)
