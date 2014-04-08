# add the binary tree to the search path for include files
# so that we will find CassiopeeConfig.h


include_directories("${PROJECT_BINARY_DIR}")

include_directories("${PROJECT_SOURCE_DIR}")

include_directories("${CMAKE_SOURCE_DIR}/src")

include( CTest )

add_executable(test_cassiopee test.cxx)
target_link_libraries(test_cassiopee cassie glog cppunit)


add_test(NAME "TestCassie" COMMAND ${PROJECT_BINARY_DIR}/test_cassiopee WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
