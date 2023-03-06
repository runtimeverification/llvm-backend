find_package(LLVM 15 QUIET CONFIG)

if (NOT LLVM_FOUND)
  find_package(LLVM 14 QUIET CONFIG)
endif()

if (NOT LLVM_FOUND)
  find_package(LLVM 13 QUIET CONFIG)
endif()

if (NOT LLVM_FOUND)
  find_package(LLVM 12 QUIET CONFIG)
endif()

if (NOT LLVM_FOUND)
  find_package(LLVM 11.1 QUIET CONFIG)
endif()

if (NOT LLVM_FOUND)
  find_package(LLVM 11 REQUIRED CONFIG)
endif()

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
if (${LLVM_PACKAGE_VERSION} VERSION_LESS 11)
  message(FATAL_ERROR "LLVM 11 or newer is required")
endif()

find_program(OPT opt
  PATHS ${LLVM_TOOLS_BINARY_DIR}
  NO_DEFAULT_PATH)

find_program(LLC llc
  PATHS ${LLVM_TOOLS_BINARY_DIR}
  NO_DEFAULT_PATH)

if(NOT OPT)
  message(FATAL_ERROR "Could not find an opt binary. Is llvm installed on your PATH?")
endif()

if(NOT LLC)
  message(FATAL_ERROR "Could not find an llc binary. Is llvm installed on your PATH?")
endif()
