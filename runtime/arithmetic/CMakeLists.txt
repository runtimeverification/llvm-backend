cmake_minimum_required (VERSION 3.4)

project(ArithmeticHooks CXX C)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
add_definitions(-Wall)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

find_package(GMP REQUIRED)
find_package(Boost REQUIRED COMPONENTS unit_test_framework)

add_library(arithmetic STATIC lib/int.c)
add_executable(unittests lib/int.c test/main.cpp test/inttest.cpp)

target_link_libraries(unittests gmp ${Boost_UNIT_TEST_FRAMEWORK_LIBRARIES})

install(TARGETS arithmetic DESTINATION lib)
install(TARGETS unittests DESTINATION bin)
