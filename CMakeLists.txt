cmake_minimum_required(VERSION 3.15)
project (yat)

option(BUILD_SHARED_LIBS "Build using shared libraries" ON)

add_definitions(-DYAT_PROJECT_NAME=${PROJECT_NAME})
add_definitions(-DYAT_PROJECT_VERSION=${VERSION})

if (UNIX)
    add_definitions(-std=c++0x)
    add_compile_definitions(_FILE_OFFSET_BITS=64)
    add_compile_definitions(YAT_HAS_GNUREGEX)
endif()

if (WIN32 AND BUILD_SHARED_LIBS MATCHES "ON")
    add_compile_definitions(YAT_DLL)
    add_compile_definitions(YAT_BUILD)
endif()

add_subdirectory (src)

if (UNIX AND EXISTS ${CMAKE_SOURCE_DIR}/yat.pc.in)
    configure_file("${CMAKE_SOURCE_DIR}/yat.pc.in"
    "${CMAKE_BINARY_DIR}/yat.pc" @ONLY IMMEDIATE)
    install(FILES ${CMAKE_BINARY_DIR}/yat.pc DESTINATION lib/pkgconfig)
endif()
