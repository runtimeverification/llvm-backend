set(LLVM_REQUIRES_RTTI ON)
set(LLVM_REQUIRES_EH ON)

add_kllvm_runtime_library(configurationparser STATIC
  ConfigurationParser.cpp
  ConfigurationPrinter.cpp
)

target_link_libraries(configurationparser Parser AST)

add_definitions(${LLVM_DEFINITIONS})
