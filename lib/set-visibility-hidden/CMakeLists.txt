add_library(SetVisibilityHiddenInternal
  SetVisibilityHidden.cpp
)

add_library(SetVisibilityHidden MODULE
  SetVisibilityHidden.cpp
)

install(
  TARGETS SetVisibilityHiddenInternal SetVisibilityHidden
  LIBRARY DESTINATION lib/kllvm
)

separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})
