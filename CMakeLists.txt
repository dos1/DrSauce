cmake_minimum_required(VERSION 2.8)

project(DrSauce C)

add_definitions(-DLIBSUPERDERPY_MOUSE_EMULATION)

SET(LIBSUPERDERPY_GAMENAME "drsauce" CACHE INTERNAL "")
SET(LIBSUPERDERPY_GAMENAME_PRETTY "Dr. Sauce" CACHE INTERNAL "")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" "${CMAKE_SOURCE_DIR}/libsuperderpy/cmake")

include(libsuperderpy)
include(SetPaths)

add_subdirectory(libsuperderpy)
add_subdirectory(src)
add_subdirectory(data)

# uninstall target
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/libsuperderpy/cmake/cmake_uninstall.cmake.in" "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake" IMMEDIATE @ONLY)
add_custom_target(uninstall COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
