add_library(io STATIC
  io.cpp
  logTerm.cpp
  parseKORE.cpp
)

install(
  TARGETS io
  ARCHIVE DESTINATION lib/kllvm
)

target_link_libraries(io PUBLIC KOREPrinter)
