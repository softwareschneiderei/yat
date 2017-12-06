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
// Copyright (C) 2006-2016 The Tango Community
//
// Part of the code comes from the ACE Framework (asm bytes swaping code)
// see http://www.cs.wustl.edu/~schmidt/ACE.html for more about ACE
//
// The thread native implementation has been initially inspired by omniThread
// - the threading support library that comes with omniORB.
// see http://omniorb.sourceforge.net/ for more about omniORB.
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
 * \author See AUTHORS file
 */

//=============================================================================
// DEPENDENCIES
//=============================================================================
#include <yat/time/Time.h>
#include <yat/file/FileName.h>
#include <yat/utils/Logging.h>

#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

namespace yat
{

//===========================================================================
// FileName
// File name manipulations
//===========================================================================

//----------------------------------------------------------------------------
// FileName::set
//----------------------------------------------------------------------------
void FileName::set(const std::string& strFullName)
{
  set_full_name(PSZ(strFullName));

  if( path_exist() && !is_path_name() )
    // Add missing path separator
    m_strFile += SEP_PATH;
}

void FileName::set(const std::string& _strPath, const std::string& _strName)
{
  std::string strPath = _strPath;
  std::string strName = _strName;

  convert_separators(&strPath);
  convert_separators(&strName);

  std::string strFullName;

  if( StringUtil::end_with(strPath, SEP_PATH) )
    strFullName = strPath + strName;
  else
    strFullName = strPath + SEP_PATH + strName;
  set(strFullName);
}

void FileName::set(const std::string& strPath, const std::string& strName, const std::string& strExt)
{
  set(strPath, strName + "." + strExt);
}

//----------------------------------------------------------------------------
// FileName::join
//----------------------------------------------------------------------------
void FileName::join(const std::string& fragment)
{
  set(path() + fragment, name_ext());
}

//----------------------------------------------------------------------------
// FileName::is_path_name
//----------------------------------------------------------------------------
bool FileName::is_path_name() const
{
  if( StringUtil::end_with( m_strFile, SEP_PATH ) )
    return true;
  return false;
}

//----------------------------------------------------------------------------
// FileName::path
//----------------------------------------------------------------------------
std::string FileName::path() const
{
  std::string strPath;

  // Backward search for first separator
  std::string::size_type iLastSepPos = m_strFile.find_last_of(SEP_PATH);
  if( std::string::npos != iLastSepPos )
   // Found
   strPath = m_strFile.substr(0, iLastSepPos + 1);

  return strPath;
}

//----------------------------------------------------------------------------
// FileName::name
//----------------------------------------------------------------------------
std::string FileName::name() const
{
  std::string strName;

  // Backward search for first separator
  std::string::size_type iLastSepPos = m_strFile.find_last_of(SEP_PATH);
  if( std::string::npos == iLastSepPos )
    // If not found  : no path
    iLastSepPos = 0;

  std::string::size_type iExtPos = m_strFile.find_last_of(SEP_EXT);
  if( std::string::npos == iExtPos )
    iExtPos = m_strFile.length();

  strName = m_strFile.substr(iLastSepPos + 1, iExtPos - iLastSepPos - 1);

  return strName;
}

//----------------------------------------------------------------------------
// FileName::dir_name
//----------------------------------------------------------------------------
std::string FileName::dir_name() const
{
  std::string strName;

  // Backward search for last separator
  std::string::size_type iLastSepPos = m_strFile.find_last_of(SEP_PATH);
  if ( std::string::npos == iLastSepPos )
  {
    return "";
  }
  std::string::size_type iPreviousSepPos = m_strFile.rfind(SEP_PATH, iLastSepPos - 1);
  if ( std::string::npos == iPreviousSepPos ) {
    return m_strFile.substr(0, iLastSepPos);
  }
  return m_strFile.substr(iPreviousSepPos + 1, iLastSepPos - iPreviousSepPos - 1);
}

//----------------------------------------------------------------------------
// FileName::name_ext
//----------------------------------------------------------------------------
std::string FileName::name_ext() const
{
  std::string strFileName = name();
  if( ext().size() > 0 )
    strFileName += '.' + ext();
  return strFileName;
}

//----------------------------------------------------------------------------
// FileName::ext
//----------------------------------------------------------------------------
std::string FileName::ext() const
{
  std::string strExt;

  // Search backward for extension separator
  std::string::size_type iExtPos = m_strFile.find_last_of(SEP_EXT);
  // Backward search for last separator
  std::string::size_type iLastSepPos = m_strFile.find_last_of(SEP_PATH);
  if( std::string::npos != iExtPos && iExtPos > iLastSepPos )
    // Separator found
    strExt = m_strFile.substr(iExtPos + 1);

  return strExt;
}

//----------------------------------------------------------------------------
// FileName::remove
//----------------------------------------------------------------------------
void FileName::remove() throw( Exception )
{
  if( !m_strFile.empty() )
  {
    if( unlink(PSZ(full_name())) )
    {
      std::string strErr = StringUtil::str_format(ERR_CANNOT_REMOVE_FILE, PSZ(full_name()));
      ThrowExceptionFromErrno(PSZ(strErr), "FileName::remove");
    }
  }
}

//----------------------------------------------------------------------------
// FileName::rmdir
//----------------------------------------------------------------------------
void FileName::rmdir(bool bRecursive, bool bContentOnly) throw( Exception )
{
  if( !is_path_name() )
    throw BadPathException(PSZ(StringUtil::str_format(ERR_DELETE_DIRECTORY, PSZ(full_name()))), "FileName::rmdir");

  if( !m_strFile.empty() )
  {
    if( !bRecursive )
    {
      // Delete files & symbolic links
      FileEnum fileEnum(full_name(), FileEnum::ENUM_FILE);
      while( fileEnum.find() )
        fileEnum.remove();
      if( !bContentOnly )
      {
        if( ::rmdir(PSZ(full_name())) )
        {
          std::string strErr = StringUtil::str_format(ERR_CANNOT_REMOVE_FILE, PSZ(full_name()));
          ThrowExceptionFromErrno(PSZ(strErr), "FileName::rmdir");
        }
      }
    }
    else
    { // Recursively delete directory content
      FileEnum dirEnum(full_name(), FileEnum::ENUM_DIR);
      while( dirEnum.find() )
        dirEnum.rmdir(true);
      // Delete files & symbolic links
      FileEnum fileEnum(full_name(), FileEnum::ENUM_FILE);
      while( fileEnum.find() )
        fileEnum.remove();

      if( !bContentOnly )
      {
        // And finally the direcory himself
        rmdir();
      }
    }
  }
}

//----------------------------------------------------------------------------
// FileName::rename
//----------------------------------------------------------------------------
void FileName::rename(const std::string& strNewName) throw( Exception )
{
  if( !m_strFile.empty() )
    if( ::rename(PSZ(m_strFile), PSZ(strNewName)) )
    {
      std::string strErr = StringUtil::str_format(ERR_CANNOT_RENAME_FILE, PSZ(m_strFile));
      ThrowExceptionFromErrno(PSZ(strErr), "FileName::rename");
    }

  // Change internal name
  set(strNewName);
}

//-------------------------------------------------------------------
// FileName::dir_copy
//-------------------------------------------------------------------
void FileName::dir_copy(const std::string& strDest, bool bCreateDir, mode_t modeDir, uid_t uid, gid_t gid) throw( Exception )
{
  FileName fnDst;

  // Create destination path
  fnDst.set(strDest);
  fnDst.mkdir(modeDir, uid, gid);

  if( bCreateDir )
  {
    // Create source directory name inside destination path
    fnDst.set(strDest + dir_name() + SEP_PATH);
    fnDst.mkdir(modeDir, uid, gid);
  }

  if( !fnDst.is_path_name() )
    throw BadPathException(PSZ(StringUtil::str_format(ERR_BAD_DEST_PATH, PSZ(fnDst.full_name()))),
                           "FileName::dir_copy");

  // Recursively copying sub-directories
  FileEnum dirEnum(full_name(), FileEnum::ENUM_DIR);
  while( dirEnum.find() )
    dirEnum.dir_copy(fnDst.path(), true, modeDir, uid, gid);

  // Copying directory files
  FileEnum fileEnum(full_name(), FileEnum::ENUM_FILE);
  while( fileEnum.find() )
    // Copy with metadata
    fileEnum.copy(fnDst.path(), true);
}

//-------------------------------------------------------------------
// FileName::recursive_chmod
//-------------------------------------------------------------------
void FileName::recursive_chmod(mode_t modeFile, mode_t modeDir, bool bCurrentLevel) throw( Exception )
{
  if( !path_exist() )
  { // File doesn't exists
    std::string strErr = StringUtil::str_format(ERR_DIR_NOT_FOUND, PSZ(full_name()));
    throw FileNotFoundException(PSZ(strErr), "FileName::recursive_chmod");
  }

  // Recursively change rights on sub-directories
  FileEnum dirEnum(full_name(), FileEnum::ENUM_DIR);
  while( dirEnum.find() )
    dirEnum.recursive_chmod(modeFile, modeDir, true);

  // Change mode on files
  FileEnum fileEnum(full_name(), FileEnum::ENUM_FILE);
  while( fileEnum.find() )
    // Copy with metadata
    fileEnum.chmod(modeFile);

  if( bCurrentLevel )
    // Change mode to directory itself
    chmod(modeDir);
}

//-------------------------------------------------------------------
// FileName::recursive_chmod_file
//-------------------------------------------------------------------
void FileName::recursive_chmod_file(mode_t mode) throw( Exception )
{
  if( !path_exist() )
  { // File doesn't exists
    std::string strErr = StringUtil::str_format(ERR_DIR_NOT_FOUND, PSZ(full_name()));
    throw FileNotFoundException(PSZ(strErr), "FileName::recursive_chmod_file");
  }

  // Recursively change rights on sub-directories
  FileEnum dirEnum(full_name(), FileEnum::ENUM_DIR);
  while( dirEnum.find() )
    dirEnum.recursive_chmod_file(mode);

  // Change mode on files
  FileEnum fileEnum(full_name(), FileEnum::ENUM_FILE);
  while( fileEnum.find() )
    // Copy with metadata
    fileEnum.chmod(mode);
}

//-------------------------------------------------------------------
// FileName::recursive_chmod_dir
//-------------------------------------------------------------------
void FileName::recursive_chmod_dir(mode_t mode) throw( Exception )
{
  if( !path_exist() )
  { // File doesn't exists
    std::string strErr = StringUtil::str_format(ERR_DIR_NOT_FOUND, PSZ(full_name()));
    throw FileNotFoundException(PSZ(strErr), "FileName::recursive_chmod_dir");
  }

  // Recursively change rights on sub-directories
  FileEnum dirEnum(full_name(), FileEnum::ENUM_DIR);
  while( dirEnum.find() )
    dirEnum.recursive_chmod_dir(mode);

  // Change mode to directory itself
  chmod(mode);
}

//-------------------------------------------------------------------
// FileName::recursive_chown
//-------------------------------------------------------------------
void FileName::recursive_chown(uid_t uid, gid_t gid) throw( Exception )
{
  if( !path_exist() )
  { // File doesn't exists
    std::string strErr = StringUtil::str_format(ERR_DIR_NOT_FOUND, PSZ(full_name()));
    throw FileNotFoundException(PSZ(strErr), "FileName::recursive_chmod");
  }

  // Recursively change rights on sub-directories
  FileEnum dirEnum(full_name(), FileEnum::ENUM_DIR);
  while( dirEnum.find() )
    dirEnum.recursive_chmod(uid, gid);

  // Change mode on files
  FileEnum fileEnum(full_name(), FileEnum::ENUM_FILE);
  while( fileEnum.find() )
    // Copy with metadata
    fileEnum.chown(uid, gid);

  // Change mode to directory itself
  chown(uid, gid);
}

//-------------------------------------------------------------------
// FileName::is_empty_dir
//-------------------------------------------------------------------
bool FileName::is_empty_dir() const
{
  FileEnum dirEnum(full_name(), FileEnum::ENUM_DIR );
  FileEnum fileEnum(full_name(), FileEnum::ENUM_FILE);

  if( !dirEnum.find() && !fileEnum.find() )
  return true;
  return false;
}

// ============================================================================
// class FileName::FSStat
// ============================================================================
const char *_size_units_[] = {"", "K", "M", "G", "T", "P", "E", NULL};

//-------------------------------------------------------------------
// FileName::FSBytes::to
//-------------------------------------------------------------------
double FileName::FSBytes::to(Units u) const
{
  switch( u )
  {
    case BYTES:
      return double(bytes);
    case KBYTES:
      return (double)bytes / YAT_KBYTES;
    case MBYTES:
      return (double)bytes / YAT_MBYTES;
    case GBYTES:
      return (double)bytes / YAT_GBYTES;
    case TBYTES:
      return (double)bytes / YAT_TBYTES;
    case PBYTES:
      return (double)bytes / YAT_PBYTES;
    case EBYTES:
      return (double)bytes / YAT_EBYTES;
    default:
      throw yat::Exception("ERROR", "Bad conversion units", "yat::FileName::FSBytes::to");
  }
}

//-------------------------------------------------------------------
// FileName::FSBytes::from
//-------------------------------------------------------------------
void FileName::FSBytes::from( double value, Units u )
{
  switch( u )
  {
    case BYTES:
      bytes = yat::int64(value);
      break;
    case KBYTES:
      bytes = yat::int64(value * YAT_KBYTES);
      break;
    case MBYTES:
      bytes = yat::int64(value * YAT_MBYTES);
      break;
    case GBYTES:
      bytes = yat::int64(value * YAT_GBYTES);
      break;
    case TBYTES:
      bytes = yat::int64(value * YAT_TBYTES);
      break;
    case PBYTES:
      bytes = yat::int64(value * YAT_PBYTES);
      break;
    case EBYTES:
      bytes = yat::int64(value * YAT_EBYTES);
      break;
    default:
      throw yat::Exception("ERROR", "Bad conversion units", "yat::FileName::FSBytes::from");
  }
}

//-------------------------------------------------------------------
// FileName::FSBytes::from
//-------------------------------------------------------------------
void FileName::FSBytes::from( const std::string &value )
{
  std::string v = value;

  if( yat::StringUtil::match(v, "*k") || yat::StringUtil::match(v, "*kbytes") )
      from( atol(v.c_str()), KBYTES );
  else if( yat::StringUtil::match(v, "*m") || yat::StringUtil::match(v, "*mbytes") )
      from( atol(v.c_str()), MBYTES );
  else if( yat::StringUtil::match(v, "*g") || yat::StringUtil::match(v, "*gbytes") )
      from( atol(v.c_str()), GBYTES );
  else if( yat::StringUtil::match(v, "*t") || yat::StringUtil::match(v, "*tbytes") )
      from( atol(v.c_str()), TBYTES );
  else if( yat::StringUtil::match(v, "*p") || yat::StringUtil::match(v, "*pbytes") )
      from( atol(v.c_str()), PBYTES );
  else if( yat::StringUtil::match(v, "*e") || yat::StringUtil::match(v, "*ebytes") )
      from( atol(v.c_str()), EBYTES );
  else
    from( atol(v.c_str()) );
}

//-------------------------------------------------------------------
// FileName::FSBytes::to_string
//-------------------------------------------------------------------
std::string FileName::FSBytes::to_string(bool long_format) const
{
  int l = 0;
  double d = bytes;
  while( d > 1024.0 )
  {
    d /= 1024.0;
    ++l;
  }

  std::ostringstream oss;
  if( d > 10 )
    oss << int(d + 0.5) << _size_units_[l];
  else
    oss << int(d * 10 + 0.5) / 10.0 << _size_units_[l];

  if( long_format )
    oss << " bytes";

  return oss.str();
}

//===========================================================================
// Class TempFileName
//===========================================================================
long TempFileName::s_lLastNumber = 0;

//-------------------------------------------------------------------
// TempFileName::GenerateRandomName
//-------------------------------------------------------------------
std::string TempFileName::GenerateRandomName()
{
  if( !s_lLastNumber )
    // Initialize random sequence
    s_lLastNumber = CurrentTime().long_unix();

  return StringUtil::str_format("temp%lx", s_lLastNumber++);
}

//-------------------------------------------------------------------
// TempFileName::TempFileName(path)
//-------------------------------------------------------------------
TempFileName::TempFileName(const std::string& strPath)
{
  set(strPath, GenerateRandomName());
}

//===========================================================================
// Class File
//===========================================================================

//-------------------------------------------------------------------
// File::load(MemBuf)
//-------------------------------------------------------------------
void File::load(MemBuf *pBuf) throw(Exception)
{
  // Open source file
  FILE *fi = fopen(PSZ(full_name()), "rb");
  if( NULL == fi )
  {
    std::string strErr = StringUtil::str_format(ERR_OPEN_FILE, PSZ(full_name()));
    throw Exception("FILE_ERROR", PSZ(strErr), "File::Load");
  }

  // Buffer
  pBuf->set_len(size()+1);

  // Read
  std::size_t lSize = size();
  std::size_t lTotalReaded = 0;
  while( lTotalReaded < lSize )
  {
    std::size_t lReaded = fread(pBuf->buf() + lTotalReaded, 1, lSize, fi);

    if( ferror(fi) || 0 == lReaded )
    {
      std::string strErr = StringUtil::str_format(ERR_READING_FILE, PSZ(full_name()));
      fclose(fi);
      throw Exception("FILE_ERROR", PSZ(strErr), "File::load");
    }
    lTotalReaded += lReaded;
  }
  memset(pBuf->buf() + lSize, 0, 1);
  fclose(fi);
}

//-------------------------------------------------------------------
// File::load(String*)
//-------------------------------------------------------------------
void File::load(std::string *pString) throw(Exception)
{
  MemBuf buf;
  load(&buf);
  buf >> (*pString);
}

//-------------------------------------------------------------------
// File::save
//-------------------------------------------------------------------
void File::save(const std::string& strContent) throw(Exception)
{
  // Open destination file
  FILE *fi = fopen(PSZ(full_name()), "wb");
  if( NULL == fi )
  {
    std::string strErr = StringUtil::str_format(ERR_OPEN_FILE, PSZ(full_name()));
    throw Exception("FILE_ERROR", PSZ(strErr), "File::Load");
  }

  // Write text content
  int iRc = fputs(PSZ(strContent), fi);
  if( EOF == iRc )
  {
    std::string strErr;
    StringUtil::printf(&strErr, "Cannot write in file '%s'", PSZ(full_name()));
    fclose(fi);
    throw Exception("FILE_ERROR", PSZ(strErr), "File::save");
  }
  fclose(fi);
}

//-------------------------------------------------------------------
// File::append
//-------------------------------------------------------------------
void File::append(const std::string& content)
{
  // Open destination file
  FILE *fi = fopen(PSZ(full_name()), "a");
  if( NULL == fi )
  {
    std::string strErr = StringUtil::str_format(ERR_OPEN_FILE, PSZ(full_name()));
    throw Exception("FILE_ERROR", PSZ(strErr), "File::append");
  }

  // Write text content
  int iRc = fputs(content.c_str(), fi);
  if( EOF == iRc )
  {
    std::string strErr;
    StringUtil::printf(&strErr, "Cannot write in file '%s'", PSZ(full_name()));
    fclose(fi);
    throw Exception("FILE_ERROR", PSZ(strErr), "File::append");
  }
  fclose(fi);
}

//===========================================================================
// class Cfgfile
//===========================================================================

//-----------------------------------------------------------------------------
// CfgFile::CfgFile
//-----------------------------------------------------------------------------
CfgFile::CfgFile(const std::string& full_name) : File(full_name) { }

//-----------------------------------------------------------------------------
// CfgFile::load
//-----------------------------------------------------------------------------
void CfgFile::load() throw(Exception)
{
  try
  {
    std::string content;
    File::load(&content);
    load_from_string(content);
    std::string strLine;
  }
  catch (Exception& e)
  {
    throw e;
  }
}

//-----------------------------------------------------------------------------
// CfgFile::load
//-----------------------------------------------------------------------------
void CfgFile::load(const std::string& file_full_name) throw(Exception)
{
  m_dictSection.clear();
  m_strSection = "";
  set(file_full_name);
  load();
}

//-----------------------------------------------------------------------------
// CfgFile::load
//-----------------------------------------------------------------------------
void CfgFile::load_from_string(const std::string& _content)
{
  std::string content = _content;
  std::string line;

  // Create a 'default' section
  Section aSection;
  m_strSection = CFG_FILE_DEFAULT_SECTION;
  m_dictSection[m_strSection] = aSection;
  bool is_object = false;
  bool is_multiline = false;
  std::string param_name, param_value;
  Objects::iterator obj_iterator;
  Parameters* current_obj_parameters_p = NULL;

  while( !content.empty() )
  {
    // Extract next line
    StringUtil::extract_token(&content, '\n', &line);
    // Supress blank characters at the begining and the end of the string
    StringUtil::trim(&line);

    if( is_multiline )
    { // multi-lines value mode
      if( !StringUtil::match(line, "*\\") )
        is_multiline = false;
      else
        line = line.substr(0, line.size() - 1);

      // Concat line content
      param_value += "\n" + line;

      if( !is_multiline )
      {
        StringUtil::trim(&param_value);
        if( !is_object )
          m_dictSection[m_strSection].m_dictParameters[param_name] = param_value;
        else
          (*current_obj_parameters_p)[param_name] = param_value;
      }
      continue;
    }

    if( line.empty() || StringUtil::match(line, "#*") || StringUtil::match(line, ";*") )
      // empty lines or comments lines are passed
      continue;

    if( StringUtil::match(line, "-*") )
    { // lines begining by a '-' means end of objects
      is_object = false;
      continue;
    }

    if( StringUtil::match(line, "[*]") )
    {  // Section declaration
      Section aSection;
      StringUtil::extract_token(&line, '[', ']', &m_strSection);
      StringUtil::trim(&m_strSection);
      m_dictSection[m_strSection] = aSection;
      is_object = false;
      continue;
    }

    if( StringUtil::match(line, "*=*") )
    { // New parameter
      StringUtil::extract_token(&line, '=', &param_name);
      param_value = line;
      StringUtil::trim(&param_value);
      StringUtil::trim(&param_name);

      if( StringUtil::match(param_value, "*\\") )
      { // Multi-lines value
        // Remove the '\'
        param_value = param_value.substr(0, param_value.size() - 1);
        is_multiline = true;
      }
      else
      {
        if( !is_object )
          m_dictSection[m_strSection].m_dictParameters[param_name] = param_value;
        else
          (*current_obj_parameters_p)[param_name] = param_value;
      }
      continue;
    }

    if( StringUtil::match(line, "*:") )
    { // New object
      std::string strObjectType;
      StringUtil::extract_token(&line, ':', &strObjectType);
      StringUtil::trim(&line);
      StringUtil::trim(&strObjectType);
      is_object = true;
      Objects::iterator it1 = m_dictSection[m_strSection].m_objects.find(strObjectType);
      if( it1 == m_dictSection[m_strSection].m_objects.end() )
      {
        it1 = m_dictSection[m_strSection].m_objects.insert(m_dictSection[m_strSection].m_objects.end(), std::pair<std::string, ObjectCollection>(strObjectType, ObjectCollection()));
      }
      it1->second.push_back(Parameters());
      current_obj_parameters_p = &(it1->second.back());
      //obj_iterator = m_dictSection[m_strSection].m_objects.insert(std::pair<std::string, Parameters>(strObjectType, Parameters()));
      continue;
    }

    m_dictSection[m_strSection].m_vecSingleValues.push_back(line);
    is_object = false;
  }

  if( is_multiline )
  {
    log_warning("Last line of the cfg file " + full_name() + " ends with a '\\'. May be it's incomplete.");
    if( !is_object )
      m_dictSection[m_strSection].m_dictParameters[param_name] = line;
    else
      (*current_obj_parameters_p)[param_name] = line;
  }

  // Set cursor on default section
  m_strSection = "default";
}

//-----------------------------------------------------------------------------
// CfgFile::get_sections
//-----------------------------------------------------------------------------
void CfgFile::get_sections(std::list<std::string>* list_p) const
{
  for( std::map<std::string, Section>::const_iterator cit = m_dictSection.begin();
       cit != m_dictSection.end(); ++cit )
  {
    list_p->push_back(cit->first);
  }
}

//-----------------------------------------------------------------------------
// CfgFile::get_values
//-----------------------------------------------------------------------------
const CfgFile::Values &CfgFile::get_values(const std::string& strSection) const
{
  if( strSection.empty() )
    return m_dictSection[m_strSection].m_vecSingleValues;
  else
    return m_dictSection[strSection].m_vecSingleValues;
}

//-----------------------------------------------------------------------------
// CfgFile::has_object
//-----------------------------------------------------------------------------
bool CfgFile::has_object(const std::string& object_type) const
{
  Objects::const_iterator cit = m_dictSection[m_strSection].m_objects.find(object_type);
  if( cit != m_dictSection[m_strSection].m_objects.end() )
    return true;
  return false;
}

//-----------------------------------------------------------------------------
// CfgFile::get_objects
//-----------------------------------------------------------------------------
const CfgFile::ObjectCollection& CfgFile::get_objects(const std::string& object_type) const
{
  Objects::const_iterator cit = m_dictSection[m_strSection].m_objects.find(object_type);
  if( cit != m_dictSection[m_strSection].m_objects.end() )
    return cit->second;

  throw Exception( "NO_DATA",
                        PSZ_FMT("No such objects: %s", PSZ(object_type)),
                        "CfgFile::get_objects(const std::string& object_type)" );
}

//-----------------------------------------------------------------------------
// CfgFile::get_objects
//-----------------------------------------------------------------------------
const CfgFile::Objects& CfgFile::get_objects() const
{
  return m_dictSection[m_strSection].m_objects;
}

//-----------------------------------------------------------------------------
// CfgFile::get_unique_object
//-----------------------------------------------------------------------------
const CfgFile::Parameters& CfgFile::get_unique_object(const std::string& object_name) const
{
  Objects::const_iterator cit = m_dictSection[m_strSection].m_objects.find(object_name);
  if( cit != m_dictSection[m_strSection].m_objects.end() )
    return cit->second[0];

  throw Exception( "NO_DATA",
                        PSZ_FMT("No such object: %s", PSZ(object_name)),
                        "CfgFile::get_unique_object" );
}

//-----------------------------------------------------------------------------
// CfgFile::get_parameters
//-----------------------------------------------------------------------------
const CfgFile::Parameters &CfgFile::get_parameters() const
{
  return m_dictSection[m_strSection].m_dictParameters;
}

//-----------------------------------------------------------------------------
// CfgFile::GetParamValue
//-----------------------------------------------------------------------------
std::string CfgFile::get_param_value(const std::string& param, bool throw_exception) const
{
  CfgFile::Parameters::const_iterator cit =
          m_dictSection[m_strSection].m_dictParameters.find(param);

  if( cit == m_dictSection[m_strSection].m_dictParameters.end() )
  {
    if( throw_exception )
      throw yat::Exception("NO_DATA", std::string("Parameter ") + param + std::string(" not found"), "CfgFile::get_param_value");
    return StringUtil::empty;
  }

  return cit->second;
}

//-----------------------------------------------------------------------------
// CfgFile::has_parameter
//-----------------------------------------------------------------------------
bool CfgFile::has_parameter(const std::string& strParamName) const
{
  CfgFile::Parameters::const_iterator cit =
          m_dictSection[m_strSection].m_dictParameters.find(strParamName);

  return cit != m_dictSection[m_strSection].m_dictParameters.end() ? true : false;
}

//-----------------------------------------------------------------------------
// CfgFile::set_section
//-----------------------------------------------------------------------------
bool CfgFile::set_section(const std::string& strSection, bool bThrowException) const
{
  std::map<std::string, Section>::iterator it = m_dictSection.find(strSection);
  if( it != m_dictSection.end() )
    m_strSection = strSection;
  else if( bThrowException )
      throw Exception("NO_DATA", PSZ(StringUtil::str_format("Section '%s' not found", PSZ(strSection))), "CfgFile::SetSection");
  else
    return false;
  return true;
}

//===========================================================================
// Class DirectoryWatcher
//===========================================================================

//-----------------------------------------------------------------------------
// DirectoryWatcher::Entry::Entry
//-----------------------------------------------------------------------------
DirectoryWatcher::Entry::Entry(const std::string& strFullName)
{
  ptrFile = new FileName(strFullName);
  ptrFile->mod_time(&tmLastModTime);
  bRemoved = false;
}

//-----------------------------------------------------------------------------
// DirectoryWatcher::DirectoryWatcher
//-----------------------------------------------------------------------------
DirectoryWatcher::DirectoryWatcher(const std::string& strDirectoryPath, WatchMode eMode) throw(Exception)
{
  set(strDirectoryPath);
  m_bDirectoryHasChanged = false;
  m_eMode = eMode;

  if( ENUM_FIRST == eMode )
  {
    FileEnum fe(full_name(), FileEnum::ENUM_ALL);
    while( fe.find() )
    {
      m_mapEntry[StringUtil::hash64(fe.full_name())] = new Entry(fe.full_name());
    }
  }
}

//-----------------------------------------------------------------------------
// DirectoryWatcher::priv_has_changed
//-----------------------------------------------------------------------------
bool DirectoryWatcher::priv_has_changed(bool bReset)
{
  CurrentTime tmCur;
  Time tmModTime;
  mod_time(&tmModTime);

  if( m_tmDirModTime.is_empty() && NO_FIRST_ENUM == m_eMode )
  {   // First call
    m_tmLocalModTime = tmCur;
    m_tmDirModTime = tmModTime;
    return true;
  }
  else if( tmModTime != m_tmDirModTime && !m_bDirectoryHasChanged )
  {
    m_bDirectoryHasChanged = true;
    m_tmLocalModTime = tmCur;
    m_tmDirModTime = tmModTime;
    return false;
  }
        // The change is considered at least 1 second after its first observation
  else if( m_bDirectoryHasChanged && tmCur.double_unix() - m_tmLocalModTime.double_unix() > 1.0 )
  {
    if( bReset )
    {
      if( tmModTime == m_tmDirModTime )
        m_bDirectoryHasChanged = false;
      else
      {
        m_tmLocalModTime = tmCur;
        m_tmDirModTime = tmModTime;
      }
    }
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
// DirectoryWatcher::has_changed
//-----------------------------------------------------------------------------
bool DirectoryWatcher::has_changed()
{
  return priv_has_changed(false);
}

//-----------------------------------------------------------------------------
// DirectoryWatcher::get_changes
//-----------------------------------------------------------------------------
void DirectoryWatcher::get_changes(FileNamePtrVector *pvecNewFilesPtr,
                                  FileNamePtrVector *pvecChangedFileNamePtr,
                  FileNamePtrVector *pvecRemovedFileNamePtr) throw(Exception)
{
  Time tmModTimeDir;

  // File modifications does not affect the last modification time of its parent directory
  if( pvecChangedFileNamePtr || priv_has_changed(true) )
  {
    if( pvecRemovedFileNamePtr )
    {
      // Mark all entry as removed for detection of removed entries
      for( EntryMap::iterator it = m_mapEntry.begin(); it != m_mapEntry.end(); it++ )
        it->second->bRemoved = true;
    }

    FileEnum fe(full_name(), FileEnum::ENUM_ALL);
    while( fe.find() )
    {
      uint64 hashFile = StringUtil::hash64(fe.full_name());

      if( m_mapEntry.find(hashFile) != m_mapEntry.end() )
      {
        EntryPtr ptrEntry = m_mapEntry[hashFile];
        ptrEntry->bRemoved = false;

        // Existing file, check for change if needed
        if( pvecChangedFileNamePtr )
        {
          Time tmModTime;
          fe.mod_time(&tmModTime);

          if( ptrEntry->tmLastModTime != tmModTime )
          {
            ptrEntry->tmLastModTime = tmModTime;
            pvecChangedFileNamePtr->push_back(ptrEntry->ptrFile);
          }
        }
      }
      else if( pvecNewFilesPtr )
      {
        m_mapEntry[hashFile] = new Entry(fe.full_name());
        pvecNewFilesPtr->push_back(m_mapEntry[hashFile]->ptrFile);
      }
    }

    if( pvecRemovedFileNamePtr )
    {
      // Delete all removed entry and push them in the user vector
      for( EntryMap::iterator it = m_mapEntry.begin(); it != m_mapEntry.end(); )
      {
        if( it->second->bRemoved )
        {
          EntryMap::iterator itErase = it;
          it++;
          pvecRemovedFileNamePtr->push_back(itErase->second->ptrFile);
          m_mapEntry.erase(itErase);
        }
        else
          it++;
      }
    }
  }
}

//===========================================================================
// Class LockFile
//===========================================================================

//-------------------------------------------------------------------
// LockFile::LockFile
//-------------------------------------------------------------------
LockFile::LockFile(const FileName& fn, Type t)
{
  m_file_name = fn;
  m_type = t;
  m_fd = 0;
}

//-------------------------------------------------------------------
// LockFile::LockFile
//-------------------------------------------------------------------
LockFile::~LockFile()
{
}

//------------------------------------------------------------------------------
// LockFile::async_lock
//------------------------------------------------------------------------------
void LockFile::async_lock(Task* task_p, MessageType lock_msg, MessageType err_msg)
{
  m_async_ptr.reset( new ASyncLock() );

  m_async_ptr->lock_msg = lock_msg;
  m_async_ptr->err_msg = err_msg;
  m_async_ptr->task_p = task_p;

  start_async_lock();
}

//------------------------------------------------------------------------------
// LockFile::async_lock
//------------------------------------------------------------------------------
void LockFile::async_lock(FileLockCallback lock_cb, FileLockCallback err_cb)
{
  m_async_ptr.reset( new ASyncLock() );

  m_async_ptr->lock_msg = 0;
  m_async_ptr->err_msg = 0;
  m_async_ptr->task_p = 0;
  m_async_ptr->lock_callback = lock_cb;
  m_async_ptr->err_callback = err_cb;

  start_async_lock();
}

//------------------------------------------------------------------------------
// LockFile::start_async_lock
//------------------------------------------------------------------------------
void LockFile::start_async_lock()
{
  yat::Pulser::Config cfg;
  cfg.period_in_msecs = 1;
  cfg.num_pulses = 1;
  cfg.callback = yat::PulserCallback::instanciate(*this, &LockFile::lock_callback);
  cfg.user_data = 0;
  m_async_ptr->pulser_ptr.reset( new yat::Pulser(cfg) );
  m_async_ptr->pulser_ptr->start();
}

//------------------------------------------------------------------------------
// LockFile::lock_callback
//------------------------------------------------------------------------------
void LockFile::lock_callback( yat::Thread::IOArg )
{
  try
  {
    lock();
    if( m_async_ptr->task_p && m_async_ptr->lock_msg > 0 )
    {
      m_async_ptr->task_p->post(m_async_ptr->lock_msg);
    }
    else if( !m_async_ptr->lock_callback.is_empty() )
    {
      m_async_ptr->lock_callback(this);
    }
  }
  catch( const yat::Exception& e )
  {
    m_async_ptr->errors = e.errors;

    if( m_async_ptr->task_p && m_async_ptr->err_msg > 0 )
    {
      m_async_ptr->task_p->post(m_async_ptr->err_msg);
    }
    else if( !m_async_ptr->err_callback.is_empty() )
    {
      m_async_ptr->err_callback(this);
    }
  }
}

//------------------------------------------------------------------------------
// LockFile::get_errors
//------------------------------------------------------------------------------
Exception::ErrorList LockFile::get_errors()
{
  if( m_async_ptr )
    return m_async_ptr->errors;
  return Exception::ErrorList();
}

//===========================================================================
// Class AutoLockFile
//===========================================================================
//
//-------------------------------------------------------------------
// AutoLockFile::AutoLockFile
//-------------------------------------------------------------------
AutoLockFile::AutoLockFile(LockFile* lock_p)
{
  m_lock_p = lock_p;
  m_lock_p->lock();
}

//-------------------------------------------------------------------
// AutoLockFile::AutoLockFile
//-------------------------------------------------------------------
AutoLockFile::~AutoLockFile()
{
  m_lock_p->unlock();
}




} // namespace
