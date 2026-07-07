add_kllvm_unittest(runtime-gc-tests
  transience.cpp
  main.cpp
)

target_link_libraries(runtime-gc-tests
  PUBLIC
  collections
  collect
  alloc
  lto-static
  gmp
  mpfr
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARIES}
)
