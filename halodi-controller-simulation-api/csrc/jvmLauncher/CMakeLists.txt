
add_library(jvmLauncher STATIC javavirtualmachine.cpp)
target_link_libraries(jvmLauncher platform)
add_dependencies(jvmLauncher JDK)

set_property(TARGET jvmLauncher PROPERTY INSTALL_RPATH_USE_LINK_PATH TRUE)
set_property(TARGET jvmLauncher PROPERTY POSITION_INDEPENDENT_CODE ON)



