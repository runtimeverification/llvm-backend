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

macro(kllvm_add_tool name)
  add_executable(${name} ${ARGN})

  llvm_config(${name}
    USE_SHARED true
    core irreader
  )

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

include(RuntimeConfig)

# Auto-generated header path
set(KLLVM_AUTOGEN_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/include)
include_directories(${KLLVM_AUTOGEN_INCLUDE_DIR})

add_subdirectory(bindings)
add_subdirectory(debug)
add_subdirectory(lib)
add_subdirectory(tools)
add_subdirectory(runtime)

if(BUILD_TESTS)
  add_subdirectory(unittests)
endif()
