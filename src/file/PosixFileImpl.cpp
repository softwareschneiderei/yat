//----------------------------------------------------------------------------
// Copyright (c) 2004-2015 Synchrotron SOLEIL
// All rights reserved. This program and the accompanying materials
// are made available under the terms of the GNU Lesser Public License v3
// which accompanies this distribution, and is available at
// http://www.gnu.org/licenses/lgpl.html
//----------------------------------------------------------------------------
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
// Ramon Sune (ALBA) for the Signal class
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

//=============================================================================
// DEPENDENCIES
//=============================================================================
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <cstring>

#include <yat/md5/md5.h>
#include <yat/time/Time.h>
#include <yat/time/Timer.h>
#include <yat/file/FileName.h>
#include <yat/memory/DataBuffer.h>

namespace yat
{

///===========================================================================
/// FileName
///===========================================================================
size_t FileName::s_copy_bloc_size = 1048576;

//----------------------------------------------------------------------------
// FileName::access_from_string
//----------------------------------------------------------------------------
mode_t FileName::access_from_string(const std::string& strAccess)
{
  mode_t mode = 0;
  sscanf(PSZ(strAccess), "%o", &mode);
  return mode;
}

//-------------------------------------------------------------------
// FileName::path_exist()
//-------------------------------------------------------------------
bool FileName::path_exist() const
{
  pcsz pszPath = PSZ(full_name());
  if( strchr(pszPath, '*') || strchr(pszPath, '?') )
    // there are wildcard. this is not a valid path
    return false;

  uint32 uiLen = strlen(pszPath) ;
  if (uiLen == 0)
     return false;

  struct stat  st ;
  int          iRc ;

  if( uiLen>=2 && IsSepPath(pszPath[uiLen-1]) && pszPath[uiLen-2] != ':' )
  {
    // Path ends with '\' => remove '\'
    std::string strPath = pszPath;
    strPath = strPath.substr(0, strPath.size()-1);
    iRc = stat(PSZ(strPath), &st);
    if( iRc && errno != ENOENT )
      ThrowExceptionFromErrno(StringFormat(ERR_STAT_FAILED).format(strPath), "FileName::path_exist");
  }
  else
  {
    iRc = stat(pszPath, &st);
    if( iRc && errno != ENOENT )
      ThrowExceptionFromErrno(StringFormat(ERR_STAT_FAILED).format(pszPath), "FileName::path_exist");
  }
  return !iRc && (st.st_mode & S_IFDIR);
}

//-------------------------------------------------------------------
// FileName::file_exist
//-------------------------------------------------------------------
bool FileName::file_exist() const
{
  pcsz pcszfull_name = full_name().c_str();

  struct stat st;
  return (!access(pcszfull_name, F_OK) &&
        !stat(pcszfull_name, &st) &&
         (st.st_mode & S_IFREG));
}

//-------------------------------------------------------------------
// FileName::file_access
//-------------------------------------------------------------------
bool FileName::file_access() const
{
  return !access(PSZ(full_name()), F_OK);
}

//----------------------------------------------------------------------------
// FileName::set_full_name
//----------------------------------------------------------------------------
void FileName::set_full_name(pcsz pszFileName)
{
  if( !pszFileName || !pszFileName[0] )
  {
    m_strFile = StringUtil::empty;
    return;
  }

  std::string strFileName = pszFileName;

  // Convert separators
  convert_separators(&strFileName);

  if (IsSepPath(strFileName[0u]))
  {
    // Absolute name
    m_strFile = strFileName;
  }
  else
  {
    // relative name: add current working directory
    char cbuf[_MAX_PATH];
    getcwd(cbuf, _MAX_PATH);
    m_strFile = StringFormat("{}/{}").format(yat::String(cbuf)).format(strFileName);
  }

}

//-------------------------------------------------------------------
// FileName::rel_name
//-------------------------------------------------------------------
std::string FileName::rel_name(const char* pszPath) const
{
  FileName fnRef(pszPath);

  // Search for first separator. If not => return full name
  const char* p = strchr(m_strFile.c_str(), SEP_PATH);
  const char* pRef = strchr(fnRef.full_name().c_str(), SEP_PATH);
  if (!p || !pRef)
    return m_strFile;

  std::string str;
  bool bClimbStarted = false;
  for(;;)
  {
    const char* p1 = strchr(p+1, SEP_PATH);
    const char* pRef1 = strchr(pRef+1, SEP_PATH);

    if( !p1 )
    {
      // No more parts in file name
      while( pRef1 )
      {
        str = std::string("../") + str;
        pRef1 = strchr(pRef1+1, SEP_PATH);
      }
      str += std::string(p+1);
      return str;
    }

    if( !pRef1 )
    {
      // No more reference
      str += std::string(p+1);
      return str;
    }

    // Compare directories
    if( (p1-p != pRef1-pRef) || bClimbStarted ||
        // Unix : le case est important
        strncmp(p, pRef, p1-p) )
    {
      // Different directory
      str = std::string("../") + str;
      bClimbStarted = true;
      str.append(p+1, p1-p);
    }
    p = p1;
    pRef = pRef1;
  }
}

//----------------------------------------------------------------------------
// FileName::convert_separators
//----------------------------------------------------------------------------
void FileName::convert_separators(std::string *pstr)
{
  char *ptc = new char[pstr->length()+1];
  char *pStart = ptc;
  strcpy(ptc, PSZ(*pstr));


  // Convert from DOS to UNIX
  while (*ptc)
  {
    if (*ptc == SEP_PATHDOS)
      *ptc = SEP_PATHUNIX;
    ptc++;
  }

  *pstr = pStart;
  delete [] pStart;
}

//----------------------------------------------------------------------------
// FileName::mkdir
//----------------------------------------------------------------------------
void FileName::mkdir(mode_t mode, uid_t uid, gid_t gid) const
  throw( Exception )
{
  std::string str = path();
  if( str.empty() )
    return;

  char *p;
  p = ::strchr(const_cast<char*>(str.c_str()), SEP_PATH);

  if( !p )
  {
    std::string strErr = StringFormat(ERR_CANNOT_CREATE_FOLDER).format(str);
    throw BadPathException(PSZ(strErr), "FileName::mkdir");
  }
  p = strchr(p+1, SEP_PATH);
  if( !p )
  {
    // path = racine ; exist
    return;
  }

  do
  {
    *p = 0;
    struct stat st;
    if( ::stat(PSZ(str), &st) )
    {
      if( errno != ENOENT )
        // stat call report error != file not found
        ThrowExceptionFromErrno(StringFormat(ERR_STAT_FAILED).format(str), "FileName::mkdir");

      if( ::mkdir(PSZ(str), 0000777) )
      {
        // failure
        ThrowExceptionFromErrno(StringFormat(ERR_CANNOT_CREATE_FOLDER).format(str), "FileName::mkdir");
      }

      // Change access mode if needed
      if( mode != 0 )
      {
        if( ::chmod(PSZ(str), mode) )
        {
          // changing access mode failed
          ThrowExceptionFromErrno(StringFormat(ERR_CHMOD_FAILED).format(str).format(mode), "FileName::mkdir");
        }
      }
      // Change owner if needed
      if( (int)uid != -1 || (int)gid != -1 )
      {
        if( ::chown(PSZ(str), uid, gid) )
        {
          // changing owner mode failed
          ThrowExceptionFromErrno(StringFormat(ERR_CHOWN_FAILED).format(str).format(uid).format(gid), "FileName::mkdir");
        }
      }
    }
    else
    {
      if( !(st.st_mode & S_IFDIR) )
      {
        // c'est un fichier : erreur
        std::string strErr = StringFormat(ERR_CANNOT_CREATE_FOLDER).format(str);
        throw BadPathException(strErr, "FileName::mkdir");
      }
      // Directory : ok
    }
    // Next path component
    *p = SEP_PATH;
    p = strchr(p+1, SEP_PATH);
  } while( p );
}

//----------------------------------------------------------------------------
// FileName::link_exist
//----------------------------------------------------------------------------
bool FileName::link_exist() const throw( Exception )
{
  struct stat st;
  std::string strFullName = full_name();
  if( is_path_name() )
    strFullName.erase(strFullName.size()-1, 1);
  int iRc = lstat(PSZ(strFullName), &st);
  if( !iRc && S_ISLNK(st.st_mode) )
    return true;

/* Probably stupid... no file => no link!
  else if( iRc )
  {
    std::string strErr = StringUtil::str_format(ERR_TEST_LINK, PSZ(full_name()));
    ThrowExceptionFromErrno(PSZ(strErr), "FileName::link_exist");
  }
*/

  return false;
}

//----------------------------------------------------------------------------
// FileName::make_sym_link
//----------------------------------------------------------------------------
void FileName::make_sym_link(const std::string& strTarget, uid_t uid, gid_t gid) const throw( Exception )
{
  int iRc = symlink(PSZ(strTarget), PSZ(full_name()));
  if( iRc )
  {
    std::string strErr = StringFormat(ERR_CANNOT_CREATE_LINK).format(full_name()).format(strTarget);
    ThrowExceptionFromErrno(PSZ(strErr), "FileName::make_sym_link");
  }
  // Change owner if needed
  if( (int)uid != -1 || (int)gid != -1 )
  {
    if( lchown(PSZ(full_name()), uid, gid) )
    {
      // changing owner mode failed
      std::string strErr = StringFormat(ERR_CHOWN_FAILED).format(full_name()).format(uid).format(gid);
      ThrowExceptionFromErrno(strErr, "FileName::make_sym_link");
    }
  }
}

//----------------------------------------------------------------------------
// FileName::size
//----------------------------------------------------------------------------
uint32 FileName::size() const throw( Exception )
{
  struct stat sStat;
  if( stat(PSZ(full_name()), &sStat) == -1 )
  {
    std::string strErr = StringFormat(ERR_CANNOT_FETCH_INFO).format(m_strFile);
    ThrowExceptionFromErrno(PSZ(strErr), "FileName::size");
  }
  return sStat.st_size;
}

//----------------------------------------------------------------------------
// FileName::size64
//----------------------------------------------------------------------------
uint64 FileName::size64() const throw( Exception )
{
  struct stat64 sStat;
  if( stat64(PSZ(full_name()), &sStat) == -1 )
  {
    std::string strErr = StringFormat(ERR_CANNOT_FETCH_INFO).format(m_strFile);
    ThrowExceptionFromErrno(PSZ(strErr), "FileName::size");
  }
  return sStat.st_size;
}

//----------------------------------------------------------------------------
// FileName::mod_time
//----------------------------------------------------------------------------
void FileName::mod_time(Time *pTm, bool bLocalTime, bool stat_link) const throw( Exception )
{
  struct stat sStat;
  int rc = 0;
  if( stat_link )
    rc = lstat(PSZ(full_name()), &sStat);
  else
    rc = stat(PSZ(full_name()), &sStat);
  if( rc < 0 )
  {
    std::string strErr = StringFormat(ERR_CANNOT_GET_FILE_TIME).format(m_strFile);
    ThrowExceptionFromErrno(strErr, "FileName::mod_time");
  }

  if( bLocalTime )
  {
    struct tm tmLocal;
    localtime_r(&sStat.st_mtime, &tmLocal);
    pTm->set_long_unix(mktime(&tmLocal) + tmLocal.tm_gmtoff);
  }
  else
    pTm->set_long_unix(sStat.st_mtime);
}

//----------------------------------------------------------------------------
// FileName::set_mod_time
//----------------------------------------------------------------------------
void FileName::set_mod_time(const Time& tm) const throw( Exception )
{
  struct utimbuf sTm;
  struct stat sStat;

  if( stat(PSZ(full_name()), &sStat) != -1 )
    // Get access time, in order to preserve it
    sTm.actime = sStat.st_atime;
  else
    // stat function failed, use the new mod time
    sTm.actime = tm.long_unix();

  sTm.modtime = tm.long_unix();
  if( utime(PSZ(full_name()), &sTm) )
  {
    std::string strErr = StringFormat(ERR_CANNOT_CHANGE_FILE_TIME).format(m_strFile);
    ThrowExceptionFromErrno(PSZ(strErr), "FileName::set_mod_time");
  }
}

//-------------------------------------------------------------------
// FileName::priv_copy
//-------------------------------------------------------------------
void FileName::priv_copy(const std::string& strDst, yat::String* md5sum_p, bool keep_metadata)
{
  if( !file_exist() )
  { // File doesn't exists
    std::string strErr = StringFormat(ERR_FILE_NOT_FOUND).format(m_strFile);
    throw FileNotFoundException(strErr, "FileName::copy");
  }

  FileName fDst(strDst);
  if( fDst.is_path_name() )
    // Take source name
    fDst.set(fDst.path(), name_ext());

  // Self copy ?
  if( m_strFile == fDst.full_name() )
  {
    std::string strErr = StringFormat(ERR_COPY_FAILED).format(full_name()).format(fDst.full_name());
    throw Exception("FILE_ERROR", strErr, "FileName::copy");
  }

  struct stat st;
  int iRc = stat(PSZ(full_name()), &st);
  if( iRc )
  {
    std::string strErr = StringFormat(ERR_COPY_FAILED).format(full_name()).format(fDst.full_name());
    ThrowExceptionFromErrno(strErr, "FileName::copy");
  }

  yat::MD5 md5_processor;

  // Open source file
  int fsrc = open(PSZ(full_name()), O_RDONLY);
  if( fsrc < 0 )
  {
    std::string strErr = StringFormat(ERR_OPEN_FILE).format(full_name());
    ThrowExceptionFromErrno(strErr, "FileName::copy");
  }

  // Buffer
  char aBuf[s_copy_bloc_size];
  ::memset(aBuf, 0, s_copy_bloc_size);

  // Get last modified time
  Time tmLastMod;
  mod_time(&tmLastMod);

  // Opens destination file
  int fdst = creat(PSZ(fDst.full_name()), st.st_mode);
  if( fdst < 0 )
  {
    std::string strErr = StringFormat(ERR_OPEN_FILE).format(fDst.full_name());
    ThrowExceptionFromErrno(strErr, "FileName::copy");
  }

  try
  {
    // Copy by blocs
    int64 llTotalSize = size64();
    int64 llSize = llTotalSize;
    ssize_t lReaded=0, lWritten=0;
    ssize_t lToRead = 0;

    // progression init
    yat::Timer tm;
    std::size_t last_elapsed_sec = 1; // Wait at least one second before sending 1st progress notification

    if( m_progress_target_p )
      m_progress_target_p->on_start(name_ext(), llTotalSize);

    while( llSize )
    {
      lToRead = s_copy_bloc_size;

      if( llSize < s_copy_bloc_size )
        lToRead = (ssize_t)llSize;

      lReaded = read(fsrc, aBuf, lToRead);
      if( lReaded < 0 )
      {
        std::string strErr = StringFormat(ERR_READING_FILE).format(full_name());
        ThrowExceptionFromErrno(strErr, "FileName::copy");
      }

      if( md5sum_p )
        md5_processor.process(aBuf, lReaded);

      lWritten = write(fdst, aBuf, lToRead);
      if( lWritten < 0 || lWritten != lToRead )
      {
        std::string strErr = StringFormat(ERR_WRITING_FILE).format(fDst.full_name());
        ThrowExceptionFromErrno(strErr, "FileName::copy");
      }

      llSize -= lWritten;

      // progression
      if( m_progress_target_p )
      {
        std::size_t elapsed_sec = std::size_t(tm.elapsed_sec());
        if( elapsed_sec > last_elapsed_sec )
        {
          if( !m_progress_target_p->on_progress(name_ext(), llTotalSize, llTotalSize - llSize, tm.elapsed_sec()) )
          {
            // Operation canceled
            break;
          }
          last_elapsed_sec = elapsed_sec;
        }
      }
    }

    if( md5sum_p )
    {
      md5_processor.finish();
      char str[MD5_STRING_SIZE];
      std::memset(str, 0, MD5_STRING_SIZE);
      md5_processor.get_string(str);
      md5sum_p->clear();
      md5sum_p->append(str);
    }

    // progress notification: operation completed
    if( m_progress_target_p && 0 == llSize )
    {
      double elapsed = tm.elapsed_sec();
      m_progress_target_p->on_progress(name_ext(), llTotalSize, llTotalSize, elapsed);
      m_progress_target_p->on_complete(name_ext(), llTotalSize, elapsed);
    }

    close(fsrc);
    close(fdst);

    if( llSize > 0 )
    {
      // The copy was canceled
      fDst.remove();
      return;
    }

    // Copy last modifitation date
    fDst.set_mod_time(tmLastMod);
  }
  catch( yat::Exception &ex )
  {
    close(fsrc);
    close(fdst);
    throw ex;
  }

  // if root set original ownership
  if( keep_metadata && 0 == geteuid() )
  {
    try
    {
      fDst.chown(st.st_uid, st.st_gid);
    }

    catch( ... )
    {
      // Don't care, we did our best effort...
    }
  }
}

//-------------------------------------------------------------------
// FileName::move
//-------------------------------------------------------------------
void FileName::move(const std::string& strDest) throw( Exception )
{
  if( !file_exist() )
  { // File doesn't exists
    std::string strErr = StringFormat(ERR_FILE_NOT_FOUND).format(m_strFile);
    throw FileNotFoundException(strErr, "FileName::move");
  }

  FileName fDst(strDest);
  if( fDst.is_path_name() )
    // Take source name
    fDst.set(fDst.path(), name_ext());

  // Remove destination
  if( fDst.file_exist() )
    fDst.remove();

  // Check filesystem id, if it's the same, we can try to rename file
  fsid_t idSrc = file_system_id();
  fsid_t idDst = fDst.file_system_id();
  if( idSrc.__val[0] == idDst.__val[0] && idSrc.__val[1] == idDst.__val[1] )
  {
    try
    {
      rename(fDst.full_name());
    }
    catch( Exception e )
    {
      // Unable to rename => make a copy
      copy(fDst.full_name(), true);

      // Deletes source file and changes name
      remove();
      set(fDst.full_name());
    }
  }
  else
  {
    copy(fDst.full_name(), true);

    // Deletes source file and changes name
    remove();
    set(fDst.full_name());
  }
}

//-------------------------------------------------------------------
// FileName::file_system_type
//-------------------------------------------------------------------
FileName::FSType FileName::file_system_type() const throw( Exception )
{
  struct statfs buf;
  int iRc = statfs(PSZ(path()), &buf);
  if( iRc )
  {
    std::string strErr = StringFormat(ERR_FSTYPE).format(path());
    ThrowExceptionFromErrno(PSZ(strErr), "FileName::file_system_type");
  }
  return FSType(buf.f_type);
}

//-------------------------------------------------------------------
// FileName::file_system_id
//-------------------------------------------------------------------
fsid_t FileName::file_system_id() const throw( Exception )
{
  struct statfs buf;
  int iRc = statfs(PSZ(path()), &buf);
  if( iRc )
  {
    std::string strErr = StringFormat(ERR_FSTYPE).format(path());
    ThrowExceptionFromErrno(PSZ(strErr), "FileName::file_system_type");
  }
  return buf.f_fsid;
}

//-------------------------------------------------------------------
// FileName::file_system_statistics
//-------------------------------------------------------------------
FileName::FSStat FileName::file_system_statistics() const
{
  struct statvfs buf;
  int iRc = statvfs(PSZ(path()), &buf);
  if( iRc )
  {
    std::string strErr = StringFormat(ERR_FSTYPE).format(path());
    ThrowExceptionFromErrno(strErr, "FileName::file_system_type");
  }

  FSStat stats;
  stats.size.bytes = (yat::uint64)buf.f_blocks * (yat::uint64)buf.f_frsize;
  stats.avail.bytes = (yat::uint64)buf.f_bavail * (yat::uint64)buf.f_frsize;
  stats.used.bytes = stats.size - stats.avail;
  stats.avail_percent = 100.0 * double(stats.avail) / double(stats.size);
  stats.used_percent = 100.0 - stats.avail_percent;

  return stats;
}

//-------------------------------------------------------------------
// FileName::info
//-------------------------------------------------------------------
void FileName::info( Info* info_p, bool follow_link ) const
{
  struct stat64 st;
  int rc = 0;

  if( follow_link )
    rc = stat64( PSZ(full_name()), &st );
  else
    rc = lstat64( PSZ(full_name()), &st );
  if( rc )
  {
    info_p->is_exist = false;
  }
  else
  {
    info_p->is_exist  = true;
    info_p->size      = st.st_size;
    info_p->mode      = st.st_mode;
    info_p->is_file   = (st.st_mode & S_IFREG) == S_IFREG;
    info_p->is_dir    = (st.st_mode & S_IFDIR) == S_IFDIR;
    info_p->is_link   = (st.st_mode & S_IFLNK) == S_IFLNK;
    info_p->uid       = st.st_uid;
    info_p->gid       = st.st_gid;
    mod_time( &(info_p->mod_time), true, !follow_link );
  }

  if( info_p->is_link )
  {
    yat::Buffer<char> buf(1024);
    long s = readlink( PSZ(full_name()), buf.base(), 1024 );
    if( s > 0 )
      info_p->link_to.append( buf.base(), s );
  }
}

//-------------------------------------------------------------------
// FileName::chmod
//-------------------------------------------------------------------
void FileName::chmod(mode_t mode) throw( Exception )
{
  int iRc = ::chmod(PSZ(full_name()), mode);
  if( iRc )
  {
    std::string strErr = StringFormat(ERR_CHMOD_FAILED).format(full_name()).format(mode);
    ThrowExceptionFromErrno(PSZ(strErr), "FileName::chmod");
  }
}

//-------------------------------------------------------------------
// FileName::chown
//-------------------------------------------------------------------
void FileName::chown(uid_t uid, gid_t gid) throw( Exception )
{
  int iRc = ::chown(PSZ(full_name()), uid, gid);
  if( iRc )
  {
    std::string strErr = StringFormat(ERR_CHOWN_FAILED).format(full_name()).format(uid).format(gid);
    ThrowExceptionFromErrno(PSZ(strErr), "FileName::chown");
  }
}

//-------------------------------------------------------------------
// FileName::ThrowExceptionFromErrno
//-------------------------------------------------------------------
void FileName::ThrowExceptionFromErrno(const String& desc, const String& origin)
{
  std::string strDesc = StringFormat("{}. [Errno {}] {}").format(desc).format(int(errno)).format(static_cast<const char*>(strerror(errno)));
  switch( errno )
  {
    case EIO:
    case EDEADLK:
    case EFAULT:
    case EINTR:
    case EINVAL:
    case ENOLCK:
      throw IOException(strDesc, origin);
    case EPERM:
    case EACCES:
    case EROFS:
      throw PermissionException(strDesc, origin);
    case ENOTEMPTY:
      throw BadPathConditionException(strDesc, origin);
    case ENAMETOOLONG:
    case ELOOP:
    case EISDIR:
    case ENOTDIR:
    case EBADF:
      throw BadPathException(strDesc, origin);
    case ENOENT:
      throw FileNotFoundException(strDesc, origin);
    default:
      throw Exception(String("FILE_ERROR"), strDesc, origin);
  }
}

//-------------------------------------------------------------------
// FileName::set_copy_bloc_size
//-------------------------------------------------------------------
void FileName::set_copy_bloc_size(size_t size)
{
  s_copy_bloc_size = size;
}


//-------------------------------------------------------------------
// FileName::md5sum
//-------------------------------------------------------------------
yat::String FileName::md5sum() const
{
  int64 llTotalSize = size64();
  int64 llSize = llTotalSize;
  ssize_t lReaded = 0, lToRead = 0;

  yat::md5::md5_t md5_processor;

  // Open source file
  int fsrc = open(full_name().c_str(), O_RDONLY);
  if( fsrc < 0 )
  {
    std::string strErr = StringFormat(ERR_OPEN_FILE).format(full_name());
    ThrowExceptionFromErrno(strErr, "FileName::copy");
  }

  // Buffer
  char aBuf[s_copy_bloc_size];
  ::memset(aBuf, 0, s_copy_bloc_size);

  while( llSize )
  {
    lToRead = s_copy_bloc_size;

    if( llSize < s_copy_bloc_size )
      lToRead = (ssize_t)llSize;

    lReaded = read(fsrc, aBuf, lToRead);
    if( lReaded < 0 )
    {
      std::string strErr = StringFormat(ERR_READING_FILE).format(full_name());
      ThrowExceptionFromErrno(PSZ(strErr), "FileName::copy");
    }

    md5_processor.process(aBuf, lReaded);
    llSize -= lReaded;
  }

  md5_processor.finish();
  char str[MD5_STRING_SIZE];
  std::memset(str, 0, MD5_STRING_SIZE);
  md5_processor.get_string(str);
  return yat::String(str);
}

//===========================================================================
// Class FileEnum
//===========================================================================

//-------------------------------------------------------------------
// Initialisation
//-------------------------------------------------------------------
FileEnum::FileEnum(const std::string& strPath, EEnumMode eMode) throw(BadPathException)
{
  m_dirDir = NULL;
  init(strPath, eMode);
}

//-------------------------------------------------------------------
// Destructeur
//-------------------------------------------------------------------
FileEnum::~FileEnum()
{
  close();
}

//-------------------------------------------------------------------
// FileEnum::init
//-------------------------------------------------------------------
void FileEnum::init(const std::string& strPath, EEnumMode eMode) throw(BadPathException)
{
  close();
  m_eMode = eMode;
  set(PSZ(strPath));

  // Initialize enumeration
  m_dirDir = opendir(path().c_str());
  if( NULL == m_dirDir )
  {
    std::string strErr = StringFormat(ERR_CANNOT_ENUM_DIR).format(path());
    throw BadPathException(PSZ(strErr), "FileEnum::Init");
  }

  m_strPath = strPath; // Save initial path.

  // translate win separator to unix superator
  StringUtil::replace(&m_strPath, SEP_PATHDOS, SEP_PATHUNIX);
}

//-------------------------------------------------------------------
// FileEnum::find
//-------------------------------------------------------------------
bool FileEnum::find() throw(BadPathException, FileNotFoundException, Exception)
{
  struct dirent *dirEntry;
  std::string str;
  while( (dirEntry = readdir(m_dirDir)) != NULL )
  {
    str = dirEntry->d_name;
    if( StringUtil::is_equal( str, "." ) == false && StringUtil::is_equal( str, ".." ) == false )
    {
      // Set new file name
      set(m_strPath, dirEntry->d_name);

      // Check file
      if( (m_eMode & ENUM_FILE) && file_exist() )
        return true;
      if( (m_eMode & ENUM_DIR) && path_exist() )
        return true;
    }
  }

  // Not found
  return false;
}

//-------------------------------------------------------------------
// FileEnum::close
//-------------------------------------------------------------------
void FileEnum::close()
{
  if(m_dirDir)
    closedir(m_dirDir);
}

//===========================================================================
// Class TempFileName
//===========================================================================

//-------------------------------------------------------------------
// TempFileName::TempFileName
//-------------------------------------------------------------------
TempFileName::TempFileName()
{
  set("/tmp", GenerateRandomName());
}

//===========================================================================
// Class LockFile
//===========================================================================

//-------------------------------------------------------------------
// LockFile::priv_lock
//-------------------------------------------------------------------
bool LockFile::priv_lock( int lock_cmd )
{
  int rc = -1;
  struct flock fl;
  memset (&fl, 0, sizeof(fl));

  switch( m_type )
  {
    case READ:
    {
      m_fd = open( m_file_name.full_name().c_str(), O_RDONLY );
      if( m_fd < 0 )
      {
        std::string strErr = StringFormat(ERR_OPEN_FILE).format(m_file_name.full_name());
        FileName::ThrowExceptionFromErrno(PSZ(strErr), "LockFile::priv_lock");
      }
      fl.l_type = F_RDLCK;
      rc = fcntl( m_fd, lock_cmd, &fl );
    }
    break;

    case WRITE:
    {
      m_fd = open( m_file_name.full_name().c_str(), O_WRONLY );
      if( m_fd < 0 )
      {
        std::string strErr = StringFormat(ERR_OPEN_FILE).format(m_file_name.full_name());
        FileName::ThrowExceptionFromErrno(PSZ(strErr), "LockFile::priv_lock");
      }
      fl.l_type = F_WRLCK;
      rc = fcntl( m_fd, lock_cmd, &fl );
    }
    break;

    default:
      throw Exception("BAD_ARGUMENT", "Unknown lock type", "FileName::priv_lock");
  }

  if( rc < 0 && (EACCES == errno || EAGAIN == errno) )
  {
    // Already locked
    return false;
  }
  else if( rc < 0 )
  {
    std::string strErr = StringFormat("Locking file {} failed").format(m_file_name.full_name());
    FileName::ThrowExceptionFromErrno(PSZ(strErr), "LockFile::priv_lock");
  }

  m_lock_cmd = lock_cmd;
  return true;
}

//-------------------------------------------------------------------
// LockFile::try_lock
//-------------------------------------------------------------------
bool LockFile::try_lock()
{
  return priv_lock( F_SETLK );
}

//-------------------------------------------------------------------
// LockFile::lock
//-------------------------------------------------------------------
void LockFile::lock()
{
  priv_lock( F_SETLKW ); // Ignore returned value
}

//-------------------------------------------------------------------
// LockFile::unlock
//-------------------------------------------------------------------
void LockFile::unlock()
{
  struct flock fl;
  memset (&fl, 0, sizeof(fl));
  fl.l_type = F_UNLCK;
  int rc = fcntl( m_fd, m_lock_cmd, &fl );
  close ( m_fd );
  if( rc < 0 )
  {
    std::string strErr = StringFormat("Unlocking file {} failed").format(m_file_name.full_name());
    FileName::ThrowExceptionFromErrno(PSZ(strErr), "LockFile::unlock");
  }
}


} // namespace
