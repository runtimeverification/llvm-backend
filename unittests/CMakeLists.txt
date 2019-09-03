add_custom_target(unittests)
add_custom_target(run-unittests)

macro(add_kllvm_unittest test_name)
  # add target for building the test
  kllvm_add_tool(${test_name} ${ARGN})

  # add a dependency to the global unit test build target
  add_dependencies(unittests ${test_name})

  # add target for running the test
  add_custom_target(run-${test_name}
    COMMAND ./${test_name}
    COMMENT "Running unit tests: ${test_name}"
  )
  add_dependencies(run-${test_name} ${test_name})

  # add a dependency to the global unit test run target
  add_dependencies(run-unittests run-${test_name})
endmacro(add_kllvm_unittest test_name)

add_subdirectory(runtime-arithmetic)
add_subdirectory(runtime-ffi)
add_subdirectory(runtime-io)
add_subdirectory(runtime-strings)
add_subdirectory(runtime-collections)
add_subdirectory(compiler)
