
if (WIN32)
    SET(PLATFORM_SUPPORT win32.cpp)

else()
    SET(PLATFORM_SUPPORT linux.cpp)
    SET(PLATFORM_LIBRARIES dl)
endif()

 

add_library(platform STATIC
    ${PLATFORM_SUPPORT})
target_link_libraries(platform ${PLATFORM_LIBRARIES})
add_dependencies(platform JDK)

set_property(TARGET platform PROPERTY POSITION_INDEPENDENT_CODE ON)
