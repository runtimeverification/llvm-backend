add_kllvm_unittest(runtime-collections-tests
  collections.cpp
  main.cpp
)

target_link_libraries(runtime-collections-tests
  PUBLIC
  collections
  alloc
  gmp
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARIES}
)
