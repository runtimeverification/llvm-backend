add_kllvm_unittest(runtime-ffi-tests
  ffi.cpp
  main.cpp
)

target_link_libraries(runtime-ffi-tests
  PUBLIC
  ffi
  io
  meta
  strings
  arithmetic
  alloc
  gmp
  mpfr
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARIES}
)
