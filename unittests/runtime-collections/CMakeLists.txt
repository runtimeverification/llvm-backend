add_kllvm_unittest(runtime-collections-tests
  lists.cpp
  maps.cpp
  sets.cpp
  main.cpp
)

target_link_libraries(runtime-collections-tests
  PUBLIC
  collections
  gmp
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARIES}
)
