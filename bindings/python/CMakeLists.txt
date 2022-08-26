pybind11_add_module(kllvm-static STATIC kllvm.cpp)
pybind11_add_module(kllvm        SHARED kllvm.cpp)

install(FILES $<TARGET_FILE:kllvm-static>
  DESTINATION lib/kllvm
  RENAME libkllvm.a)

install(TARGETS kllvm
  DESTINATION lib/kllvm
  LIBRARY)
