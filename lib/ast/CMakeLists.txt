set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

add_library(AST
  AST.cpp
)

target_link_libraries(AST
  PUBLIC Parser BinaryKore)

install(
  TARGETS AST
  ARCHIVE DESTINATION lib/kllvm
)
