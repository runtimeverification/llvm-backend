configure_file(CreateTerm.cpp CreateTerm.cpp @ONLY)
configure_file(ApplyPasses.cpp ApplyPasses.cpp @ONLY)

add_library(Codegen
  ${CMAKE_CURRENT_BINARY_DIR}/CreateTerm.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/ApplyPasses.cpp
  CreateStaticTerm.cpp
  Debug.cpp
  Decision.cpp
  DecisionParser.cpp
  EmitConfigParser.cpp
  Util.cpp
)

target_link_libraries(Codegen
  PUBLIC  AST fmt::fmt
  PRIVATE base64)
