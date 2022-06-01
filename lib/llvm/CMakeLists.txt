add_library(LLVMPass MODULE
  EmitGCLayoutInfo.cpp
  KoreGC.cpp
  MarkTailCallsAsGCLeaf.cpp
  NoOmitFramePointerForTailcc.cpp
)

target_link_libraries(LLVMPass PUBLIC AST)
if (APPLE)
  target_link_libraries(LLVMPass PUBLIC "-undefined dynamic_lookup")
endif()

install(
  TARGETS LLVMPass
  LIBRARY DESTINATION lib/kllvm
)
