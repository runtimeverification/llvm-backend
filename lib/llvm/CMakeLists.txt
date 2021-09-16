set(LLVM_REQUIRES_RTTI ON)
set(LLVM_REQUIRES_EH ON)

add_library(LLVMPass SHARED
  EmitGCLayoutInfo.cpp
)

target_link_libraries(LLVMPass PUBLIC AST)

install(
  TARGETS LLVMPass
  LIBRARY DESTINATION lib/kllvm
)

add_definitions(${LLVM_DEFINITIONS})
