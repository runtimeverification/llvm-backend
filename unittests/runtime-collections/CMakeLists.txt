add_kllvm_unittest(runtime-collections-tests
  lists.cpp
  maps.cpp
  treemaps.cpp
  rangemaps.cpp
  rangemap-hooks.cpp
  sets.cpp
  main.cpp
)

target_link_libraries(runtime-collections-tests
  PUBLIC
  collections
  gmp
  mpfr
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARIES}
)
