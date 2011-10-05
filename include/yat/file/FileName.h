//----------------------------------------------------------------------------
// YAT LIBRARY
//----------------------------------------------------------------------------
//
// Copyright (C) 2006-2011  The Tango Community
//
// Part of the code comes from the ACE Framework (i386 asm bytes swaping code)
// see http://www.cs.wustl.edu/~schmidt/ACE.html for more about ACE
//
// The thread native implementation has been initially inspired by omniThread
// - the threading support library that comes with omniORB. 
// see http://omniorb.sourceforge.net/ for more about omniORB.
//
// Contributors form the TANGO community:
// Ramon Sune (ALBA) for the yat::Signal class 
//
// The YAT library is free software; you can redistribute it and/or modify it 
// under the terms of the GNU General Public License as published by the Free 
// Software Foundation; either version 2 of the License, or (at your option) 
// any later version.
//
// The YAT library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
// Public License for more details.
//
// See COPYING file for license details 
//
// Contact:
//      Nicolas Leclercq
//      Synchrotron SOLEIL
//------------------------------------------------------------------------------
/*!
 * \author S.Poirier - Synchrotron SOLEIL
 */

#ifndef __YAT_FILE_NAME_H__
#define __YAT_FILE_NAME_H__

#include <yat/CommonHeader.h>
#include <yat/utils/String.h>
#include <yat/memory/SharedPtr.h>
#include <yat/memory/MemBuf.h>
#include <yat/time/Time.h>

#if defined(_MSC_VER)
  #include <sys\utime.h>
#else
  #include <utime.h>
#endif

#include <sys/types.h>

#if defined YAT_WIN32 && !defined(_CSTDIO_)
  #include <cstdio>
#endif

#ifdef YAT_LINUX
	#include <dirent.h>
  #include <unistd.h>
  #include <sys/stat.h>
  #include <errno.h>
  #include <sys/vfs.h>
#elif defined YAT_WIN32
  #include <sys\stat.h>
  #include <ctype.h>
  #include <direct.h>
  #include <errno.h>
  #include <windows.h>
#elif defined YAT_MACOSX 
  #include <errno.h>
  #include <sys/stat.h>
  #include <sys/param.h>
  #include <sys/mount.h>
#endif

#include <map>

