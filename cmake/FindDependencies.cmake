include(FindLLVM)

find_package(PkgConfig  REQUIRED)
find_package(ZLIB       REQUIRED)
find_package(FLEX       REQUIRED)
find_package(GMP        REQUIRED)
find_package(Boost      REQUIRED COMPONENTS unit_test_framework)

pkg_check_modules(FFI REQUIRED libffi)
if(USE_NIX)
  include_directories(AFTER         ${FFI_INCLUDE_DIRS})
  link_directories(   AFTER         ${FFI_LIBRARY_DIRS})
else() # USE_NIX
  include_directories(AFTER SYSTEM  ${FFI_INCLUDE_DIRS})
  link_directories(   AFTER         ${FFI_LIBRARY_DIRS})
endif() # USE_NIX
