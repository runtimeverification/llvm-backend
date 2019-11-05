FLEX_TARGET(KOREScanner KOREScanner.l ${CMAKE_CURRENT_BINARY_DIR}/KOREScanner.cpp)

set(LLVM_REQUIRES_RTTI ON)
set(LLVM_REQUIRES_EH ON)

add_library(Parser
  KOREScanner.cpp
  KOREParser.cpp
)

install(
  TARGETS Parser
  ARCHIVE DESTINATION lib/kllvm
)

add_definitions(${LLVM_DEFINITIONS})
