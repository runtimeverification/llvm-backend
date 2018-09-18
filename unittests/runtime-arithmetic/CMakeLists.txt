add_kllvm_unittest(runtime-arithmetic-tests
  inttest.cpp
  main.cpp
)

target_link_libraries(runtime-arithmetic-tests
  PUBLIC
  arithmetic
  gmp
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARIES}
)
