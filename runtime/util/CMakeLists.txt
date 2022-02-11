add_library(ConfigurationPrinter OBJECT
  ConfigurationPrinter.cpp
)

add_library(util STATIC
  ConfigurationParser.cpp
  $<TARGET_OBJECTS:ConfigurationPrinter>
  search.cpp
)

if(NOT APPLE)
  add_custom_command(TARGET util PRE_BUILD
    COMMAND ${PRINT_GDB_SECTION} ${GDBFiles} > debug_gdb_scripts.bin
    COMMAND ${CMAKE_OBJCOPY} -I binary -O elf64-x86-64 -B i386 --rename-section .data=.debug_gdb_scripts debug_gdb_scripts.bin debug_gdb_scripts.a
    COMMAND ${CMAKE_LINKER} --relocatable debug_gdb_scripts.a $<TARGET_OBJECTS:ConfigurationPrinter> -o temp.a
    COMMAND mv temp.a $<TARGET_OBJECTS:ConfigurationPrinter>
    DEPENDS ConfigurationPrinter
  )
endif()

install(
  TARGETS util
  ARCHIVE DESTINATION lib/kllvm
)

target_link_libraries(util PUBLIC Parser AST)
