option(CUSTOM_PYTHON_VERSION "The number of the custom Python version" OFF)
option(Python_ROOT_DIR "Root directory of this custom Python" /usr/bin/)

if(${CUSTOM_PYTHON_VERSION})
  message("--- Build the bindings against a custom Python version")
  message("--- Trying to find Python " ${CUSTOM_PYTHON_VERSION})

  find_package(Python
    ${CUSTOM_PYTHON_VERSION} EXACT REQUIRED
    COMPONENTS Interpreter Development)

  if(${Python_FOUND})
    message("--- Found custom Python " ${Python_VERSION})
    message("--- Python executable: " ${Python_EXECUTABLE})
    message("--- Python include dirs: " ${Python_INCLUDE_DIRS})
    message("--- Python libs: " ${Python_LIBRARIES})
  else(${Python_FOUND})
    message("--- Couldn't find custom Python " ${CUSTOM_PYTHON_VERSION})
  endif(${Python_FOUND})

endif(${CUSTOM_PYTHON_VERSION})

# set(Python_ROOT_DIR /home/max/.local/share/ov/pkg/isaac_sim-2021.2.0/kit/python)

add_subdirectory(pybind11)

pybind11_add_module(halodi_controller_python MODULE halodicontroller_binding.cpp)
target_link_libraries(halodi_controller_python PRIVATE halodi-controller-api)