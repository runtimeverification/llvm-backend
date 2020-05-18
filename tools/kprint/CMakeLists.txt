set(LLVM_REQUIRES_RTTI ON)
set(LLVM_REQUIRES_EH ON)
kllvm_add_tool(kprint
  addBrackets.cpp
  main.cpp
)

target_link_libraries(kprint PUBLIC Parser AST)
target_compile_options(kprint PUBLIC -O3)

install(
  TARGETS kprint
  RUNTIME DESTINATION bin
)
