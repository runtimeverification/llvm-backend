if(NOT LLVM_CONFIG_PATH)
  set(LLVM_CONFIG_PATH ${LLVM_TOOLS_BINARY_DIR}/llvm-config)
endif()

execute_process(
  COMMAND ${LLVM_CONFIG_PATH} --host-target
  OUTPUT_VARIABLE BACKEND_TARGET_TRIPLE
  OUTPUT_STRIP_TRAILING_WHITESPACE)

if(NOT LLVM_CLANG_PATH)
  set(LLVM_CLANG_PATH "${LLVM_TOOLS_BINARY_DIR}/clang")
endif()

# This is the best way I can figure out to get a _working_ default datalayout
# for the host platform. If it's left blank (and the LLVM internals supply one),
# something breaks the ARM/macOS ABI and produces crashes.
#
# The pipeline here generates an empty module using the appropriate version of
# clang, and figures out what the _frontend_ thinks the datalayout should be by
# scraping it from the generated LLVM.
execute_process(
  COMMAND "sh" "${CMAKE_SOURCE_DIR}/cmake/guess-triple.sh" "${LLVM_CLANG_PATH}"
  OUTPUT_VARIABLE BACKEND_TARGET_DATALAYOUT
  OUTPUT_STRIP_TRAILING_WHITESPACE)

message(STATUS "Using LLVM datalayout: ${BACKEND_TARGET_DATALAYOUT}")
message(STATUS "Using LLVM triple    : ${BACKEND_TARGET_TRIPLE}")
