cmake_minimum_required (VERSION 3.4)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# We rely on Clang being the CMAKE_{C,CXX}_COMPILER universally, so we need to
# set the relevant variables explicitly in this prelude module before starting
# the project (i.e. we can't look them up based on the LLVM version we find
# later on).
include(LLVMKompilePrelude)

project(KLLVM CXX C)

set(CMAKE_DISABLE_SOURCE_CHANGES  ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

include(CTest)

# Needs to be included first so that pkg-config paths are set up when looking
# for dependencies.
include(FixHomebrew)

include(FindDependencies)
include(KLLVMCompilerFlags)

# Needs to be included last so that it picks up LLVM executables correctly.
include(SetTriple)

if ((CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
     AND K_LLVM_BACKEND_LTO)
set(LLVM_KOMPILE_LTO "lto")
else()
set(LLVM_KOMPILE_LTO "nolto")
endif()

include_directories(${LLVM_INCLUDE_DIRS})
include_directories(${PROJECT_SOURCE_DIR}/include)

# Find the libraries that correspond to the LLVM components
# that we wish to use
llvm_map_components_to_libnames(llvm_libs core irreader)

macro(kllvm_add_tool name)
  add_executable(${name} ${ARGN})
  if(APPLE)
    if(NOT USE_NIX)
      target_link_libraries(${name} PUBLIC "-ljemalloc" "-Wl,-rpath ${BREW_PREFIX}/lib" "-ldl")
    else()
      target_link_libraries(${name} PUBLIC "-ljemalloc" "-ldl")
    endif() # NOT USE_NIX
  else()
    if(NOT USE_NIX)
      target_link_libraries(${name} PUBLIC "-ljemalloc" "-Wl,-rpath /usr/local/lib" "-ldl")
    else()
      target_link_libraries(${name} PUBLIC "-ljemalloc" "-ldl")
    endif() # NOT USE_NIX
  endif() # APPLE
endmacro(kllvm_add_tool)

configure_file(runtime/main/main.ll ${CMAKE_CURRENT_BINARY_DIR} @ONLY)
install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/main.ll runtime/main/search.cpp
  DESTINATION lib/kllvm/llvm/main
)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)
configure_file(bin/llvm-kompile bin @ONLY)
configure_file(bin/llvm-krun bin @ONLY)
configure_file(bin/llvm-kompile-testing bin @ONLY)
configure_file(bin/llvm-kompile-clang bin @ONLY)

install(
  PROGRAMS
    ${CMAKE_CURRENT_BINARY_DIR}/bin/llvm-kompile
    ${CMAKE_CURRENT_BINARY_DIR}/bin/llvm-kompile-clang
    ${CMAKE_CURRENT_BINARY_DIR}/bin/llvm-krun
  DESTINATION bin
)

install(
  DIRECTORY include/kllvm
  DESTINATION include
)

install(
  DIRECTORY include/runtime
  DESTINATION include/kllvm
)
install(
  DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include/fmt
  DESTINATION include/fmt
)
install(
  DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include/immer
  DESTINATION include/kllvm
)
install(
  DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include/rapidjson
  DESTINATION include/kllvm
)

install(
  DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include/config
  DESTINATION include/kllvm
)

file(COPY ${PROJECT_SOURCE_DIR}/deps/immer/immer DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/include)
file(COPY ${PROJECT_SOURCE_DIR}/deps/fmt/include/fmt DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/include)
file(COPY ${PROJECT_SOURCE_DIR}/deps/rapidjson/include/rapidjson DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/include)

set(GC_THRESHOLD 2097152 CACHE STRING "Initial Young Generation Size")

set(NOT_YOUNG_OBJECT_BIT 0x10000000000000)
if(CMAKE_BUILD_TYPE STREQUAL "GcStats")
set(AGE_MASK 0x7ff0000000000)
set(AGE_OFFSET 40)
set(AGE_WIDTH 11)
else()
set(AGE_MASK 0x4000000000000)
set(AGE_OFFSET 50)
set(AGE_WIDTH 1)
endif()
set(FWD_PTR_BIT 0x20000000000000)
set(VARIABLE_BIT 0x8000000000000)
set(LAYOUT_OFFSET 54)
set(TAG_MASK 0xffffffff)
set(LENGTH_MASK 0xffffffffff)
if(CMAKE_BUILD_TYPE STREQUAL "GcStats")
set(HDR_MASK -18013298997854209) # 0xffc000ffffffffff
else()
set(HDR_MASK -16888498602639361) # 0xffc3ffffffffffff
endif()

set(MAP_LAYOUT 1)
set(LIST_LAYOUT 2)
set(SET_LAYOUT 3)
set(INT_LAYOUT 4)
set(FLOAT_LAYOUT 5)
set(STRINGBUFFER_LAYOUT 6)
set(BOOL_LAYOUT 7)
set(SYMBOL_LAYOUT 8)
set(VARIABLE_LAYOUT 9)

get_filename_component(INSTALL_DIR_ABS_PATH "${CMAKE_INSTALL_PREFIX}"
                       REALPATH BASE_DIR "${CMAKE_BINARY_DIR}")

set(ERROR_TAG 4294967294) # 2^32-2

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/runtime)
configure_file(runtime/equality.ll runtime @ONLY)
configure_file(runtime/finish_rewriting.ll runtime @ONLY)
configure_file(runtime/fresh.ll runtime @ONLY)
configure_file(runtime/getTag.ll runtime @ONLY)
configure_file(runtime/move_float.ll runtime @ONLY)
configure_file(runtime/move_int.ll runtime @ONLY)
configure_file(runtime/string_equal.ll runtime @ONLY)
configure_file(runtime/take_steps.ll runtime @ONLY)

FILE(GLOB LLVMFiles ${CMAKE_CURRENT_BINARY_DIR}/runtime/*.ll)

install(
  FILES ${LLVMFiles}
  DESTINATION lib/kllvm/llvm
)

FILE(GLOB_RECURSE HaskellSources matching/src/*.hs matching/app/Main.hs)

# Auto-generated header path
set(KLLVM_AUTOGEN_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/include)
include_directories(${KLLVM_AUTOGEN_INCLUDE_DIR})

add_subdirectory(debug)
add_subdirectory(deps)
add_subdirectory(lib)
add_subdirectory(tools)
add_subdirectory(runtime)

find_package(fmt)

if(BUILD_TESTS)
  add_subdirectory(unittests)
endif()

add_subdirectory(bindings)
