set(LLVM_LINK_COMPONENTS
  ${LLVM_TARGETS_TO_BUILD}
  Support
)

set(LLVM_REQUIRES_RTTI ON)

add_library(Alloc
  Alloc.cpp
)

install(
  TARGETS Alloc
  ARCHIVE DESTINATION lib/kllvm
)

add_definitions(${LLVM_DEFINITIONS})
