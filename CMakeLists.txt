cmake_minimum_required(VERSION 3.10)
project(halodi-controller-simulation-api)


set(CMAKE_BUILD_TYPE RelWithDebInfo)


find_package(JNI REQUIRED)

include_directories(include ${JNI_INCLUDE_DIRS})
add_subdirectory(csrc)
