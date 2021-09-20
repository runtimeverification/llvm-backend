add_library(LLVMPass MODULE
  EmitGCLayoutInfo.cpp
)

target_link_libraries(LLVMPass PUBLIC AST)

install(
  TARGETS LLVMPass
  LIBRARY DESTINATION lib/kllvm
)
