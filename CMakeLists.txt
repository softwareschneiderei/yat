project (yat)
cmake_minimum_required (VERSION 2.6)

set(CMAKE_VERBOSE_MAKEFILE TRUE)
set(VERSION "1.7.11")

if (lib_dir)
  set(LIB_INSTALL_DIR ${lib_dir})
else()
  set(LIB_INSTALL_DIR "lib${LIB_SUFFIX}") 
endif()  

if (UNIX)
	add_definitions(-ansi -Wall -Wextra -Werror)
endif()

add_subdirectory (src)
#add_subdirectory (test)

set(prefix ${CMAKE_INSTALL_PREFIX})
if (UNIX)
  configure_file("${CMAKE_CURRENT_SOURCE_DIR}/yat.pc.in"
  "${CMAKE_CURRENT_BINARY_DIR}/yat.pc" @ONLY IMMEDIATE)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/yat.pc DESTINATION ${LIB_INSTALL_DIR}/pkgconfig)
endif()
