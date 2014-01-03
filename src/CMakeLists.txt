
add_library(Cassie cassiopee.cxx)
add_executable(Cassiopee cassie.cxx)

target_link_libraries(Cassiopee Cassie glog)

# add the install targets
install (TARGETS Cassiopee DESTINATION bin)
install (FILES "${PROJECT_BINARY_DIR}/CassiopeeConfig.h"        
         DESTINATION include)
