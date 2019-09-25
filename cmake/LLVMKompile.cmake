set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AR "${CMAKE_C_COMPILER_AR}")
set(CMAKE_RANLIB "${CMAKE_C_COMPILER_RANLIB}")

find_program(LLVM_KOMPILE llvm-kompile)

set(LLVM_KOMPILE_FLAGS "${KOMPILED_DIR}/definition.kore ${KOMPILED_DIR}/dt ${KOMPILE_USE_MAIN}")
set(CMAKE_CXX_LINK_EXECUTABLE "${LLVM_KOMPILE} ${LLVM_KOMPILE_FLAGS} <FLAGS> <CMAKE_CXX_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")

get_filename_component(K_BIN_DIR ${LLVM_KOMPILE} DIRECTORY)
include_directories(${K_BIN_DIR}/../include)

target_compile_options(${TARGET_NAME}
	PUBLIC -Wno-return-type-c-linkage)
set_target_properties(${TARGET_NAME} PROPERTIES LINK_DEPENDS "${KOMPILED_DIR}/definition.kore")

install(TARGETS ${TARGET_NAME}
	RUNTIME DESTINATION bin)

if(APPLE)
  include_directories(AFTER SYSTEM /usr/local/include)
  target_link_directories(${TARGET_NAME} PUBLIC /usr/local/lib)
endif()
