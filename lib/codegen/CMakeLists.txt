add_library(Codegen
  CreateTerm.cpp
  ApplyPasses.cpp
  CreateStaticTerm.cpp
  Debug.cpp
  Decision.cpp
  DecisionParser.cpp
  EmitConfigParser.cpp
  Metadata.cpp
  Options.cpp
  ProofEvent.cpp
  Util.cpp
)

target_link_libraries(Codegen
  PUBLIC  AST fmt::fmt-header-only alloc-cpp opaque-cpp
  PRIVATE base64)
