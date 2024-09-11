add_library(KLLVMPassInternal
  SetVisibilityHidden.cpp
  RemoveDeadKFunctions.cpp
  PluginInfo.cpp
)

add_library(KLLVMPass MODULE
  SetVisibilityHidden.cpp
  RemoveDeadKFunctions.cpp
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
