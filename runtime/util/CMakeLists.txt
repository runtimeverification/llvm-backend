add_library(util STATIC
  ConfigurationParser.cpp
  ConfigurationPrinter.cpp
  ConfigurationSerializer.cpp
  search.cpp
  util.cpp
)

install(
  TARGETS util
  ARCHIVE DESTINATION lib/kllvm
)

target_link_libraries(util PUBLIC numeric_strings Parser AST)
