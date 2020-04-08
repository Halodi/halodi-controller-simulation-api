
add_library(jvmLauncher STATIC javavirtualmachine.cpp)
target_link_libraries(jvmLauncher ${JNI_LIBRARIES})
add_dependencies(jvmLauncher JDK)

set_property(TARGET jvmLauncher PROPERTY INSTALL_RPATH_USE_LINK_PATH TRUE)
set_property(TARGET jvmLauncher PROPERTY POSITION_INDEPENDENT_CODE ON)


if(NOT ament_cmake_FOUND)
    add_executable(jvm-launcher-demo demo.cpp)
    target_link_libraries(jvm-launcher-demo jvmLauncher)
endif()

