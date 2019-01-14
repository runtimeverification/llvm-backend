cmake_minimum_required (VERSION 3.4)

project (KLLVM CXX C)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
add_definitions(-Wall -Werror)
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -flto")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -flto")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -flto")

set(CMAKE_AR "llvm-ar")
set(CMAKE_RANLIB "llvm-ranlib")

if(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
set(RUST_TARGET_TRIPLE "x86_64-apple-darwin")
else()
set(RUST_TARGET_TRIPLE "x86_64-unknown-linux-gnu")
endif()

find_package(LLVM 6 REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
if (${LLVM_PACKAGE_VERSION} VERSION_LESS 6.0)
  message(FATAL_ERROR "LLVM 6.0 or newer is required")
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

# Find the libraries that correspond to the LLVM components
# that we wish to use
llvm_map_components_to_libnames(llvm_libs support core irreader)

macro(kllvm_add_tool name)
  add_executable(${name} ${ARGN})

  # Link against LLVM libraries
  llvm_config(${name})
if(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
  target_link_libraries(${name} PUBLIC "-ljemalloc" "-Wl,-rpath /usr/local/lib")
else()
  target_link_libraries(${name} PUBLIC "-ljemalloc" "-Wl,-rpath=/usr/local/lib")
endif()
endmacro(kllvm_add_tool)

FILE(GLOB LLVMFiles runtime/*.ll)

install(
  FILES ${LLVMFiles}
  DESTINATION lib/kllvm/llvm
)

install(
  FILES runtime/main/main.ll
  DESTINATION lib/kllvm/llvm/main
)


install(
  PROGRAMS bin/llvm-kompile bin/llvm-kompile-clang bin/llvm-kompile-matching bin/llvm-kompile-rust-lto
  DESTINATION bin
)

install(
  DIRECTORY include/runtime
  DESTINATION include
)

FILE(GLOB_RECURSE HaskellSources matching/src/*.hs matching/app/Main.hs)

add_custom_command(
  OUTPUT bin/llvm-kompile-matching
  DEPENDS ${HaskellSources}
  COMMAND stack build
  COMMAND stack install --local-bin-path ${PROJECT_SOURCE_DIR}/bin
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/matching
)

add_custom_target(matching
  ALL DEPENDS bin/llvm-kompile-matching
)

# Auto-generated header path
set(KLLVM_AUTOGEN_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/include)
include_directories(${KLLVM_AUTOGEN_INCLUDE_DIR})

add_subdirectory(lib)
add_subdirectory(tools)
add_subdirectory(runtime)
add_subdirectory(unittests)
#add_subdirectory(test)
