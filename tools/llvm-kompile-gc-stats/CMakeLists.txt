set(LLVM_REQUIRES_RTTI ON)
set(LLVM_REQUIRES_EH ON)
kllvm_add_tool(llvm-kompile-gc-stats
  main.cpp
)

target_link_libraries(llvm-kompile-gc-stats PUBLIC gmp)

install(
  TARGETS llvm-kompile-gc-stats
  RUNTIME DESTINATION bin
)
