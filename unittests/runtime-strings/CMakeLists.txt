add_kllvm_unittest(runtime-strings-tests
  stringtest.cpp
  main.cpp
)

target_link_libraries(runtime-strings-tests
  strings
  gmp
  mpfr
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARIES}
)
