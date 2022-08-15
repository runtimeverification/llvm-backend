pybind11_add_module(kore STATIC kore.cpp)

target_link_libraries(kore
  PUBLIC AST Parser util)

install(FILES $<TARGET_FILE:kore>
  DESTINATION lib/kllvm
  RENAME libpykore.a)
