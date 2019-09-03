add_subdirectory(lib)

add_kllvm_unittest(runtime-ffi-tests
  ffi.cpp
  main.cpp
)

link_directories(../unittests/runtime-ffi/lib)

target_link_libraries(runtime-ffi-tests
  PUBLIC
  ffitest
  collections
  meta
  ffi
  io
  strings
  arithmetic
  alloc
  gmp
  mpfr
  dl
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARIES}
)
