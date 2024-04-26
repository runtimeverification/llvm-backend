file(
  READ "${PROJECT_SOURCE_DIR}/package/version"
  LLVM_BACKEND_VERSION
)

configure_file(version.inc.in version.inc @ONLY)

kllvm_add_tool(llvm-backend-version version.cpp)

target_include_directories(llvm-backend-version
  PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

install(
  TARGETS llvm-backend-version
  RUNTIME DESTINATION bin
)
