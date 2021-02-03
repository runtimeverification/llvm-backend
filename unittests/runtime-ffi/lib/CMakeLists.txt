add_library(ffitest SHARED
  foreign.cpp
)
set_target_properties(ffitest PROPERTIES EXCLUDE_FROM_ALL TRUE)

set(LIB_FFITEST_DEST "../unittests/runtime-ffi/lib")

install(TARGETS ffitest
  LIBRARY DESTINATION ${LIB_FFITEST_DEST})
