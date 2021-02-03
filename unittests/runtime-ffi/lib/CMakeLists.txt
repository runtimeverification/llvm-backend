add_library(ffitest SHARED
  foreign.cpp
)
set_target_properties(ffitest PROPERTIES EXCLUDE_FROM_ALL TRUE)
