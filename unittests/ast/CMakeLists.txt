add_kllvm_unittest(ast-tests
  asttest.cpp
  main.cpp
)

target_link_libraries(ast-tests
  AST
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARIES}
)
