add_subdirectory(arithmetic)
add_subdirectory(configurationparser)
add_subdirectory(strings)
add_subdirectory(meta)
add_subdirectory(alloc)

set(RUSTDIR_ debug)
set(RUSTDIR_Debug debug)
set(RUSTDIR_Release release)
set(RUSTFLAGS_Release --release)
set(RUSTFLAGS ${RUSTFLAGS_${CMAKE_BUILD_TYPE}})
set(RUSTDIR ${RUSTDIR_${CMAKE_BUILD_TYPE}})

FILE(GLOB DatastructuresSources datastructures/Cargo.toml datastructures/src/*.rs)

set(RUSTLIB "datastructures/target/${RUSTDIR}/libdatastructures.rlib")
set(RUSTDEPS datastructures/target/${RUSTDIR}/deps/)
execute_process(
  COMMAND rustc --print sysroot
  OUTPUT_VARIABLE RUSTSYSROOT
  OUTPUT_STRIP_TRAILING_WHITESPACE)
set(RUSTSTDLIB "${RUSTSYSROOT}/lib/rustlib/${RUST_TARGET_TRIPLE}/lib/")

add_custom_command(
  OUTPUT ${RUSTLIB}
  DEPENDS ${DatastructuresSources}
  COMMAND cargo build ${RUSTFLAGS}
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/runtime/datastructures
)

add_custom_target(datastructures
  ALL DEPENDS ${RUSTLIB}
)

install(
  FILES ${RUSTLIB}
  DESTINATION lib/kllvm/rust
)

install(
  DIRECTORY ${RUSTDEPS}
  DESTINATION lib/kllvm/rust/deps
  FILES_MATCHING
  PATTERN *.rlib
)

install(
  DIRECTORY ${RUSTSTDLIB}
  DESTINATION lib/kllvm/rust/deps/stdlib
  FILES_MATCHING
  PATTERN libcore-*.rlib
  PATTERN libstd-*.rlib
  PATTERN liballoc-*.rlib
  PATTERN libunwind-*.rlib
  PATTERN libcompiler_builtins-*.rlib
  PATTERN libpanic_abort-*.rlib
  PATTERN libbacktrace_sys-*.rlib
  PATTERN librustc_demangle-*.rlib
  PATTERN libcfg_if-*.rlib
)

if (CMAKE_BUILD_TYPE STREQUAL "Release")
  install(SCRIPT ${PROJECT_SOURCE_DIR}/cmake/mkrlib.cmake)
endif()
