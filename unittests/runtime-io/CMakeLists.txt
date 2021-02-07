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

if(APPLE)
target_link_libraries(runtime-io-tests
  PUBLIC
  iconv
)
endif()

add_executable(IOTest iotest.cpp)
set_target_properties(IOTest PROPERTIES EXCLUDE_FROM_ALL TRUE)
add_dependencies(runtime-io-tests IOTest)
