project (yat)
cmake_minimum_required (VERSION 2.6)

set(CMAKE_VERBOSE_MAKEFILE TRUE)
set(VERSION "1.7.17")

# check for 64 bit
if (CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(LIB_INSTALL_DIR "lib64")
else()
  set(LIB_INSTALL_DIR "lib")
endif()

if (UNIX)
  add_definitions(-std=c++0x -Wall -Wextra)
endif()

add_subdirectory (src)

set(prefix ${CMAKE_INSTALL_PREFIX})
if (UNIX)
  configure_file("${CMAKE_CURRENT_SOURCE_DIR}/yat.pc.in"
  "${CMAKE_CURRENT_BINARY_DIR}/yat.pc" @ONLY IMMEDIATE)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/yat.pc DESTINATION ${LIB_INSTALL_DIR}/pkgconfig)
endif()
