cmake_minimum_required(VERSION 3.10)
project(halodi-controller-simulation-api)

option(BUILD_PYTHON_BINDINGS "Build the Python bindings for the controller" ON)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")


set(CMAKE_BUILD_TYPE RelWithDebInfo)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    link_libraries(stdc++fs)
endif()

find_package(ament_cmake)

include(FindOrDownloadJDK)
include_directories(include ${JNI_INCLUDE_DIRS})

add_subdirectory(csrc)

install(
  DIRECTORY include/
  DESTINATION include
)

install(
  TARGETS halodi-controller-api jvmLauncher platform
  EXPORT halodi-controller-api
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin)

if(BUILD_PYTHON_BINDINGS)
  message("--Build and install the Python bindings...")
  add_subdirectory(pythonsrc)

  install(TARGETS halodi_controller_python
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin)

endif(BUILD_PYTHON_BINDINGS)

if(ament_cmake_FOUND)
    
    ament_export_include_directories(include)
    ament_export_libraries(halodi-controller-api)
    ament_export_targets(halodi-controller-api HAS_LIBRARY_TARGET)

    ament_package()
endif()
