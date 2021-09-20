FLEX_TARGET(KOREScanner KOREScanner.l ${CMAKE_CURRENT_BINARY_DIR}/KOREScanner.cpp COMPILE_FLAGS -Cfa)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

add_library(Parser
  KOREScanner.cpp
  KOREParser.cpp
)

install(
  TARGETS Parser
  ARCHIVE DESTINATION lib/kllvm
)
