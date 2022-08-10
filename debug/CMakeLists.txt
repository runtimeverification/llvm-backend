if(APPLE)
  find_program(LLDB lldb)

  if(LLDB)
    execute_process(
      COMMAND "${LLDB}"
      ARGUMENTS "-P"
      OUTPUT_VARIABLE LLDB_PYTHONPATH
      OUTPUT_STRIP_TRAILING_WHITESPACE)

    configure_file(k-lldb ${CMAKE_CURRENT_BINARY_DIR} @ONLY)

    install(
      PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/k-lldb
      DESTINATION bin)
  else()
    message(WARNING "No LLDB installation found")
  endif()
endif()

set(GDB_SCRIPT_PATH ${CMAKE_CURRENT_BINARY_DIR})
set(GDB_SCRIPT_NAME kllvm.py)

configure_file(kllvm.py ${CMAKE_CURRENT_BINARY_DIR} @ONLY)

configure_file(
  ${CMAKE_SOURCE_DIR}/config/macros.h
  ${CMAKE_BINARY_DIR}/include/config/macros.h
  @ONLY)
