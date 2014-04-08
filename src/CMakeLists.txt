
set(Cassie_VERSION_STRING ${Cassiopee_VERSION_MAJOR}.${Cassiopee_VERSION_MINOR}.0)
add_library(cassie SHARED cassiopee.cxx)
set_target_properties(cassie PROPERTIES VERSION  ${Cassie_VERSION_STRING} SOVERSION ${Cassiopee_VERSION_MAJOR})
add_executable(cassiopee cassie.cxx)
add_executable(cassiopeeknife knife.cxx)

target_link_libraries(cassie glog ${Boost_LIBRARIES} )
target_link_libraries(cassiopee cassie glog ${Boost_LIBRARIES} )
target_link_libraries(cassiopeeknife cassie glog ${Boost_LIBRARIES} )

# add the install targets
install (TARGETS cassiopee DESTINATION bin)
install (TARGETS cassiopeeknife DESTINATION bin)
install (FILES "${PROJECT_BINARY_DIR}/CassiopeeConfig.h"        
         DESTINATION include)
install (TARGETS cassie DESTINATION lib)

set(HS Cassiopee.h tree/tree.hh tree/tree_util.hh)
MACRO(INSTALL_HEADERS_WITH_DIRECTORY HEADER_LIST)

FOREACH(HEADER ${${HEADER_LIST}})
STRING(REGEX MATCH "(.*)[/\\]" DIR ${HEADER})
INSTALL(FILES ${HEADER} DESTINATION include/${DIR})
ENDFOREACH(HEADER)

ENDMACRO(INSTALL_HEADERS_WITH_DIRECTORY)

INSTALL_HEADERS_WITH_DIRECTORY(HS)

