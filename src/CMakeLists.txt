set (source_files
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
  plugin/PlugInManager.cpp
  threading/Barrier.cpp
  threading/Message.cpp
  threading/MessageQ.cpp
  threading/Pulser.cpp
  threading/SharedObject.cpp
  threading/SyncAccess.cpp
  threading/Task.cpp
  time/Time.cpp
  utils/Logging.cpp
  utils/String.cpp
  utils/StringTemplate.cpp
  utils/StringTokenizer.cpp
  utils/URI.cpp
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
		)
endif()

include_directories(
  ../include
)

add_library (yat SHARED ${source_files})

if (WIN32)
	target_link_libraries(yat ws2_32.lib shell32.lib)
endif()

install (DIRECTORY ../include/yat DESTINATION include/
  FILES_MATCHING PATTERN "*.h"
  PATTERN "*.tpp"
  PATTERN "*.i"
  PATTERN "CMakeFiles" EXCLUDE
  PATTERN "config-macosx.h" EXCLUDE
  PATTERN "config-win32.h" EXCLUDE
  PATTERN "WinNt*" EXCLUDE
)

if (UNIX)
	install (TARGETS yat LIBRARY DESTINATION ${LIB_INSTALL_DIR})
endif()
