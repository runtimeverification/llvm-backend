add_library(base64
  src/base64.cpp)

target_include_directories(base64
  PRIVATE   include/base64
  INTERFACE include)
