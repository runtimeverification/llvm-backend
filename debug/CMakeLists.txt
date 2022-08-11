if(APPLE)
  # We need to hard-code the path to the system (i.e. XCode) installation of
  # lldb here because a Homebrew version is likely to want a different version
  # of Python 3 than the default macOS version.
  set(LLDB_PATH "/usr/local/bin/lldb")

  if(EXISTS ${LLDB_PATH})
    execute_process(
      COMMAND ${LLDB_PATH}
      ARGUMENTS "-P"
      OUTPUT_VARIABLE LLDB_PYTHONPATH
      OUTPUT_STRIP_TRAILING_WHITESPACE)

    configure_file(klldb.py ${CMAKE_CURRENT_BINARY_DIR} @ONLY)

    install(
      PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/klldb.py
      DESTINATION lib/lldb)
  else()
    message(WARNING "No compatible LLDB installation found")
  endif()
endif()

set(GDB_SCRIPT_PATH ${CMAKE_CURRENT_BINARY_DIR})
set(GDB_SCRIPT_NAME kgdb.py)

configure_file(kgdb.py ${CMAKE_CURRENT_BINARY_DIR} @ONLY)

configure_file(
  ${CMAKE_SOURCE_DIR}/config/macros.h
  ${CMAKE_BINARY_DIR}/include/config/macros.h
  @ONLY)
