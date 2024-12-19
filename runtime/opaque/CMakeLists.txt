add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/runtime/opaque/opaque_cpp.cpp
	COMMAND xxd -i opaque.ll opaque_cpp.cpp
	DEPENDS opaque.ll
	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/runtime/opaque
)

add_library(opaque-cpp STATIC
  ${CMAKE_BINARY_DIR}/runtime/opaque/opaque_cpp.cpp
)

set_target_properties(opaque-cpp PROPERTIES EXPORT_COMPILE_COMMANDS Off)
