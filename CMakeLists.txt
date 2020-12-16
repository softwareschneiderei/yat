project (yat)
cmake_minimum_required (VERSION 2.6)

set(CMAKE_VERBOSE_MAKEFILE TRUE)

set(MAJOR_VERSION "1")
set(MINOR_VERSION "17")
set(PATCH_VERSION "6")
set(VERSION "${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION}")

# check for 64 bit
if (NOT DEFINED CMAKE_INSTALL_PREFIX AND CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(LIB_INSTALL_DIR "lib64")
else()
    set(LIB_INSTALL_DIR "lib")
endif()

if (UNIX)
    add_definitions(-O2 -g -gstabs+ -std=c++0x -Wall -Wno-unused-variable -D_FILE_OFFSET_BITS=64 -DYAT_PROJECT_VERSION=1.17.6 -DYAT_PROJECT_NAME=YAT)
    set(CMAKE_SHARED_LINKER_FLAGS "-Wl,-z,defs")
endif()

add_subdirectory (src)

set(prefix ${CMAKE_INSTALL_PREFIX})

if (UNIX)
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/yat.pc.in"
    "${CMAKE_CURRENT_BINARY_DIR}/yat.pc" @ONLY IMMEDIATE)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/yat.pc DESTINATION ${LIB_INSTALL_DIR}/pkgconfig)
endif()

