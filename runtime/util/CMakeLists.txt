set(LLVM_REQUIRES_RTTI ON)
set(LLVM_REQUIRES_EH ON)

add_library(util STATIC
  ConfigurationParser.cpp
  ConfigurationPrinter.cpp
)

install(
  TARGETS util
  ARCHIVE DESTINATION lib/kllvm
)

target_link_libraries(util PUBLIC Parser AST)

add_definitions(${LLVM_DEFINITIONS})
