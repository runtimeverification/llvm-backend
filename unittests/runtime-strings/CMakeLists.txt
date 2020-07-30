add_kllvm_unittest(runtime-strings-tests
  stringtest.cpp
  bytestest.cpp
  main.cpp
)

target_link_libraries(runtime-strings-tests
  PUBLIC
  strings
  arithmetic
  alloc
  gmp
  mpfr
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARIES}
)

if(APPLE)
target_link_libraries(runtime-strings-tests
  PUBLIC
  iconv
)
endif()
