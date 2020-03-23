
add_library(halodi-controller-plugin SHARED halodicontrollerplugin.cpp)
target_link_libraries(halodi-controller-plugin ${GAZEBO_LIBRARIES} ${halodi-controller-simulation-api_LIBRARIES})
