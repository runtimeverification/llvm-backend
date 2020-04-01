set(CMAKE_AR "${CMAKE_C_COMPILER_AR}")
set(CMAKE_RANLIB "${CMAKE_C_COMPILER_RANLIB}")

find_program(LLVM_KOMPILE llvm-kompile)

set(LLVM_KOMPILE_FLAGS "${KOMPILED_DIR}/definition.o ${KOMPILE_USE_MAIN}")
set(CMAKE_CXX_LINK_EXECUTABLE "${LLVM_KOMPILE} ${LLVM_KOMPILE_FLAGS} <FLAGS> <CMAKE_CXX_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")

get_filename_component(K_BIN_DIR ${LLVM_KOMPILE} DIRECTORY)
include_directories(${K_BIN_DIR}/../include)

get_source_file_property(LLVM_KOMPILE_FLAGS ${KOMPILED_DIR}/definition.o COMPILE_FLAGS)
add_custom_command(
	OUTPUT "${KOMPILED_DIR}/definition.o"
	COMMAND ${LLVM_KOMPILE} ${KOMPILED_DIR}/definition.kore ${KOMPILED_DIR}/dt ${KOMPILE_USE_MAIN} ${LLVM_KOMPILE_FLAGS} -o ${KOMPILED_DIR}/definition.o -c
	DEPENDS ${KOMPILED_DIR}/definition.kore)

target_compile_options(${TARGET_NAME}
	PUBLIC -Wno-return-type-c-linkage)

install(TARGETS ${TARGET_NAME}
	RUNTIME DESTINATION bin)

if(APPLE)
  include_directories(AFTER SYSTEM /usr/local/include)
  target_link_directories(${TARGET_NAME} PUBLIC /usr/local/lib)
endif()
