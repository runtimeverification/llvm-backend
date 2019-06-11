add_kllvm_unittest(compiler-tests
  asttest.cpp
  main.cpp
)

target_link_libraries(compiler-tests
  PUBLIC
  AST
  Codegen
  gmp
  yaml
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARIES}
  ${llvm_libs}
)
