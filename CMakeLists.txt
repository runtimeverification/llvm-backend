cmake_minimum_required (VERSION 3.4)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(LLVMKompilePrelude)
project (KLLVM CXX C)

set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
if(NOT CMAKE_BUILD_TYPE STREQUAL "DebugGcc")
add_definitions(-Wall -Werror -Wno-return-type-c-linkage)
endif()
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-stack-protector")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-stack-protector")
set(CMAKE_C_FLAGS_DEBUGGCC "${CMAKE_C_FLAGS_DEBUG}")
set(CMAKE_C_FLAGS_FASTBUILD "${CMAKE_C_FLAGS_RELEASE}")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -flto")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} -flto -fno-omit-frame-pointer")
set(CMAKE_CXX_FLAGS_DEBUGGCC "${CMAKE_CXX_FLAGS_DEBUG}")
set(CMAKE_CXX_FLAGS_FASTBUILD "${CMAKE_CXX_FLAGS_RELEASE}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -flto")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -flto -fno-omit-frame-pointer")
set(CMAKE_EXE_LINKER_FLAGS_DEBUGGCC "${CMAKE_EXE_LINKER_FLAGS_DEBUG}")
set(CMAKE_EXE_LINKER_FLAGS_FASTBUILD "${CMAKE_EXE_LINKER_FLAGS_RELEASE}")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -flto")
set(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -flto")

if(APPLE)
set(BACKEND_TARGET_TRIPLE "x86_64-apple-darwin")
include_directories(AFTER SYSTEM /usr/local/include)
link_directories(AFTER /usr/local/lib)
set(ENV{PKG_CONFIG_PATH} "/usr/local/opt/libffi/lib/pkgconfig")
else()
set(BACKEND_TARGET_TRIPLE "x86_64-unknown-linux-gnu")
endif()

find_package(PkgConfig REQUIRED)
pkg_check_modules(FFI REQUIRED libffi)
include_directories(AFTER SYSTEM ${FFI_INCLUDE_DIRS})
link_directories(AFTER SYSTEM ${FFI_LIBRARY_DIRS})

if (CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
set(LLVM_KOMPILE_LTO "lto")
else()
set(LLVM_KOMPILE_LTO "nolto")
endif()

set(CMAKE_AR "${CMAKE_C_COMPILER_AR}")
set(CMAKE_RANLIB "${CMAKE_C_COMPILER_RANLIB}")

find_package(LLVM 8 QUIET CONFIG)

if (NOT LLVM_FOUND)
  find_package(LLVM 7 REQUIRED CONFIG)
endif()

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
if (${LLVM_PACKAGE_VERSION} VERSION_LESS 7.0.1)
  message(FATAL_ERROR "LLVM 7.0.1 or newer is required")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(LLVM_BUILD_TOOLS ON)

find_package(ZLIB REQUIRED)
find_package(FLEX REQUIRED)
find_package(GMP REQUIRED)
find_package(Boost REQUIRED COMPONENTS unit_test_framework)

link_directories(${LLVM_LIBRARY_DIR})
include_directories(${LLVM_INCLUDE_DIR})
include_directories (${PROJECT_SOURCE_DIR}/include)

# Find the libraries that correspond to the LLVM components
# that we wish to use
llvm_map_components_to_libnames(llvm_libs support core irreader)

macro(kllvm_add_tool name)
  add_executable(${name} ${ARGN})

  # Link against LLVM libraries
  llvm_config(${name})
if(APPLE)
  target_link_libraries(${name} PUBLIC "-ljemalloc" "-Wl,-rpath /usr/local/lib" "-ldl")
else()
  target_link_libraries(${name} PUBLIC "-ljemalloc" "-Wl,-rpath=/usr/local/lib" "-ldl")
endif()
endmacro(kllvm_add_tool)

install(
  FILES runtime/main/main.ll
  DESTINATION lib/kllvm/llvm/main
)

find_program(OPT opt-8)
find_program(OPT opt-7)
find_program(OPT opt)
find_program(LLC llc-8)
find_program(LLC llc-7)
find_program(LLC llc)
if(${OPT} STREQUAL "OPT-NOTFOUND")
  message(FATAL_ERROR "Could not find an opt binary. Is llvm installed on your PATH?")
endif()
if(${LLC} STREQUAL "OPT-NOTFOUND")
  message(FATAL_ERROR "Could not find an llvm binary. Is llvm installed on your PATH?")
endif()

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
  DIRECTORY include/runtime
  DESTINATION include
)
install(
  DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include/immer
  DESTINATION include
)

install(
  DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include/config
  DESTINATION include
)

file(COPY ${PROJECT_SOURCE_DIR}/deps/immer/immer DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/include)

set(GC_INTERVAL 2000 CACHE STRING "Garbage collect every N steps")

set(NOT_YOUNG_OBJECT_BIT 0x10000000000000)
set(YOUNG_AGE_BIT 0x8000000000000)
set(FWD_PTR_BIT 0x20000000000000)
set(VARIABLE_BIT 0x4000000000000)
set(LAYOUT_OFFSET 54)
set(HDR_MASK -16888498602639361) # 0xffc3ffffffffffff

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

configure_file(config/macros.h include/config/macros.h @ONLY)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/runtime)
configure_file(runtime/equality.ll runtime @ONLY)
configure_file(runtime/finish_rewriting.ll runtime @ONLY)
configure_file(runtime/fresh.ll runtime @ONLY)
configure_file(runtime/getTag.ll runtime @ONLY)
configure_file(runtime/hash.ll runtime @ONLY)
configure_file(runtime/move_float.ll runtime @ONLY)
configure_file(runtime/move_int.ll runtime @ONLY)
configure_file(runtime/string_equal.ll runtime @ONLY)
configure_file(runtime/take_steps.ll runtime @ONLY)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/gdb)
configure_file(gdb/interpreter-gdb.py gdb @ONLY)
configure_file(gdb/kllvm.py gdb @ONLY)
configure_file(gdb/__init__.py gdb @ONLY)

FILE(GLOB LLVMFiles ${CMAKE_CURRENT_BINARY_DIR}/runtime/*.ll)
FILE(GLOB GDBFiles ${CMAKE_CURRENT_BINARY_DIR}/gdb/*.py)

install(
  FILES ${LLVMFiles}
  DESTINATION lib/kllvm/llvm
)

install(
  FILES ${GDBFiles}
  DESTINATION lib/kllvm/gdb
)

FILE(GLOB_RECURSE HaskellSources matching/src/*.hs matching/app/Main.hs)

# Auto-generated header path
set(KLLVM_AUTOGEN_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/include)
include_directories(${KLLVM_AUTOGEN_INCLUDE_DIR})

add_subdirectory(lib)
add_subdirectory(tools)
add_subdirectory(runtime)
add_subdirectory(unittests)
#add_subdirectory(test)
