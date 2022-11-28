add_library(kllvm-c-static STATIC
  lib.cpp
)

target_link_libraries(kllvm-c-static PUBLIC
  AST
  util
)

install(FILES $<TARGET_FILE:kllvm-c-static>
  DESTINATION lib/kllvm
  RENAME libkllvmcruntime.a)
