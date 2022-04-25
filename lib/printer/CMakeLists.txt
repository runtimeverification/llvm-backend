add_library(KOREPrinter
  addBrackets.cpp
  printer.cpp
)

install(
  TARGETS KOREPrinter
  ARCHIVE DESTINATION lib/kllvm
)

target_link_libraries(KOREPrinter PUBLIC Parser AST)
