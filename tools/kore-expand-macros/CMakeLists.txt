set(LLVM_REQUIRES_RTTI ON)
set(LLVM_REQUIRES_EH ON)
kllvm_add_tool(kore-expand-macros
  main.cpp
)

target_link_libraries(kore-expand-macros PUBLIC Parser AST)
target_compile_options(kore-expand-macros PUBLIC -O3)

install(
  TARGETS kore-expand-macros
  RUNTIME DESTINATION bin
)
