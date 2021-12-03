macro(add_kllvm_unittest test_name)
  # add target for building the test
  kllvm_add_tool(${test_name} ${ARGN})

  add_test(
    NAME ${test_name}
    COMMAND ${test_name}
  )
endmacro(add_kllvm_unittest test_name)

add_subdirectory(runtime-arithmetic)
add_subdirectory(runtime-ffi)
add_subdirectory(runtime-io)
add_subdirectory(runtime-strings)
add_subdirectory(runtime-collections)
add_subdirectory(compiler)
