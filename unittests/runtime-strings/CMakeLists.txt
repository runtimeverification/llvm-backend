add_kllvm_unittest(runtime-strings-tests
  stringtest.cpp
  main.cpp
)

target_link_libraries(runtime-strings-tests
  strings
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARIES}
)
