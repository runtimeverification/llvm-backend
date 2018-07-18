cmake_minimum_required (VERSION 3.4)

project (KLLVM CXX C)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
add_definitions(-Wall)

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
if (${LLVM_PACKAGE_VERSION} VERSION_LESS 5.0)
  message(FATAL_ERROR "LLVM 5.0 or newer is required")
endif()

message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(AddLLVM)
include(HandleLLVMOptions)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(LLVM_BUILD_TOOLS ON)

find_package(ZLIB REQUIRED)
find_package(BISON)
find_package(FLEX)

option(USE_CLANG_WERROR_FLAGS OFF)
if (USE_CLANG_WERROR_FLAGS)
  set(WARNING_FLAGS -Weverything
    -Wno-unknown-warning-option -Wno-c++98-compat -Wno-padded
    -Wno-global-constructors -Wno-exit-time-destructors -Wno-unused-macros)
  list(APPEND LLVM_DEFINITIONS ${WARNING_FLAGS} -Werror)
endif()

link_directories(${LLVM_LIBRARY_DIR})
include_directories(${LLVM_INCLUDE_DIR})
include_directories (${PROJECT_SOURCE_DIR}/include)

set(CMAKE_INSTALL_PREFIX "install")

macro(add_kllvm_library name)
  llvm_add_library(${name} ${ARGN})
  install(
    TARGETS ${name}
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    COMPONENT ${name}
  )
endmacro(add_kllvm_library name)

# Auto-generated header path
set(KLLVM_AUTOGEN_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/include)
include_directories(${KLLVM_AUTOGEN_INCLUDE_DIR})

install(DIRECTORY include/kllvm ${KLLVM_AUTOGEN_INCLUDE_DIR}/kllvm
  DESTINATION include
  COMPONENT kllvm-headers
  FILES_MATCHING PATTERN "*.h" PATTERN "*.hh"
)

add_subdirectory(lib)
add_subdirectory(tools)
#add_subdirectory(test)
