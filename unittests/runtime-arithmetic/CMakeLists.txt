add_kllvm_unittest(runtime-arithmetic-tests
  inttest.cpp
  floattest.cpp
  main.cpp
)

target_link_libraries(runtime-arithmetic-tests
  PUBLIC
  arithmetic
  gmp
  mpfr
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARIES}
)
