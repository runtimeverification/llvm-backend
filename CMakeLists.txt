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

add_subdirectory(deps)
include(FindDependencies)

include(KLLVMCompilerFlags)

include(SetTriple)

include(RuntimeConfig)

add_subdirectory(bin)
add_subdirectory(bindings)
add_subdirectory(debug)
add_subdirectory(include)
add_subdirectory(lib)
add_subdirectory(tools)
add_subdirectory(runtime)

if(BUILD_TESTS)
  add_subdirectory(unittests)
endif()
