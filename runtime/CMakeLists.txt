set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/main)
configure_file(main/main.ll ${CMAKE_CURRENT_BINARY_DIR}/main @ONLY)
install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/main/main.ll main/search.cpp
  DESTINATION lib/kllvm/llvm/main
)

configure_file(equality.ll ${CMAKE_CURRENT_BINARY_DIR} @ONLY)
configure_file(finish_rewriting.ll ${CMAKE_CURRENT_BINARY_DIR} @ONLY)
configure_file(fresh.ll ${CMAKE_CURRENT_BINARY_DIR} @ONLY)
configure_file(getTag.ll ${CMAKE_CURRENT_BINARY_DIR} @ONLY)
configure_file(move_float.ll ${CMAKE_CURRENT_BINARY_DIR} @ONLY)
configure_file(move_int.ll ${CMAKE_CURRENT_BINARY_DIR} @ONLY)
configure_file(string_equal.ll ${CMAKE_CURRENT_BINARY_DIR} @ONLY)
configure_file(take_steps.ll ${CMAKE_CURRENT_BINARY_DIR} @ONLY)

FILE(GLOB LLVMFiles ${CMAKE_CURRENT_BINARY_DIR}/*.ll)

install(
  FILES ${LLVMFiles}
  DESTINATION lib/kllvm/llvm
)

add_subdirectory(arithmetic)
add_subdirectory(util)
add_subdirectory(strings)
add_subdirectory(meta)
add_subdirectory(alloc)
add_subdirectory(collect)
add_subdirectory(io)
add_subdirectory(collections)
add_subdirectory(json)
