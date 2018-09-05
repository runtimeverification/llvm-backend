cmake_minimum_required (VERSION 3.4)

project (KLLVM CXX C)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
add_definitions(-Wall -Werror -flto)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -flto")
set(CMAKE_AR "llvm-ar")
set(CMAKE_RANLIB "llvm-ranlib")

find_package(LLVM 5 REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
if (${LLVM_PACKAGE_VERSION} VERSION_LESS 5.0)
  message(FATAL_ERROR "LLVM 5.0 or newer is required")
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(LLVM_BUILD_TOOLS ON)

find_package(ZLIB REQUIRED)
find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)
find_package(GMP REQUIRED)
find_package(Boost REQUIRED COMPONENTS unit_test_framework)

link_directories(${LLVM_LIBRARY_DIR})
include_directories(${LLVM_INCLUDE_DIR})
include_directories (${PROJECT_SOURCE_DIR}/include)

set(CMAKE_INSTALL_PREFIX "install")

macro(add_kllvm_library name)
  add_library(${name} ${ARGN})
  install(
    TARGETS ${name}
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    COMPONENT ${name}
  )
endmacro(add_kllvm_library name)

macro(add_kllvm_runtime_library name)
  add_library(${name} ${ARGN})
  install(
    TARGETS ${name}
    LIBRARY DESTINATION runtime
    ARCHIVE DESTINATION runtime
    COMPONENT ${name}
  )
endmacro(add_kllvm_runtime_library name)

# Find the libraries that correspond to the LLVM components
# that we wish to use
llvm_map_components_to_libnames(llvm_libs support core irreader)

macro(kllvm_add_tool name)
  add_executable(${name} ${ARGN})

  # Link against LLVM libraries
  target_link_libraries(${name} ${llvm_libs})
endmacro(kllvm_add_tool)

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
add_subdirectory(runtime)
add_subdirectory(unittests)
#add_subdirectory(test)
