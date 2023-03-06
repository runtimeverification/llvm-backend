set(GDB_SCRIPT_PATH ${CMAKE_CURRENT_BINARY_DIR})
set(GDB_SCRIPT_NAME kgdb.py)

configure_file(kgdb.py  ${CMAKE_CURRENT_BINARY_DIR} @ONLY)
configure_file(klldb.py ${CMAKE_CURRENT_BINARY_DIR} @ONLY)

install(
  PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/klldb.py
  DESTINATION lib/kllvm/lldb)

configure_file(
  ${CMAKE_SOURCE_DIR}/config/macros.h
  ${CMAKE_BINARY_DIR}/include/config/macros.h
  @ONLY)
