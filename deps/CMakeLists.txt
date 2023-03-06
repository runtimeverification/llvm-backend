file(
  COPY
    ${CMAKE_CURRENT_SOURCE_DIR}/immer/immer
    ${CMAKE_CURRENT_SOURCE_DIR}/fmt/include/fmt
    ${CMAKE_CURRENT_SOURCE_DIR}/rapidjson/include/rapidjson
  DESTINATION
    ${CMAKE_BINARY_DIR}/include
)

add_subdirectory(base64)
add_subdirectory(fmt)
add_subdirectory(pybind11)
