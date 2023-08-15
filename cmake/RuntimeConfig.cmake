set(GC_THRESHOLD          2097152
    CACHE STRING "Initial Young Generation Size")

set(NOT_YOUNG_OBJECT_BIT  0x10000000000000)

if(CMAKE_BUILD_TYPE STREQUAL "GcStats")
  set(AGE_MASK    0x7ff0000000000)
  set(AGE_OFFSET  40)
  set(AGE_WIDTH   11)
else()
  set(AGE_MASK    0x4000000000000)
  set(AGE_OFFSET  50)
  set(AGE_WIDTH   1)
endif()

set(FWD_PTR_BIT   0x20000000000000)
set(VARIABLE_BIT  0x8000000000000)
set(LAYOUT_OFFSET 54)
set(TAG_MASK      0xffffffff)
set(LENGTH_MASK   0xffffffffff)
set(IS_BYTES_BIT  0x10000000000)

if(CMAKE_BUILD_TYPE STREQUAL "GcStats")
  set(HDR_MASK -18013298997854209) # 0xffc000ffffffffff
else()
  set(HDR_MASK -16888498602639361) # 0xffc3ffffffffffff
endif()

set(MAP_LAYOUT          1)
set(LIST_LAYOUT         2)
set(SET_LAYOUT          3)
set(INT_LAYOUT          4)
set(FLOAT_LAYOUT        5)
set(STRINGBUFFER_LAYOUT 6)
set(BOOL_LAYOUT         7)
set(SYMBOL_LAYOUT       8)
set(VARIABLE_LAYOUT     9)
set(RANGEMAP_LAYOUT    11)

get_filename_component(INSTALL_DIR_ABS_PATH "${CMAKE_INSTALL_PREFIX}"
                       REALPATH BASE_DIR "${CMAKE_BINARY_DIR}")

set(ERROR_TAG 4294967294) # 2^32-2
