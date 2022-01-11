configure_file(CreateTerm.cpp CreateTerm.cpp @ONLY)

add_library(Codegen
  ${CMAKE_CURRENT_BINARY_DIR}/CreateTerm.cpp
  CreateStaticTerm.cpp
  Debug.cpp
  Decision.cpp
  DecisionParser.cpp
  EmitConfigParser.cpp
  Util.cpp
)

target_link_libraries(Codegen
  PUBLIC  AST
  PRIVATE base64)