namespace yat
{

#define SEP_PATHDOS     '\\'
#define SEP_PATHUNIX    '/'
#ifdef YAT_WIN32
  #define SEP_PATH      SEP_PATHDOS
#else
  #define SEP_PATH      SEP_PATHUNIX
#endif
#define SEP_EXT         '.'
#define SEP_DSK         ':'

#ifndef _MAX_PATH
  #define _MAX_PATH 260
#endif


// Error msgs
const char ERR_CANNOT_CREATE_FOLDER[]     = "Cannot create directory '%s'";
const char ERR_CANNOT_CREATE_LINK[]       = "Cannot create link; name = '%s', target='%s'";
const char ERR_CANNOT_ENUM_DIR[]          = "Cannot enumerate directory '%s'";
const char ERR_CANNOT_REMOVE_FILE[]       = "Cannot remove file '%s'";
const char ERR_CANNOT_FETCH_INFO[]        = "Cannot fetch informations for file '%s'";
const char ERR_CANNOT_RENAME_FILE[]       = "Cannot rename file '%s'";
const char ERR_FILE_NOT_FOUND[]           = "File '%s' not found";
const char ERR_DIR_NOT_FOUND[]            = "Directory '%s' not found";
const char ERR_COPY_FAILED[]              = "A error has occured while copying from '%s' to '%s'";
const char ERR_OPEN_FILE[]                = "Cannot open file '%s'";
const char ERR_CANNOT_CREATE_WIN32[]      = "Cannot get handle for '%s' (Win32 API)";
const char ERR_CANNOT_CHANGE_FILE_TIME[]  = "Cannot change file time for '%s'";
const char ERR_CANNOT_GET_FILE_TIME[]     = "Cannot get file time for '%s'";
const char ERR_READING_FILE[]             = "Error while reading file '%s'";
const char ERR_WRITING_FILE[]             = "Error while writing file '%s'";
const char ERR_STAT_FAILED[]              = "Cannot get informations about file '%s'";
const char ERR_CHMOD_FAILED[]             = "Cannot change access for '%s' to '%o'";
const char ERR_CHOWN_FAILED[]             = "Cannot change owner for '%s' to %d:%d";
const char ERR_FSTYPE[]                   = "Error gathering file system information on '%s'";
const char ERR_NOT_A_DIRECTORY[]          = "Is not a directory";
const char ERR_DELETE_DIRECTORY[]         = "Cannot delete directory '%s'";
const char ERR_BAD_DEST_PATH[]            = "Bad destination path '%s'";
const char ERR_TEST_LINK[]                = "Cannot check entry '%s'";

// Begining of cygwin absolute file names
const char FILE_CYGDRIVE[] = "\\cygdrive\\";


//=============================================================================
// Free functions
//=============================================================================


/// Make symbolic link
///
/// @param strFileName opbject to link
int MakeLink(const String &strLink, const String &strTarget);

//=============================================================================
///
/// Exceptions
///
//=============================================================================
class YAT_DECL IOException : public Exception
{
public:
  IOException(const char *pcszDesc, const char *pcszOrigin):
  yat::Exception("IO_ERROR", pcszDesc, pcszOrigin)
  {  }
};

//=============================================================================
///
/// Bad path specification
///
//=============================================================================
class YAT_DECL BadPathException : public Exception
{
public:
  BadPathException(const char *pcszDesc, const char *pcszOrigin):
  yat::Exception("BAD_PATH", pcszDesc, pcszOrigin)
  { }
};

//=============================================================================
///
/// Current conditions doesn't allow to perform the action
///
//=============================================================================
class YAT_DECL BadPathConditionException : public Exception
{
public:
  BadPathConditionException(const char *pcszDesc, const char *pcszOrigin):
  yat::Exception("BAD_PATH_CONDITION", pcszDesc, pcszOrigin)
  { }
};

//=============================================================================
///
/// Path or file not found
///
//=============================================================================
class YAT_DECL FileNotFoundException : public Exception
{
public:
  FileNotFoundException(const char *pcszDesc, const char *pcszOrigin):
  yat::Exception("FILE_NOT_FOUND", pcszDesc, pcszOrigin)
  { }
};

//=============================================================================
///
/// Permission
///
//=============================================================================
class YAT_DECL PermissionException : public Exception
{
public:
  PermissionException(const char *pcszDesc, const char *pcszOrigin):
  yat::Exception("PERMISSION_ERROR", pcszDesc, pcszOrigin)
  { }
};

//=============================================================================
///
/// bad drive name
///
//=============================================================================
class YAT_DECL BadDriveException : public Exception
{
public:
  BadDriveException(const char *pcszDesc, const char *pcszOrigin):
  yat::Exception("BAD_DRIVE", pcszDesc, pcszOrigin)
  { }
};


//============================================================================
/// Free function
//============================================================================
inline int IsSepPath(char c)
 { return c == SEP_PATHDOS || c == SEP_PATHUNIX; }

//============================================================================
/// Classe FileName
///
/// File name manipulations
//============================================================================
class YAT_DECL FileName
{
protected:
  String m_strFile;  /// File name
public:
  
  // File system types
  enum FSType
  {
    //- Defaut value for Microsoft partition type
    MS    = 0,
    //- Microsoft partitions types
    FAT16 = 0x04,
    FAT32 = 0x0B,
    IFS   = 0x07,
    //- Unix file system types
    AFFS  = 0xADFF,
    EFS   = 0x00414A53,
    EXT   = 0x137D,
    EXT2  = 0xEF53,
    HPFS  = 0xF995E849,
    ISOFS = 0x9660,
    MSDOS = 0x4d44,
    NFS   = 0x6969,
    PROC  = 0x9fa0,
    SMB   = 0x517B,
    //- Apple filesystem types (TODO)
    HFS   = 0xffff
  };

private:

  void ThrowExceptionFromErrno(const char *pszError, const char *pszMethod) const;

public:
  /// Constructor
  FileName()  { }
  FileName(const String &strFileName)  { set(strFileName); }
  FileName(const String &strPath, const String &strName)  { set(strPath, strName); }

  /// Returns true if the filename is a path
  bool is_path_name() const;

  /// Returns true if the file name is a existing path
  bool path_exist() const;

  /// Returns true if file exists
  bool file_exist() const;

  /// Return true if filename is a empty directory
  bool is_empty_dir() const;
  
  /// Set file name
  /// Convert separators
  void set(const String &strFullName);
  void set(const String &strPath, const String &strName);
  void set(const String &strPath, const String &strName, const  String &strExt);

