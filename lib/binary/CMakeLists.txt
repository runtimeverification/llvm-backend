add_library(BinaryKore
  serializer.cpp
  deserializer.cpp
  ProofTraceParser.cpp
)

target_link_libraries(BinaryKore
  PUBLIC AST fmt::fmt
)

target_link_libraries(BinaryKore
  PUBLIC AST
)

install(
  TARGETS BinaryKore
  ARCHIVE DESTINATION lib/kllvm
)
