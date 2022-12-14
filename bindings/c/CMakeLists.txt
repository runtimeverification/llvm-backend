add_library(kllvm-c-static STATIC
  lib.cpp
)

target_include_directories(kllvm-c-static PUBLIC
  include)

target_link_libraries(kllvm-c-static PUBLIC
  AST
  util
)

install(FILES $<TARGET_FILE:kllvm-c-static>
  DESTINATION lib/kllvm
  RENAME libkllvmcruntime.a)

install(DIRECTORY include/
  DESTINATION include)
