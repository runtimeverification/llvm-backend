pybind11_add_module(kore SHARED kore.cpp)

target_link_libraries(kore
  PUBLIC AST Parser)
