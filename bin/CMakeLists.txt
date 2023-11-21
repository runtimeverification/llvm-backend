if ((CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
     AND K_LLVM_BACKEND_LTO)
  set(LLVM_KOMPILE_LTO "lto")
else()
  set(LLVM_KOMPILE_LTO "nolto")
endif()

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)
configure_file(llvm-kompile         ${CMAKE_CURRENT_BINARY_DIR} @ONLY)
configure_file(llvm-krun            ${CMAKE_CURRENT_BINARY_DIR} @ONLY)
configure_file(llvm-kompile-testing ${CMAKE_CURRENT_BINARY_DIR} @ONLY)
configure_file(llvm-kompile-clang   ${CMAKE_CURRENT_BINARY_DIR} @ONLY)
configure_file(utils.sh             ${CMAKE_CURRENT_BINARY_DIR} @ONLY)

install(
  PROGRAMS
    ${CMAKE_CURRENT_BINARY_DIR}/llvm-kompile
    ${CMAKE_CURRENT_BINARY_DIR}/llvm-kompile-clang
    ${CMAKE_CURRENT_BINARY_DIR}/llvm-krun
  DESTINATION bin
)

install(
  PROGRAMS
    ${CMAKE_CURRENT_BINARY_DIR}/utils.sh
  DESTINATION lib/kllvm/scripts
)
