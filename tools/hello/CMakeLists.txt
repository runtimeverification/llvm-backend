kllvm_add_tool(hello
  main.cpp
)

target_link_libraries(hello Hello)

llvm_config(hello
  ${LLVM_TARGETS_TO_BUILD}
  Support
)

add_definitions(${LLVM_DEFINITIONS})
