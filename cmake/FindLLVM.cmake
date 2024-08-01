find_package(LLVM 18.1 QUIET CONFIG)

if (NOT LLVM_FOUND)
  find_package(LLVM 17 QUIET CONFIG)
endif()

if (NOT LLVM_FOUND)
  find_package(LLVM 16 QUIET CONFIG)
endif()

if (NOT LLVM_FOUND)
  find_package(LLVM 15 QUIET CONFIG)
endif()

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
if (${LLVM_PACKAGE_VERSION} VERSION_LESS 15)
  message(FATAL_ERROR "LLVM 15 or newer is required")
endif()

find_program(OPT opt
  PATHS ${LLVM_TOOLS_BINARY_DIR}
  NO_DEFAULT_PATH)

find_program(LLC llc
  PATHS ${LLVM_TOOLS_BINARY_DIR}
  NO_DEFAULT_PATH)

find_program(LLDB lldb
  PATHS ${LLVM_TOOLS_BINARY_DIR}
  NO_DEFAULT_PATH)

find_program(LLVM_DIS llvm-dis
  PATHS ${LLVM_TOOLS_BINARY_DIR}
  NO_DEFAULT_PATH)

execute_process(
  COMMAND "${LLVM_TOOLS_BINARY_DIR}/llvm-config" "--libdir"
  OUTPUT_VARIABLE LLVM_LIBRARY_DIR
  OUTPUT_STRIP_TRAILING_WHITESPACE)

if(NOT OPT)
  message(FATAL_ERROR "Could not find an opt binary. Is llvm installed on your PATH?")
endif()

if(NOT LLC)
  message(FATAL_ERROR "Could not find an llc binary. Is llvm installed on your PATH?")
endif()

if(NOT LLVM_DIS)
  message(FATAL_ERROR "Could not find an llvm-dis binary. Is llvm installed on your PATH?")
endif()
