add_library(ffitest SHARED
  foreign.cpp
)

set(LIB_FFITEST_DEST "../unittests/runtime-ffi/lib")

install(TARGETS ffitest
  LIBRARY DESTINATION ${LIB_FFITEST_DEST})
