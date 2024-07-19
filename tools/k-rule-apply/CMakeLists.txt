kllvm_add_tool(k-rule-apply
  main.cpp
  shims.cpp
)

install(
  TARGETS k-rule-apply
  RUNTIME DESTINATION bin
)
