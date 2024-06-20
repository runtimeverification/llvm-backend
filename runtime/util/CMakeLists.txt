add_library(util STATIC
  ConfigurationParser.cpp
  ConfigurationPrinter.cpp
  ConfigurationSerializer.cpp
  finish_rewriting.cpp
  match_log.cpp
  search.cpp
  util.cpp
  clock.cpp
)

install(
  TARGETS util
  ARCHIVE DESTINATION lib/kllvm
)

target_link_libraries(util PUBLIC numeric_strings Parser AST)
