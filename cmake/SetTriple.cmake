execute_process(
  COMMAND llvm-config --host-target
  OUTPUT_VARIABLE BACKEND_TARGET_TRIPLE
  OUTPUT_STRIP_TRAILING_WHITESPACE)

execute_process(
  COMMAND llvm-config --bindir
  OUTPUT_VARIABLE LLVM_BIN_DIR
  OUTPUT_STRIP_TRAILING_WHITESPACE)

# This is the best way I can figure out to get a _working_ default datalayout
# for the host platform. If it's left blank (and the LLVM internals supply one),
# something breaks the ARM/macOS ABI and produces crashes.
#
# The pipeline here generates an empty module using the appropriate version of
# clang, and figures out what the _frontend_ thinks the datalayout should be by
# scraping it from the generated LLVM.
execute_process(
  COMMAND "${CMAKE_SOURCE_DIR}/cmake/guess-triple.sh" "${LLVM_BIN_DIR}/clang"
  OUTPUT_VARIABLE BACKEND_TARGET_DATALAYOUT
  OUTPUT_STRIP_TRAILING_WHITESPACE)

message(STATUS "Using LLVM datalayout: ${BACKEND_TARGET_DATALAYOUT}")
message(STATUS "Using LLVM triple    : ${BACKEND_TARGET_TRIPLE}")
