add_library(halodi-controller-api SHARED halodicontroller.cpp halodicontrollerimplementation.cpp NativeIMUHandleHolder.cpp NativeEffortJointHandleHolder.cpp NativeForceTorqueSensorHandleHolder.cpp)
target_link_libraries(halodi-controller-api jvmLauncher)

set_property(TARGET halodi-controller-api PROPERTY INSTALL_RPATH_USE_LINK_PATH TRUE)


if(NOT ament_cmake_FOUND)
    add_executable(halodi-controller-demo halodicontrollerdemo.cpp)
    target_link_libraries(halodi-controller-demo jvmLauncher halodi-controller-api)
endif()


install(TARGETS halodi-controller-api
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)
