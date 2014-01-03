# add the binary tree to the search path for include files
# so that we will find CassiopeeConfig.h
include_directories("${PROJECT_BINARY_DIR}")

include_directories("${PROJECT_SOURCE_DIR}")

include( CTest )

add_executable(test_cassiopee test.cxx)
target_link_libraries(test_cassiopee Cassie glog cppunit)


add_test(CassieTest test_cassiopee)
