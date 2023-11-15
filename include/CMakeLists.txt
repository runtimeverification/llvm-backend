install(
  DIRECTORY kllvm
  DESTINATION include
)

install(
  DIRECTORY runtime
  DESTINATION include/kllvm
)
install(
  DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/fmt
  DESTINATION include/kllvm
)
install(
  DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/immer
  DESTINATION include/kllvm
)
install(
  DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/rapidjson
  DESTINATION include/kllvm
)

install(
  DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/config
  DESTINATION include/kllvm
)
