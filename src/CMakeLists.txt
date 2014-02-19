
#add_library(Cassie cassiopee.cxx)
add_library(Cassie SHARED cassiopee.cxx)
add_executable(Cassiopee cassie.cxx)
add_executable(CassiopeeKnife knife.cxx)

target_link_libraries(Cassie glog ${Boost_LIBRARIES} )
target_link_libraries(Cassiopee Cassie glog ${Boost_LIBRARIES} )

# add the install targets
install (TARGETS Cassiopee DESTINATION bin)
install (TARGETS CassiopeeKnife DESTINATION bin)
install (FILES "${PROJECT_BINARY_DIR}/CassiopeeConfig.h"        
         DESTINATION include)
