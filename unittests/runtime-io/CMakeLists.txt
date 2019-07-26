add_kllvm_unittest(runtime-io-tests
  io.cpp
  main.cpp
)

target_link_libraries(runtime-io-tests
  PUBLIC
  io
  strings
  arithmetic
  alloc
  gmp
  mpfr
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARIES}
)

add_executable(IOTest iotest.cpp)
target_compile_options(IOTest PUBLIC -O0)
