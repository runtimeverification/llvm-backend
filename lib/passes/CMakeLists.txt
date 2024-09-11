add_library(KLLVMPassInternal
  SetVisibilityHidden.cpp
  PluginInfo.cpp
)

add_library(KLLVMPass MODULE
  SetVisibilityHidden.cpp
  PluginInfo.cpp
)

install(
  TARGETS KLLVMPassInternal KLLVMPass
  LIBRARY DESTINATION lib/kllvm
)

llvm_config(KLLVMPass
  USE_SHARED true
  core irreader passes
)
