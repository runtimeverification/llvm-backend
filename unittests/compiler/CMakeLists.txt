add_kllvm_unittest(compiler-tests
  asttest.cpp
  decisiontest.cpp
  main.cpp
)

target_link_libraries(compiler-tests
  AST
  Codegen
  gmp
  yaml-cpp
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARIES}
)