  void set_full_name(pcsz pszFileName);

  /// Convert separator according to the current operating system
  static void convert_separators(String *pstr);

  /// Get access mode from a string in the form "rwxrwxrwx"
  ///
  /// @param strAccess Input string
  static mode_t access_from_string(const String &strAccess);

  /// Return full name
  const String &full_name() const { return m_strFile; }

  /// Return path
  String path() const;

  /// Return filename without path and extension
  String name() const;

  /// Return directory name (last path component before file name)
  String dir_name() const;

  /// Return filename relative to specified path
  String rel_name(const char* pszPath) const;

  /// Return filename without path
  String name_ext() const;

  /// Return extension
  String ext() const;

  /// Create directory
  void mkdir(mode_t mode = 0, uid_t uid = (uid_t)-1, gid_t gid = (uid_t)-1) const 
    throw(Exception);

  /// Return true the file name is a existing link
  bool link_exist() const 
    throw(Exception);

  /// Make a symbolic link
  void make_sym_link(const String &strTarget, uid_t uid = (uid_t)-1, gid_t gid = (uid_t)-1) const
    throw(Exception);

  /// Remove file
  void remove() 
    throw(Exception);
  
  /// Remove directory
  void rmdir(bool bRecursive=false, bool bContentOnly=false) 
    throw(Exception);

  /// Copy a directory and its whole content inside the destination dir
  /// if bCreateDir is false the destination directory must already exists
  void dir_copy(const String &strDest, bool bCreateDir=false, mode_t modeDir = 0, uid_t uid = (uid_t)-1, gid_t gid = (uid_t)-1) 
    throw(Exception);

  /// Return size in octets
  uint32 size() const 
    throw(Exception);

  /// Return size in octets: 64bits version for files bigger than 2GB
  uint64 size64() const 
    throw(Exception);
  
  /// Rename the file
  void rename(const String &strNewName) 
    throw(Exception);
  
  /// Copy the file to the specified destination with given 
  void copy(const String &strDest, bool bKeepMetaData=false) 
    throw(Exception);
  
  /// Move file
  ///
  void move(const String &strDest) 
    throw(Exception);

  // Returns last modification date & time
  void mod_time(Time *pTm, bool bLocalTime=false) const 
    throw(Exception);

  // Sets last modification date & time
  void set_mod_time(const Time& tm) const 
    throw(Exception);

  /// Change file rights
  void chmod(mode_t mode) 
    throw(Exception);

  /// Recursively change rights for directory and files
  void recursive_chmod(mode_t modeFile, mode_t modeDir, bool bCurrentLevel=true) 
    throw(Exception);

  /// Recursively change directory rights
  void recursive_chmod_dir(mode_t mode) 
    throw(Exception);

  /// Recursively change file rights
  void recursive_chmod_file(mode_t mode) 
    throw(Exception);

  /// Change file owner
  void chown(uid_t uid, gid_t gid = (uid_t)-1) 
    throw(Exception);

  /// Recursively change file owner
  void recursive_chown(uid_t uid, gid_t gid = (uid_t)-1) 
    throw(Exception);

  // Returns filesystem type
  FSType file_system_type() const 
    throw(Exception);

  // Returns filesystem type
  fsid_t file_system_id() const 
    throw(Exception);
};

///===========================================================================
/// Classe File
///
/// Name for temporary file
///===========================================================================
class YAT_DECL File: public FileName
{
  public:
  File()
  { }
  File(const String &strFileName)
  { set(strFileName); }
  File(const String &strPath, const String &strName)
  { set(strPath, strName); }

  // Load file to the given MemBuf object
  void load(MemBuf *pMemBuf) 
    throw(Exception);

  // Load file to the given String
  void load(String *pString) 
    throw(Exception);

  // Save file from the given String
  void save(const String &strContent) 
    throw(Exception);
};


///===========================================================================
/// Classe TempFileName
///
/// Name for temporary file
///===========================================================================
class YAT_DECL TempFileName : public FileName
{
private:
  static long s_lLastNumber;

  // Generate file name
  String GenerateRandomName();

public:
  /// Constructors
  TempFileName();
  TempFileName(const String &strPath);
};  

///===========================================================================
/// Classe FileEnum
///
/// Enumerate all file of a given path
///===========================================================================
class YAT_DECL FileEnum : public FileName
{
public:
  enum EEnumMode
  {
     ENUM_FILE = 1,
     ENUM_DIR = 2,
     ENUM_ALL = 3
  };

protected: 

