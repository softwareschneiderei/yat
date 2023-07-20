set (source_files
      Version.cpp
      Exception.cpp
      bitsstream/BitsStream.cpp
      bitsstream/Endianness.cpp
      file/FileName.cpp
      memory/MemBuf.cpp
      network/Address.cpp
      network/ClientSocket.cpp
      network/Socket.cpp
      network/SocketException.cpp
      plugin/PlugIn.cpp
      plugin/PlugInUnix.cpp
      plugin/PlugInWin32.cpp
      plugin/PlugInBase.cpp
      plugin/PlugInManager.cpp
      plugin/CPlugIn.cpp
      plugin/CPlugInManager.cpp
      threading/Barrier.cpp
      threading/Message.cpp
      threading/MessageQ.cpp
      threading/Pulser.cpp
      threading/SharedObject.cpp
      threading/SyncAccess.cpp
      threading/Task.cpp
      threading/Utilities.cpp
      time/Time.cpp
      utils/CommandLine.cpp
      utils/Logging.cpp
      utils/String.cpp
      utils/StringTemplate.cpp
      utils/StringTokenizer.cpp
      utils/URI.cpp
      regex/Regex.cpp
      md5/md5.cpp
)

if (UNIX)
	list(APPEND source_files
		threading/PosixThreadingImpl.cpp
		plugin/PlugInUnix.cpp
		system/PosixSysUtilsImpl.cpp
		file/PosixFileImpl.cpp
		)
endif()

if (WIN32)
	list(APPEND source_files
		threading/WinNtThreadingImpl.cpp
		plugin/PlugInWin32.cpp
		system/Win32SysUtilsImpl.cpp
		file/Win32FileImpl.cpp
    regex/impl/gnuregex.c
		)
endif()

add_library (yat ${source_files})
target_include_directories(yat PUBLIC ${CMAKE_SOURCE_DIR}/include)

if (UNIX)
	target_link_libraries(yat pthread dl)
endif()

if (WIN32)
	target_link_libraries(yat ws2_32.lib shell32.lib)
endif()

if(MAJOR_VERSION)
  set_target_properties (yat PROPERTIES VERSION ${VERSION} SOVERSION ${MAJOR_VERSION})
endif()

if (UNIX)
  install (DIRECTORY ${CMAKE_SOURCE_DIR}/include/yat DESTINATION include
  	  FILES_MATCHING PATTERN "*.h"
  	  PATTERN "*.tpp"
  	  PATTERN "*.hpp"
  	  PATTERN "*.i"
  	  PATTERN ".svn" EXCLUDE
  	  PATTERN "CMakeFiles" EXCLUDE
  	  PATTERN "config-macosx.h" EXCLUDE
  	  PATTERN "config-win32.h" EXCLUDE
  	  PATTERN "WinNt*" EXCLUDE
  )
endif()

if (WIN32)
  install (DIRECTORY ${CMAKE_SOURCE_DIR}/include/yat DESTINATION include
      FILES_MATCHING PATTERN "*.h"
      PATTERN "*.tpp"
      PATTERN "*.hpp"
      PATTERN "*.i"
      PATTERN ".svn" EXCLUDE
      PATTERN "CMakeFiles" EXCLUDE
      PATTERN "config-macosx.h" EXCLUDE
      PATTERN "config-linux.h" EXCLUDE
  )
endif()

install(TARGETS yat
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)
