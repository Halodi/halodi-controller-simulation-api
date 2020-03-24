
add_library(halodi-controller-plugin SHARED halodicontrollerplugin.cpp)
#target_link_libraries(halodi-controller-plugin ${GAZEBO_LIBRARIES} ${halodi-controller-simulation-api_LIBRARIES})

ament_target_dependencies(halodi-controller-plugin gazebo halodi-controller-simulation-api)

install(
    TARGETS halodi-controller-plugin
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)