  EEnumMode m_eMode;  // Find mode (FILE or DIR)

#ifdef YAT_WIN32
  void* m_hFind;
  /// WIN32_FIND_DATA pointer
  void* m_pfindData;
  /// Indicate if a error occured since last find action
  int m_bErrorOnLastFind;
  bool m_bFirst;
#else
  DIR * m_dirDir;
#endif

  /// Path
  String m_strPath;    

public:
  ///-------------------------------------------------------------------------
  /// @group Constructeur/Destructeur
  ///-------------------------------------------------------------------------

  FileEnum(const String &strPath, EEnumMode eMode=ENUM_FILE) 
    throw(BadPathException);

  ~FileEnum();

  ///-------------------------------------------------------------------------
  /// @group Methodes standards
  ///-------------------------------------------------------------------------

  /// Initializing search mask
  /// 
  /// @param strPath Path to enumerate
  /// @param eMode enum mode: ENUM_FILE => files, ENUM_DIR => directories, ENUM_ALL => files & directories
  void init(const String &strPath, EEnumMode eMode=ENUM_FILE) 
    throw(BadPathException);

  /// Find next file
  /// @param pointer to a string to put the file name
  /// @return true if a file has been found, false if not
  bool find() 
    throw(BadPathException, FileNotFoundException, Exception);

  // Ferme enumeration
  void close();
};

///===========================================================================
/// class Cfgfile
/// Windows style configuration file
/// ex:
/// [section]
/// key=value
/// string
///
///===========================================================================
class YAT_DECL CfgFile : public File
{
public:
  typedef std::vector<String> Values;
  typedef std::map<String, String> Parameters;

private:
	class Section
	{
	public:
		Values m_vecSingleValues;
		Parameters m_dictParameters;
	};

	std::map<String, Section> m_dictSection;
	String m_strSection;

public:
	CfgFile(const String &strFile);
	void load() 
    throw(Exception);
	bool set_section(const String &strSection, bool bThrowException=true) 
    throw( Exception );
	const Values &get_values(const String &strSection=String::nil);
	const Parameters &get_parameters();
	bool has_parameter(const String &strParamName);
	String get_param_value(const String &strParamName);
};

#define FOR_EACH_VALUES_IN_CFG_SECTION(cfg_file, section, x) \
		cfg_file.SetSection(section); \
		for( CfgFile::Values::const_iterator x = cfg_file.GetValues().begin(); x != cfg_file.GetValues().end(); x++)

///===========================================================================
/// DirectoryWatcher
///
/// Watch changes in directory: new files/directories/links or removed files
///===========================================================================
class YAT_DECL DirectoryWatcher : public FileName
{
public:
  typedef yat::SharedPtr<FileName> FileNamePtr;
  typedef std::vector<FileNamePtr> FileNamePtrVector;

	typedef enum
	{
		ENUM_FIRST=0,	/// The directory content at object construction time is memorized
		NO_FIRST_ENUM   /// The directory content at object construction time is not memorized,
		                /// the first call to GetChanges will returns the whole directory content
	} WatchMode;
	
private:
	struct Entry
	{
		FileNamePtr ptrFile;
		Time tmLastModTime;
		bool bRemoved;
		
		Entry(const String& strFullName);
	};
	
	typedef yat::SharedPtr<Entry> EntryPtr;
	typedef std::map<uint64, EntryPtr> EntryMap;
	
	EntryMap  m_mapEntry;            // Directory content	
	Time      m_tmDirModTime;        // Last modification time of the directory
	Time      m_tmLocalModTime;      // Last time the the directory's modification time has changfes
	bool      m_bDirectoryHasChanged;
	FileName  m_fnDir;               // Directory path
	WatchMode m_eMode;
	
	bool priv_has_changed(bool bReset);
	
public:

	/// c-tor
	DirectoryWatcher(const String &strDirectoryPath, WatchMode eMode = ENUM_FIRST)
     throw( Exception);
	
	/// Get new, changed and removed file names
	void get_changes(FileNamePtrVector *vecNewFilesPtr, FileNamePtrVector *vecChangedFileNamePtr=NULL,
                    FileNamePtrVector *vecRemovedFileNamePtr=NULL) 
    throw( Exception);
	
	/// Has the directory changed ?
	bool has_changed();
};
		
}


#endif

