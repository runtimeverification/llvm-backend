set(BISON_AUTOGEN_INCLUDE_DIR ${KLLVM_AUTOGEN_INCLUDE_DIR}/kllvm/parser)
file(MAKE_DIRECTORY ${BISON_AUTOGEN_INCLUDE_DIR})

BISON_TARGET(KOREParser KOREParser.y ${CMAKE_CURRENT_BINARY_DIR}/KOREParser.cpp
             DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/KOREParser.hh)

FLEX_TARGET(KOREScanner KOREScanner.l ${CMAKE_CURRENT_BINARY_DIR}/KOREScanner.cpp)
ADD_FLEX_BISON_DEPENDENCY(KOREScanner KOREParser)

add_custom_command(
  OUTPUT ${BISON_AUTOGEN_INCLUDE_DIR}/KOREParser.hh
  DEPENDS ${BISON_KOREParser_OUTPUTS}
  COMMAND ${CMAKE_COMMAND} -E copy
          ${CMAKE_CURRENT_BINARY_DIR}/KOREParser.hh
          ${BISON_AUTOGEN_INCLUDE_DIR}/KOREParser.hh
  COMMENT "[BISON] Copying auto-generated file KOREParser.hh"
)

add_custom_command(
  OUTPUT ${BISON_AUTOGEN_INCLUDE_DIR}/location.hh
  DEPENDS ${BISON_KOREParser_OUTPUTS}
  COMMAND ${CMAKE_COMMAND} -E copy
          ${CMAKE_CURRENT_BINARY_DIR}/location.hh
          ${BISON_AUTOGEN_INCLUDE_DIR}/location.hh
  COMMENT "[BISON] Copying auto-generated file location.hh"
)

add_custom_command(
  OUTPUT ${BISON_AUTOGEN_INCLUDE_DIR}/position.hh
  DEPENDS ${BISON_KOREParser_OUTPUTS}
  COMMAND ${CMAKE_COMMAND} -E copy
          ${CMAKE_CURRENT_BINARY_DIR}/position.hh
          ${BISON_AUTOGEN_INCLUDE_DIR}/position.hh
  COMMENT "[BISON] Copying auto-generated file position.hh"
)

add_custom_command(
  OUTPUT ${BISON_AUTOGEN_INCLUDE_DIR}/stack.hh
  DEPENDS ${BISON_KOREParser_OUTPUTS}
  COMMAND ${CMAKE_COMMAND} -E copy
          ${CMAKE_CURRENT_BINARY_DIR}/stack.hh
          ${BISON_AUTOGEN_INCLUDE_DIR}/stack.hh
  COMMENT "[BISON] Copying auto-generated file stack.hh"
)

set(LLVM_REQUIRES_RTTI ON)
set(LLVM_REQUIRES_EH ON)

add_kllvm_library(Parser
  KOREScanner.cpp
  KOREParser.cpp

  ${BISON_AUTOGEN_INCLUDE_DIR}/KOREParser.hh
  ${BISON_AUTOGEN_INCLUDE_DIR}/location.hh
  ${BISON_AUTOGEN_INCLUDE_DIR}/position.hh
  ${BISON_AUTOGEN_INCLUDE_DIR}/stack.hh
)

add_definitions(${LLVM_DEFINITIONS})
