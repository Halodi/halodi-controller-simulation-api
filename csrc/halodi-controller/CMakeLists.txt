add_library(halodi-controller-api SHARED halodicontroller.cpp NativeIMUHandleHolder.cpp NativeEffortJointHandleHolder.cpp)
target_link_libraries(halodi-controller-api jvmLauncher)

set_property(TARGET halodi-controller-api PROPERTY INSTALL_RPATH_USE_LINK_PATH TRUE)


