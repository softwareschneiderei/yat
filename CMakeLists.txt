cmake_minimum_required(VERSION 3.15)
project (yat)

option(BUILD_SHARED_LIBS "Build using shared libraries" ON)

add_compile_definitions(YAT_PROJECT_NAME=${PROJECT_NAME})
add_compile_definitions(YAT_PROJECT_VERSION=${PROJECT_VERSION})

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

set(prefix ${CMAKE_INSTALL_PREFIX})

if (UNIX AND EXISTS ${CMAKE_SOURCE_DIR}/yat.pc.in)
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/yat.pc.in"
    "${CMAKE_CURRENT_BINARY_DIR}/yat.pc" @ONLY IMMEDIATE)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/yat.pc DESTINATION ${LIB_INSTALL_DIR}/pkgconfig)
endif()

set(CPACK_PACKAGE_VERSION ${VERSION})
set(CPACK_PACKAGE_CONTACT "Mihael Koep <mihael.koep@softwareschneiderei.de>")
set(CPACK_GENERATOR DEB)
set(CPACK_DEBIAN_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
set(CPACK_DEBIAN_PACKAGE_RELEASE 1)

include(CPack)

