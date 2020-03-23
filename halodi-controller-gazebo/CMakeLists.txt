cmake_minimum_required(VERSION 3.10)
project(halodi-controller-gazebo)


find_package(ament_cmake REQUIRED)
find_package(gazebo REQUIRED)
find_package(halodi-controller-simulation-api REQUIRED )

include_directories(${GAZEBO_INCLUDE_DIRS} ${halodi-controller-simulation-api_INCLUDE_DIRS})
link_directories(${GAZEBO_LIBRARY_DIRS})
list(APPEND CMAKE_CXX_FLAGS "${GAZEBO_CXX_FLAGS}")


add_subdirectory(src)


ament_package()
