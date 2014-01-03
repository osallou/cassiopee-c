cmake_minimum_required (VERSION 2.6)
project (Cassiopee)

link_directories(/usr/local/lib)

add_subdirectory (test)

set (Cassiopee_VERSION_MAJOR 1)
set (Cassiopee_VERSION_MINOR 0)

set (PROJECT_SOURCE_DIR src)
set (PROJECT_BINARY_DIR bin)
set (PROJECT_DOC_DIR doc)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")


# add a target to generate API documentation with Doxygen
find_package(Doxygen)
if(DOXYGEN_FOUND)
configure_file(${PROJECT_SOURCE_DIR}/Doxyfile.in ${PROJECT_BINARY_DIR}/Doxyfile @ONLY)
add_custom_target(doc
${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_BINARY_DIR}/Doxyfile
WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
COMMENT "Generating API documentation with Doxygen" VERBATIM
)
endif(DOXYGEN_FOUND)

FIND_PACKAGE(OpenMP)
if(OPENMP_FOUND)
set(USE_OPENMP 1)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
else()
set(USE_OPENMP 0)
endif()

# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
  "${PROJECT_SOURCE_DIR}/CassiopeeConfig.h.in"
  "${PROJECT_BINARY_DIR}/CassiopeeConfig.h"
  )
 
# add the binary tree to the search path for include files
# so that we will find CassiopeeConfig.h
include_directories("${PROJECT_BINARY_DIR}")

add_library(Cassie ${PROJECT_SOURCE_DIR}/cassiopee.cxx)
add_executable(Cassiopee ${PROJECT_SOURCE_DIR}/cassie.cxx)

target_link_libraries(Cassiopee Cassie glog)

# add the install targets
install (TARGETS Cassiopee DESTINATION bin)
install (FILES "${PROJECT_BINARY_DIR}/CassiopeeConfig.h"        
         DESTINATION include)
