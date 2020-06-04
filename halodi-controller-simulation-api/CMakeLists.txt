cmake_minimum_required(VERSION 3.10)
project(halodi-controller-simulation-api)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")


set(CMAKE_BUILD_TYPE RelWithDebInfo)

set (CMAKE_CXX_STANDARD 17)


find_package(ament_cmake)

include(FindOrDownloadJDK)
include_directories(include ${JNI_INCLUDE_DIRS})

add_subdirectory(csrc)

install(
  DIRECTORY include/
  DESTINATION include
)


if(ament_cmake_FOUND)
    
    ament_export_include_directories(include)
    ament_export_libraries(halodi-controller-api)

    ament_package()
endif()
