add_library(util STATIC
  ConfigurationParser.cpp
  ConfigurationPrinter.cpp
  ConfigurationSerializer.cpp
  ConfigurationToAST.cpp
  search.cpp
)

install(
  TARGETS util
  ARCHIVE DESTINATION lib/kllvm
)

target_link_libraries(util PUBLIC Parser AST)
