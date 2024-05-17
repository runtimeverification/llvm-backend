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

llvm_config(SetVisibilityHidden
  USE_SHARED true
  core irreader passes
)
