kllvm_add_tool(k-rule-apply
  main.cpp
)

target_link_libraries(k-rule-apply 
  PUBLIC Parser AST gmp gmpxx
)

install(
  TARGETS k-rule-apply
  RUNTIME DESTINATION bin
)
