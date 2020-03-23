add_library(jvmLauncher javavirtualmachine.cpp)
target_link_libraries(jvmLauncher ${JNI_LIBRARIES})

set_property(TARGET jvmLauncher PROPERTY INSTALL_RPATH_USE_LINK_PATH TRUE)


add_executable(jvm-launcher-demo demo.cpp)
target_link_libraries(jvm-launcher-demo jvmLauncher)


install(TARGETS jvmLauncher
  LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  )

