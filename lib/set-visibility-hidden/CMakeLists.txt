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
