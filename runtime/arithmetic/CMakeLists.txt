
add_library(arithmetic STATIC lib/int.c)
add_executable(unittests lib/int.c test/main.cpp test/inttest.cpp)

target_link_libraries(unittests gmp ${Boost_UNIT_TEST_FRAMEWORK_LIBRARIES})

install(TARGETS arithmetic DESTINATION lib)
install(TARGETS unittests DESTINATION bin)
