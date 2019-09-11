set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AR "${CMAKE_C_COMPILER_AR}")
set(CMAKE_RANLIB "${CMAKE_C_COMPILER_RANLIB}")

find_program(LLVM_KOMPILE llvm-kompile)

set(LLVM_KOMPILE_FLAGS "${CMAKE_CURRENT_BINARY_DIR}/definition.kore ${KOMPILED_DIR}/dt ${KOMPILE_USE_MAIN}")
set(CMAKE_CXX_LINK_EXECUTABLE "${LLVM_KOMPILE} ${LLVM_KOMPILE_FLAGS} <FLAGS> <CMAKE_CXX_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")

get_filename_component(K_BIN_DIR ${LLVM_KOMPILE} DIRECTORY)
include_directories(${K_BIN_DIR}/../include)

add_custom_command(
	OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/definition.kore"
	COMMAND "${CMAKE_COMMAND}" -E copy "${KOMPILED_DIR}/definition.kore" "${CMAKE_CURRENT_BINARY_DIR}"
	DEPENDS "${KOMPILED_DIR}/definition.kore")
add_custom_target(definition
	DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/definition.kore")

target_compile_options(${TARGET_NAME} -Wno-return-type-c-linkage)
add_dependencies(${TARGET_NAME} definition)
set_target_properties(${TARGET_NAME} PROPERTIES LINK_DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/definition.kore")
