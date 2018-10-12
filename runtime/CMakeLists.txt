add_subdirectory(arithmetic)
add_subdirectory(configurationparser)
add_subdirectory(strings)

set(RUSTDIR_ debug)
set(RUSTDIR_Debug debug)
set(RUSTDIR_Release release)
set(RUSTFLAGS_Release --release)
set(RUSTFLAGS ${RUSTFLAGS_${CMAKE_BUILD_TYPE}})
set(RUSTDIR ${RUSTDIR_${CMAKE_BUILD_TYPE}})

FILE(GLOB DatastructuresSources datastructures/Cargo.toml datastructures/src/*.rs)

set(RUSTLIB "datastructures/target/${RUSTDIR}/libdatastructures.rlib")
set(RUSTDEPS datastructures/target/${RUSTDIR}/deps/)

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
  PATTERN *.rlib
)
