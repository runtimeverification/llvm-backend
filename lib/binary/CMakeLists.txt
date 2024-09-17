add_library(BinaryKore
  serializer.cpp
  deserializer.cpp
  ProofTraceParser.cpp
  ProofTraceUtils.cpp
)

target_link_libraries(BinaryKore
  PUBLIC AST fmt::fmt-header-only
)

install(
  TARGETS BinaryKore
  ARCHIVE DESTINATION lib/kllvm
)
