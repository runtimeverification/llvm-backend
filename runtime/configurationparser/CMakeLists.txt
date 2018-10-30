set(LLVM_REQUIRES_RTTI ON)
set(LLVM_REQUIRES_EH ON)

add_library(configurationparser STATIC
  ConfigurationParser.cpp
  ConfigurationPrinter.cpp
)

install(
  TARGETS configurationparser
  ARCHIVE DESTINATION lib/kllvm
)

target_link_libraries(configurationparser PUBLIC Parser AST)

add_definitions(${LLVM_DEFINITIONS})
