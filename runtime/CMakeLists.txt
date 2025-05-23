set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/main)
configure_file(main/main.ll         ${CMAKE_CURRENT_BINARY_DIR}/main   @ONLY)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/opaque)
configure_file(opaque/opaque.ll     ${CMAKE_CURRENT_BINARY_DIR}/opaque @ONLY)

install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/main/main.ll main/search.cpp
  DESTINATION lib/kllvm/llvm/main
)

add_subdirectory(alloc)
add_subdirectory(arithmetic)
add_subdirectory(collect)
add_subdirectory(collections)
add_subdirectory(io)
add_subdirectory(json)
add_subdirectory(lto)
add_subdirectory(meta)
add_subdirectory(opaque)
add_subdirectory(strings)
add_subdirectory(util)
add_subdirectory(timer)
