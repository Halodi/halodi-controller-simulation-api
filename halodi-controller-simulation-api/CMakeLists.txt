cmake_minimum_required(VERSION 3.10)
project(halodi-controller-simulation-api)


set(CMAKE_BUILD_TYPE RelWithDebInfo)

find_package(JNI REQUIRED)

find_package(ament_cmake)


include_directories(include ${JNI_INCLUDE_DIRS})
add_subdirectory(csrc)

install(
  DIRECTORY include/
  DESTINATION include
)


if(ament_cmake_FOUND)
message("FOUND AMENT")
    
    ament_export_include_directories(include)
    ament_export_libraries(halodi-controller-api)

    ament_package()
endif()