set(LLVM_LINK_COMPONENTS
  ${LLVM_TARGETS_TO_BUILD}
  Support
)

add_kllvm_library(Hello
  hello.cpp
)

add_definitions(${LLVM_DEFINITIONS})
