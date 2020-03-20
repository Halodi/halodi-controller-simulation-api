add_subdirectory(jvmLauncher)
add_subdirectory(halodi-controller)

add_executable(demo demo.cpp)
target_link_libraries(demo jvmLauncher)
