//----------------------------------------------------------------------------
// Copyright (c) 2004-2021 Synchrotron SOLEIL
// All rights reserved. This program and the accompanying materials
// are made available under the terms of the GNU Lesser Public License v3
// which accompanies this distribution, and is available at
// http://www.gnu.org/licenses/lgpl.html
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
// YAT LIBRARY
//----------------------------------------------------------------------------
//
// Copyright (C) 2006-2021 The Tango Community
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
//      Stephane Poirier
//      Synchrotron SOLEIL
//------------------------------------------------------------------------------
/*!
 * \author See AUTHORS file
 */

//=============================================================================
// DEPENDENCIES
//=============================================================================
#include <yat/time/Time.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#if (defined WIN32 || defined _WIN32)
  #include <sys\timeb.h>
  #define _X86_
  #include <windef.h>
  #include <winbase.h>
  #include <winnls.h>
#elif (defined _linux_ || defined __linux__)
  #include <sys/time.h>
#endif

// standard library objets
#include <iostream>
#include <time.h>

namespace yat
{

/// Absolute reference : 0 january 4713 b JC at 12h ( -> -4712 for the calculations )
const int REF_YEAR = -4712;

/// Julian date for UNIX reference
const int JULIAN_REF_UNIX = 2440588;

// Error msgs
const char DATE_SET_ERR[]        = "BAD_ARGS";

// Reasons msgs
const char DATE_BAD_ARGS[]        = "Cannot Setting Date object";

// Month names.
static yat::pcsz s_pszMonthEnAbbr[] =
{
  "jan", "feb", "mar", "apr", "may", "jun",
  "jul", "aug", "sep", "oct", "nov", "dec"
};
static pcsz s_pszMonthEn[] =
{
  "january", "february", "march", "april", "may", "june", "july",
  "august", "september", "october", "november", "december"
};

static pcsz s_pszDayEnAbbr[] =
{
  "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"
};
static pcsz s_pszDayEn[] =
{
  "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"
};

//============================================================================
// Internal functions
//============================================================================

// Number of days between 1th march 0 and 1th march iY
long DeltaDaysYear(int iY)
{
  return 365l * iY +
   ((long)iY/4l) -     // leap year
   ((long)iY/100l) +   // 100th are not leap
   ((long)iY/400l);    // 400th are leap years
}

// Number of days from 1th march to 1th <iM>
long DeltaDaysMonth(int iM)
{
  return (306l*iM + 4l)/10l;
}

//----------------------------------------------------------------------------
// Calculate julian date for a day at 12.
// piDeltaDays : pointer to delta with julian calendar without the reform (in 1582)
//----------------------------------------------------------------------------
long calc_julian_date(int iY, int iM, int iD, int *piDeltaDays=NULL)
{
  int flGregLeap = 0;
  int flJulianLeap = 0;
  int iYMod = iY % 4;
  int iYFromRef = iY - REF_YEAR;
  int iDeltaDays = 0;
  // Number of days in Julian calendar
  long lNbDays = iYFromRef * 365L + ((iYFromRef + 3) / 4);

  // Leap year candidate
  if( iYMod == 0 )
  {
    flJulianLeap = 1;
    flGregLeap = 1;
  }

  // Year in Gregorian calendar
  // Applying reform :
  if( iY > 1582 || (iY == 1582 && iM > 12 ) || (iY == 1582 && iM == 12 && iD > 19) )
    iDeltaDays = 10;

  // Calculating delta
  int iDM = 0;
  if( iY >= 1700 )
    iDM = -((iY - 1) / 100) + ((iY - 1) / 400) + 12;

  if( (iY % 400) && !(iY % 100) )
    flGregLeap = 0;

  iDeltaDays -= iDM;

  // Add number of days
  if( iM <= 8 )
    lNbDays += (iM-1) * 30 + iM/2;
  else
    lNbDays += (iM-1) * 30 + (iM-1) / 2 + 1;

  if( iM >= 3 && flGregLeap )
    lNbDays--;
  else if( iM >= 3 && !flGregLeap )
  {
    lNbDays += flJulianLeap - 2;
    iDeltaDays += flJulianLeap;
  }

  if( piDeltaDays )
    // Returns delta between Gregorian et Julian calandar
    *piDeltaDays = iDeltaDays;

  // Julian day in right calendar
  return lNbDays + iD - 1 - iDeltaDays;
}

//----------------------------------------------------------------------------
// Calculate Julian day date
//----------------------------------------------------------------------------
void JJToDate(long lJJ, int16 *piYear, uint8 *puiMonth, uint8 *puiDay)
{
  int iYear, iMonth=0, iDay;
  // Retreive year
  int iJulianYear = (int)(lJJ / 365.25);
  iYear =  iJulianYear + REF_YEAR;
  int iDeltaDays;
  // Retreive le julian day for first day of this year
  long lJJBegYear = calc_julian_date(iYear, 1, 1, &iDeltaDays);

  // Position in respect to the reform
  int iDJ = int(lJJ - lJJBegYear);
  int flLeap = 0;

  if( lJJ < 2299226.5 )
  {
    iDJ -= iDeltaDays;

    if( (iYear % 4) == 0 )
      flLeap = 1;
  }
  else if( iYear == 1582 ) // && dJJ >= 2299226.5
    iDJ += 10;

  if( flLeap == 0 )
  {
    if( (iYear % 4) == 0 )
      flLeap = 1;
  }

  if( lJJ >= 2299226.5 && (iYear % 100) == 0 && (iYear % 400) != 0 )
    flLeap = 0;

  int iDM = 0;
  if( flLeap && iDJ == 59 )
  {
    // 29th februar
    iDM = 30;
    iMonth = 2;
  }

  if( flLeap && iDJ > 59 )
    // Substract leap day
    iDJ -= 1;

  // Retreive month
  iDJ -= 1;
  if( iDJ >= 333 )
  {
    iMonth = 12;
    iDM = 333;
  }
  else if( iDJ >= 303 )
  {
    iMonth = 11;
    iDM = 303;
  }
  else if( iDJ >= 272 )
  {
    iMonth = 10;
    iDM = 272;
  }
  else if( iDJ >= 242 )
  {
    iMonth = 9;
    iDM = 242;
  }
  else if( iDJ >= 211 )
  {
    iMonth = 8;
    iDM = 211;
  }
  else if( iDJ >= 180 )
  {
    iMonth = 7;
    iDM = 180;
  }
  else if( iDJ >= 150 )
  {
    iMonth = 6;
    iDM = 150;
  }
  else if( iDJ >= 119 )
  {
    iMonth = 5;
    iDM = 119;
  }
  else if( iDJ >= 89 )
  {
    iMonth = 4;
    iDM = 89;
  }
  else if( iDJ >= 58 && iDM == 0 )
  {
    iMonth = 3;
    iDM = 58;
  }
  else if( iDJ >= 30 && iDM == 0 )
  {
    iMonth = 2;
    iDM = 30;
  }
  else if( iDM == 0 )
  {
    iMonth = 1;
    iDM = -1;
  }

  // Retreive day
  iDay = iDJ - iDM + 1;

  if( iDJ > 333 && iDay > 31 )
  {
    iYear += 1;
    iDay -= 31;
    iMonth = 1;
  }

  *piYear = iYear;
  *puiMonth = iMonth;
  *puiDay = iDay;
}


//=============================================================================
// DateFields structure
//=============================================================================
//----------------------------------------------------------------------------
// DateFields::Clear
//----------------------------------------------------------------------------
void DateFields::clear()
{
  memset(this, 0, sizeof(*this));
}

//----------------------------------------------------------------------------
// DateFields::is_empty
//----------------------------------------------------------------------------
int DateFields::is_empty() const
{
  return year == 0 &&
         month == 0 &&
         day == 0 &&
         hour == 0 &&
         min == 0 &&
         sec == 0.;
}

//=============================================================================
// Time class
//=============================================================================

const std::string Time::date_inter = "%y/%m/%d";
const std::string Time::iso8601    = "%Y-%m-%dT%H:%M:%S";
const std::string Time::iso8601_d  = "%Y-%m-%d";
const std::string Time::iso8601_t  = "%H:%M:%S";
const std::string Time::iso8601_tz = "%H:%M:%S%Z";
const std::string Time::iso8601_z  = "%Y-%m-%dT%H:%M:%S%Z";
const unsigned short Time::millisec = 3;
const unsigned short Time::microsec = 6;

//----------------------------------------------------------------------------
// Time::Time
//----------------------------------------------------------------------------
Time::Time(const std::string& date_time, const std::string& format)
: m_llTime(0), m_local_time(false), m_tz_bias(-1)
{
  from_string(date_time, format);
}

//----------------------------------------------------------------------------
// Time::Time
//----------------------------------------------------------------------------
Time::Time(int16 iYear, uint8 uiMonth, uint8 uiDay, uint8 uiHour, uint8 uiMin, double dSec)
: m_llTime(0), m_local_time(false), m_tz_bias(-1)
{
  set(iYear, uiMonth, uiDay, uiHour, uiMin, dSec);
}

//----------------------------------------------------------------------------
// private method
//----------------------------------------------------------------------------
void Time::set_tz(const std::string& tz)
{
  if( 'Z' == tz[0] && tz.size() == 1 )
  {
    m_tz_bias = -1;
    m_local_time = false;
  }
  else
  {
    if( tz.size() != 5 || (tz[0] != '+' && tz[0] != '-') ||
        !isdigit(tz[1]) || !isdigit(tz[2]) || !isdigit(tz[3]) || !isdigit(tz[4]) )
      throw yat::Exception("BAD_FORMAT", "Bad time zone field string", "Time::set_tz");

    m_tz_bias =    yat::String(tz.substr(1,2)).to_num<int>() * 60
                +  yat::String(tz.substr(3,2)).to_num<int>();

    if( '-' == tz[0] )
      m_tz_bias = -m_tz_bias;

    m_local_time = true;
  }
}

//----------------------------------------------------------------------------
// Time::time_zone_bias
//----------------------------------------------------------------------------
int Time::time_zone_bias() const
{
  static int s_tz_bias = -1;
  if( -1 == s_tz_bias )
  {
    s_tz_bias = (int(double(yat::CurrentTime().raw_value() -
                            yat::CurrentUTime().raw_value()) / MICROSEC_PER_SEC + 0.5)) / 60;
  }

  if( -1 == m_tz_bias )
    m_tz_bias = s_tz_bias;

  return m_tz_bias;
}

//----------------------------------------------------------------------------
// Time::nb_days_in_month
//----------------------------------------------------------------------------
uint8 Time::nb_days_in_month(uint8 iMonth, int16 iYear)
{
  switch( iMonth )
  {
    case 1: // january
    case 3: // march
    case 5: // may
    case 7: // july
    case 8: // august
    case 10:// october
    case 12:// december
      return 31;

    case 2: // februar : depend of year
      if( (iYear%4 == 0 && iYear < 1582) ||
          (iYear%4 == 0 && ( (iYear%100) || !(iYear%400) )) )
        return 29; // leap years
      else
        return 28; // ordinary years

    default : // other months
      return 30;
  }
}

//----------------------------------------------------------------------------
// Time::nb_days_in_year
//----------------------------------------------------------------------------
uint16 Time::nb_days_in_year(int16 iYear)
{
  if( !(iYear%4) && ( (iYear%100) || !(iYear%400) ) )
    // Leap year
    return 366;

  return 365;
}

//----------------------------------------------------------------------------
// Time::month_name
//----------------------------------------------------------------------------
pcsz Time::month_name(uint8 iMonth, bool long_name)
{
  if( iMonth > 0 && iMonth < 13 )
  {
    if( long_name )
      return s_pszMonthEn[iMonth-1];
    else
      return s_pszMonthEnAbbr[iMonth-1];
  }

  return "";
}

//----------------------------------------------------------------------------
// Time::day_name
//----------------------------------------------------------------------------
pcsz Time::day_name(uint8 day, bool long_name)
{
  if( day > 0 && day < 8 )
  {
    if( long_name )
      return s_pszDayEn[day-1];
    else
      return s_pszDayEnAbbr[day-1];
  }

  return "";
}

//----------------------------------------------------------------------------
// Time::set_current
//----------------------------------------------------------------------------
Time& Time::set_current(bool utc)
{
  #ifdef WIN32
    SYSTEMTIME sysTm;
    if( utc )
    {
      GetSystemTime(&sysTm);
      set_utc(sysTm.wYear, (uint8)sysTm.wMonth, (uint8)sysTm.wDay, (uint8)sysTm.wHour, (uint8)sysTm.wMinute,
          (double)sysTm.wSecond + ((double)sysTm.wMilliseconds)/1000.0);
    }
    else
    {
      GetLocalTime(&sysTm);
      set_local(sysTm.wYear, (uint8)sysTm.wMonth, (uint8)sysTm.wDay, (uint8)sysTm.wHour, (uint8)sysTm.wMinute,
        (double)sysTm.wSecond + ((double)sysTm.wMilliseconds)/1000.0);
    }

  #else
    long lTm, lMs;
    struct timeval tv;
    struct timezone tzp;
    gettimeofday(&tv, &tzp);
    lTm = tv.tv_sec;
    lMs = tv.tv_usec;

    // Convert from 'time_t' format to 'struct tm' format
    struct tm tmCurrent;
    if( utc )
    {
      gmtime_r(&lTm, &tmCurrent);
      set_utc(tmCurrent.tm_year+1900, tmCurrent.tm_mon+1, tmCurrent.tm_mday, tmCurrent.tm_hour,
          tmCurrent.tm_min, (double)tmCurrent.tm_sec + (lMs/1000000.));
    }
    else
    {
      localtime_r(&lTm, &tmCurrent);
      set_local(tmCurrent.tm_year+1900, tmCurrent.tm_mon+1, tmCurrent.tm_mday, tmCurrent.tm_hour,
          tmCurrent.tm_min, (double)tmCurrent.tm_sec + (lMs/1000000.));
    }
  #endif
  return *this;
}

//----------------------------------------------------------------------------
// Time::set_current_utc
//----------------------------------------------------------------------------
Time& Time::set_current_utc()
{
  return set_current(true);
}

//----------------------------------------------------------------------------
// Time::set_current_local
//----------------------------------------------------------------------------
Time& Time::set_current_local()
{
  return set_current();
}

//----------------------------------------------------------------------------
// Time::now
//----------------------------------------------------------------------------
Time& Time::now()
{
  if( m_local_time )
    return set_current();
  else
    return set_current(true);
}

//----------------------------------------------------------------------------
// Time::local_to_utc
//----------------------------------------------------------------------------
Time& Time::to_utc()
{
  if( m_local_time )
    *this -= 60 * time_zone_bias();
  m_local_time = false;
  return *this;
}

//----------------------------------------------------------------------------
// Time::utc_to_local
//----------------------------------------------------------------------------
Time& Time::to_local()
{
  if( !m_local_time )
    *this += 60 * time_zone_bias();
  m_local_time = true;
  return *this;
}

//----------------------------------------------------------------------------
// Time::local_to_UT
//----------------------------------------------------------------------------
void Time::local_to_UT()
{
  to_utc();
}

//----------------------------------------------------------------------------
// Time::UT_to_local
//----------------------------------------------------------------------------
void Time::UT_to_local()
{
  to_local();
}

//----------------------------------------------------------------------------
// Time::get
//----------------------------------------------------------------------------
void Time::get(DateFields *pDF) const
{
  // Retreive Julian date
  long lJJ = julian_date();

  // Day of week
  pDF->day_of_week = (int)(lJJ % 7) + 2;
  if( pDF->day_of_week > 7 )
    pDF->day_of_week -= 7;

  // Set monday as first day of week
  pDF->day_of_week -= 1;
  if( pDF->day_of_week == 0 )
    pDF->day_of_week = 7;

  // Retreive date
  JJToDate(lJJ, &pDF->year, &pDF->month, &pDF->day);

  // Hour, minutes, seconds
  uint32 ulSec = uint32((m_llTime / int64(MICROSEC_PER_SEC)) % int64(SEC_PER_DAY));
  pDF->hour = (uint8)(ulSec / SEC_PER_HOUR);
  ulSec -= pDF->hour * SEC_PER_HOUR;
  pDF->min = (uint8)(ulSec / SEC_PER_MIN);
  int iSec = (uint8)(ulSec - pDF->min * SEC_PER_MIN);
  // Retreive micro-seconds
  double dMicro = double(m_llTime % MICROSEC_PER_SEC);
  pDF->sec = iSec + dMicro / (double)MICROSEC_PER_SEC;

  // Calculate day of year
  pDF->day_of_year = (int16)(julian_date() - calc_julian_date(pDF->year, 1, 1)) + 1;

  // Week of year
  int16 iDay = pDF->day_of_year - pDF->day_of_week;
  pDF->week_of_year = iDay / 7;
  if( iDay >= 0 )
    pDF->week_of_year++;

  pDF->tz_bias = time_zone_bias();
}

//----------------------------------------------------------------------------
// Time::set
//----------------------------------------------------------------------------
void Time::set(const DateFields &df)
{
  if( df.is_empty() )
    // Empty date
    m_llTime = 0;
  else
  {
    uint8 uiMonth = df.month;
    int16 iYear  = df.year;

    // Check fields
    if( uiMonth > 12 || uiMonth < 1 ||
        df.day < 1 || df.day > nb_days_in_month(uiMonth, iYear) ||
        df.hour > 24 ||
        df.min > 59 ||
        df.sec < 0 || df.sec >= 60. )
    {
      throw Exception(DATE_SET_ERR, DATE_BAD_ARGS, "Time::Set");
    }

    // Julian day at 12.
    double dJJ = calc_julian_date(df.year, df.month , df.day);
    m_llTime = int64(dJJ * SEC_PER_DAY) * int64(MICROSEC_PER_SEC) +
               int64(df.hour * SEC_PER_HOUR) * int64(MICROSEC_PER_SEC) +
               int64(df.min * SEC_PER_MIN) * int64(MICROSEC_PER_SEC) +
               int64(df.sec * MICROSEC_PER_SEC);
    m_local_time = df.local_time;
  }
}

//----------------------------------------------------------------------------
// Time::set
//----------------------------------------------------------------------------
void Time::set(int16 iYear, uint8 uiMonth, uint8 uiDay,
               uint8 uiHour, uint8 uiMin, double dSec)
{
  DateFields df;
  df.year  = iYear;
  df.month = uiMonth;
  df.day   = uiDay;
  df.hour  = uiHour;
  df.min   = uiMin;
  df.sec   = dSec;
  df.local_time = false;
  set(df);
}

//----------------------------------------------------------------------------
// Time::set_utc
//----------------------------------------------------------------------------
void Time::set_utc(int16 year, uint8 month, uint8 day,
                   uint8 hour, uint8 min, double sec)
{
  set(year, month, day, hour, min, sec);
  m_local_time = false;
}

//----------------------------------------------------------------------------
// Time::set_local
//----------------------------------------------------------------------------
void Time::set_local(int16 year, uint8 month, uint8 day,
                     uint8 hour, uint8 min, double sec)
{
  set(year, month, day, hour, min, sec);
  m_local_time = true;
}


//----------------------------------------------------------------------------
// Time::set_day_of_year
//----------------------------------------------------------------------------
void Time::set_day_of_year(uint16 iDayOfYear, int16 iYear)
{
  DateFields df;
  get(&df);
  set(iYear, 1, 1, df.hour, df.min, df.sec);
  add_sec((iDayOfYear - 1) * SEC_PER_DAY);
}

// Macro for implementing access to date fields
#define IMPLEMENT_DATEFIELD(Name)     \
void Time::set_##Name(uint8 ui)       \
{                                     \
  DateFields df;                     \
  get(&df);                           \
  df.Name = ui;                     \
  set(df);                            \
}                                     \
                                      \
uint8 Time::Name() const               \
{                                     \
  DateFields df;                     \
  get(&df);                           \
  return df.Name;                  \
}

IMPLEMENT_DATEFIELD(hour)
IMPLEMENT_DATEFIELD(day)
IMPLEMENT_DATEFIELD(month)

//----------------------------------------------------------------------------
// Time::set_minute
//----------------------------------------------------------------------------
void Time::set_minute(uint8 iMinute)
{
  DateFields df;
  get(&df);
  df.min = iMinute;
  set(df);
}

//----------------------------------------------------------------------------
// Time::minute
//----------------------------------------------------------------------------
uint8 Time::minute() const
{
  DateFields df;
  get(&df);
  return df.min;
}

//----------------------------------------------------------------------------
// Time::set_year
//----------------------------------------------------------------------------
void Time::set_year(int16 iYear)
{
  DateFields df;
  get(&df);
  df.year = iYear;
  set(df);
}

//----------------------------------------------------------------------------
// Time::set_month
//----------------------------------------------------------------------------
void Time::set_month(const std::string& month_name)
{
  set_month(get_month_from_name(month_name));
}

//----------------------------------------------------------------------------
// Time::year
//----------------------------------------------------------------------------
int16 Time::year() const
{
  DateFields df;
  get(&df);
  return df.year;
}

//----------------------------------------------------------------------------
// Time::set_sec
//----------------------------------------------------------------------------
void Time::set_second(double dSec)
{
  DateFields df;
  get(&df);
  df.sec = dSec;
  set(df);
}

//----------------------------------------------------------------------------
// Time::set_time_zone_bias
//----------------------------------------------------------------------------
void Time::set_time_zone_bias(int minutes)
{
  m_tz_bias = minutes;
}

//----------------------------------------------------------------------------
// Time::sec
//----------------------------------------------------------------------------
double Time::second() const
{
  DateFields df;
  get(&df);
  return df.sec;
}

//----------------------------------------------------------------------------
// Time::micro
//----------------------------------------------------------------------------
long Time::micro() const
{
  return long(m_llTime % MICROSEC_PER_SEC);
}

//----------------------------------------------------------------------------
// Time::ms
//----------------------------------------------------------------------------
long Time::ms() const
{
  return long(m_llTime % MICROSEC_PER_SEC) / 1000;
}

//----------------------------------------------------------------------------
// Time::julian_date
//----------------------------------------------------------------------------
long Time::julian_date() const
{
  return long(m_llTime / MICROSEC_PER_DAY);
}

//----------------------------------------------------------------------------
// Time::set_julian_date
//----------------------------------------------------------------------------
void Time::set_julian_date(long lJulianDate)
{
  m_llTime = lJulianDate * MICROSEC_PER_DAY;
}

//----------------------------------------------------------------------------
// Time::day_of_week
//----------------------------------------------------------------------------
uint8 Time::day_of_week() const
{
  DateFields df;
  get(&df);
  return df.day_of_week;
}

//----------------------------------------------------------------------------
// Time::day_of_year
//----------------------------------------------------------------------------
uint16 Time::day_of_year() const
{
  long lJStartDate = calc_julian_date(year(), 1, 1);
  return (int)(julian_date() - lJStartDate) + 1;
}

//----------------------------------------------------------------------------
// Time::week_of_year
//----------------------------------------------------------------------------
uint8 Time::week_of_year() const
{
  DateFields df;
  get(&df);
  return df.week_of_year;
}

//----------------------------------------------------------------------------
// Time::long_unix()
//----------------------------------------------------------------------------
long Time::long_unix() const
{
  if( m_local_time )
  {
    Time tm(*this);
    tm.to_utc();
    return (long)(((tm.raw_value() / MICROSEC_PER_SEC) - int64(JULIAN_REF_UNIX) * int64(SEC_PER_DAY)));
  }
  else
    return (long)(((m_llTime / MICROSEC_PER_SEC) - int64(JULIAN_REF_UNIX) * int64(SEC_PER_DAY)));
}

//----------------------------------------------------------------------------
// Time::set_long_unix
//----------------------------------------------------------------------------
void Time::set_long_unix(long lRefSec)
{
  m_llTime = (int64(JULIAN_REF_UNIX) * int64(SEC_PER_DAY) + int64(lRefSec)) * int64(MICROSEC_PER_SEC);
  m_local_time = false;
}

//----------------------------------------------------------------------------
// Time::double_unix()
//----------------------------------------------------------------------------
double Time::double_unix() const
{
  if( m_local_time )
  {
    Time tm(*this);
    tm.to_utc();
    return double(((tm.raw_value() / MICROSEC_PER_SEC) - int64(JULIAN_REF_UNIX) * int64(SEC_PER_DAY)))
         + ms() / 1000.0;
  }
  return double(((m_llTime / MICROSEC_PER_SEC) - int64(JULIAN_REF_UNIX) * int64(SEC_PER_DAY)))
         + ms() / 1000.0;
}

//----------------------------------------------------------------------------
// Time::set_double_unix
//----------------------------------------------------------------------------
void Time::set_double_unix(double dRefSec)
{
  m_llTime = (int64(JULIAN_REF_UNIX) * int64(SEC_PER_DAY) * int64(1000) +
             int64(dRefSec * 1000)) * int64(1000);
  m_local_time = false;
}

//----------------------------------------------------------------------------
// Time::to_ISO8601_ms_TU
//----------------------------------------------------------------------------
std::string Time::to_ISO8601_ms_TU() const
{
  return to_string_utc(yat::Time::iso8601, 3);
}

//----------------------------------------------------------------------------
// Time::to_ISO8601_micro_TU
//----------------------------------------------------------------------------
std::string Time::to_ISO8601_micro_TU() const
{
  return to_string_utc(yat::Time::iso8601, 6);
}

//----------------------------------------------------------------------------
// Time::to_ISO8601_ms
//----------------------------------------------------------------------------
std::string Time::to_ISO8601_ms() const
{
  return to_string(yat::Time::iso8601, 3);
}

//----------------------------------------------------------------------------
// Time::to_ISO8601
//----------------------------------------------------------------------------
std::string Time::to_ISO8601() const
{
  return to_string(yat::Time::iso8601);
}

//----------------------------------------------------------------------------
// Time::to_ISO8601_micro
//----------------------------------------------------------------------------
std::string Time::to_ISO8601_micro() const
{
  return to_string_utc(yat::Time::iso8601, 6);
}

//----------------------------------------------------------------------------
// Time::to_local_ISO8601
//----------------------------------------------------------------------------
std::string Time::to_local_ISO8601() const
{
  return to_string_local(yat::Time::iso8601);
}

//----------------------------------------------------------------------------
// Time::to_local_ISO8601_ms
//----------------------------------------------------------------------------
std::string Time::to_local_ISO8601_ms() const
{
  return to_string_local(yat::Time::iso8601, 3);
}

//----------------------------------------------------------------------------
// Time::to_local_ISO8601_micro
//----------------------------------------------------------------------------
std::string Time::to_local_ISO8601_micro() const
{
  return to_string_local(yat::Time::iso8601, 6);
}

//----------------------------------------------------------------------------
// Time::to_inter
//----------------------------------------------------------------------------
std::string Time::to_inter(bool bMillis) const
{
  if( bMillis )
    return to_string("%Y-%m-%d %H:%M:%S", 3);
  else
    return to_string("%Y-%m-%d %H:%M:%S");
}

//----------------------------------------------------------------------------
// Time::to_string_local
//----------------------------------------------------------------------------
yat::String Time::to_string_local(const std::string& format, unsigned short precision) const
{
  if( !m_local_time )
    return Time(*this).to_local().to_string(format, precision);
  else
    return to_string(format, precision);
}

//----------------------------------------------------------------------------
// Time::to_string_utc
//----------------------------------------------------------------------------
yat::String Time::to_string_utc(const std::string& format, unsigned short precision) const
{
  if( m_local_time )
    return Time(*this).to_utc().to_string(format, precision);
  else
    return to_string(format, precision);
}

//----------------------------------------------------------------------------
// internal utility function
//----------------------------------------------------------------------------
std::string _iso8601_bias(const Time &tm)
{
  if( tm.utc() )
    return "Z";

  std::string fmt;
  int bias = tm.time_zone_bias();
  if( bias < 0 )
    fmt = "-{02d}{02d}";
  else
    fmt = "+{02d}{02d}";

  int h = abs(bias) / 60;
  int m = abs(bias) - (60 * h);

  return Format(fmt).arg(h).arg(m);
}

//----------------------------------------------------------------------------
// Time::to_string
//----------------------------------------------------------------------------
yat::String Time::to_string(const std::string& format, unsigned short precision) const
{
  bool get_identifier = false;
  bool add_Z_if_utc = false;
  std::ostringstream oss;
  DateFields df;
  get(&df);


  for( std::size_t i_f = 0; i_f < format.size(); ++i_f )
  {
    if( '%' == format[i_f] && !get_identifier )
    {
      get_identifier = true;
      continue;
    }

    if( get_identifier )
    {
      switch( format[i_f] )
      {
        case 'a':
          oss << s_pszDayEnAbbr[df.day_of_week - 1];
          break;
        case 'A':
          oss << s_pszDayEn[df.day_of_week - 1];
          break;
        case 'b':
        case 'h':
          oss << s_pszMonthEnAbbr[df.month - 1];
          break;
        case 'B':
          oss << s_pszMonthEn[df.month - 1];
          break;
        case 'd':
          oss.width(2); oss.fill('0'); oss << int(df.day);
          break;
        case 'H':
          oss.width(2); oss.fill('0'); oss << int(df.hour);
          add_Z_if_utc = true;
          break;
        case 'j':
          oss.width(3); oss.fill('0'); oss << df.day_of_year;
          break;
        case 'm':
          oss.width(2); oss.fill('0'); oss << int(df.month);
          break;
        case 'M':
          oss.width(2); oss.fill('0'); oss << int(df.min);
          break;
        case 'R':
          oss.width(2); oss.fill('0'); oss << int(df.hour) << ':';
          oss.width(2); oss.fill('0'); oss << int(df.min);
          break;
        case 's':
          oss << long_unix();
          break;
        case 'S':
          if( 0 == precision )
          { oss.width(2); oss.fill('0'); oss << int(df.sec); }
          else
          { oss.width(precision + 3); oss.precision(precision); oss << std::fixed << df.sec; }
          break;
        case 't':
          oss << '\t';
          break;
        case 'T':
          oss.width(2); oss.fill('0'); oss << df.hour << ':';
          oss.width(2); oss.fill('0'); oss << df.min << ':';
          oss.width(2); oss.fill('0'); oss << int(df.sec);
          break;
        case 'u':
          oss << df.day_of_week;
          break;
        case 'U':
          oss.width(2); oss.fill('0'); oss << df.week_of_year;
          break;
        case 'w':
         {
           int d = df.day_of_week;
           if( 7 == d ) d = 0; // Sunday is '0'
           oss << d;
           break;
         }
        case 'y':
          oss.width(2); oss.fill('0'); oss << (df.year % 100);
          break;
        case 'Y':
          oss.width(4); oss.fill('0'); oss << df.year;
          break;
        case 'Z':
          oss << _iso8601_bias(*this);
          add_Z_if_utc = false;
          break;
        default:
          throw yat::Exception("BAD_FORMAT",
                               std::string("Unknown date-time identifier: ") +
                               format.substr(i_f, 1) + " in format: " + format,
                               "Time::to_string");
      }
      get_identifier = false;
    }
    else
      oss << format[i_f];
  }

  if( add_Z_if_utc && utc() )
    oss << 'Z';

  return yat::String(oss.str());
}

//----------------------------------------------------------------------------
// Time::from_string
//----------------------------------------------------------------------------
void Time::from_string(const std::string& date_time, const std::string& format)
{
  char identifier = '\0';
  bool get_identifier = false;
  std::size_t field_len = 0;
  bool string_id = false;
  std::size_t i_s = 0;

  for( std::size_t i_f = 0; i_f < format.size(); ++i_f )
  {
    if( '%' == format[i_f])
    {
      get_identifier = true;
      continue;
    }

    if( get_identifier )
    {
      string_id = false;
      switch( format[i_f] )
      {
        case 'a':
        case 'A':
        case 'b':
        case 'B':
        case 'h':
          string_id = true;
          field_len = 0;
          break;
        case 'Y':
          field_len = 4;
          break;
        case 'y':
        case 'd':
        case 'm':
        case 'H':
        case 'M':
        case 'S':
          field_len = 2;
          break;
        case 'Z':
          if( 'Z' == date_time[i_s] )
            field_len = 1;
          else
            field_len = 5;
          string_id = true;
          break;
        case 'j':
          field_len = 3;
          break;
        default:
          if( isdigit(format[i_f]) && format[i_f] - '0' > 0 && format[i_f] - '0' < 7 )
            // seconds value presision
            field_len = format[i_f] - '0';
          else
            throw yat::Exception("BAD_FORMAT",
                               std::string("Unknown date-time identifier: ") +
                               format.substr(i_f, 1) + " in format: " + format,
                               "Time::from_string");
      }

      if( string_id )
      {
        // Get string (terminated by first non-alpha character)
        std::string v;
        if( 0 == field_len )
        {
          while( i_s < date_time.size() && isalpha(date_time[i_s]) )
            v.push_back(date_time[i_s++]);
        }
        else if( i_s + field_len <= date_time.size() )
        {
          v.append(date_time.substr(i_s, field_len));
          i_s += field_len;
        }

        switch( format[i_f] )
        {
          case 'a':
          case 'A':
            // do nothing
            break;
          case 'b':
          case 'B':
          case 'h':
            set_month(v);
            break;
          case 'Z':
            set_tz(v);
            break;
          default:
            throw yat::Exception("BAD_FORMAT",
                                 std::string("Unknown date-time identifier: ") +
                                 format.substr(i_f, 1) + " in format: " + format,
                                 "Time::from_string");
        }
      }
      else
      {
        unsigned long v = 0;
        for( std::size_t i = 0; i < field_len; ++i )
        {
          if( !isdigit(date_time[i_s]) || i_s >= date_time.size() )
            throw yat::Exception("BAD_FORMAT", "Bad date-time string: expected digit", "Time::from_string");

          v *= 10;
          v += date_time[i_s++] - char('0');
        }

        switch( format[i_f] )
        {
          case 'y':
            set_year(v + 2000);
            break;
          case 'Y':
            set_year(v);
            break;
          case 'm':
            set_month(v);
            break;
          case 'd':
            set_day(v);
            break;
          case 'j':
            set_day_of_year(v, year());
            break;
          case 'H':
            set_hour(v);
            break;
          case 'M':
            set_minute(v);
            break;
          case 'S':
            set_second(v);
            break;
          default:
            if( isdigit(format[i_f]) )
            {
              if( field_len < 6 )
                set_second( second() + ((double)v / pow(10, field_len)));
              else
                set_second( second() + (((double)v + 0.5)/ pow(10, field_len)));
            }
            else
              throw yat::Exception("BAD_FORMAT",
                                   std::string("Unknown date-time identifier: ") +
                                   format.substr(i_f, 1) + " in format: " + format,
                                   "Time::from_string");
        }
      }
      get_identifier = false;
      continue;
    }
    if( date_time.size() < i_s + 1 || date_time[i_s++] != format[i_f] )
      throw yat::Exception("BAD_FORMAT",
                           std::string("Time string: '")
                           + date_time
                           + std::string("' doesn't match the required format: '")
                           + format
                           + "'",
                           "Time::from_string");
  }
}

//----------------------------------------------------------------------------
// Time::unix_time
//----------------------------------------------------------------------------
int32 Time::unix_time()
{
  return CurrentUTime().long_unix();
}

//----------------------------------------------------------------------------
// Time::is_daylight_saving_time
//----------------------------------------------------------------------------
bool Time::is_daylight_saving_time()
{
  #ifdef WIN32
    TIME_ZONE_INFORMATION lpTimeZoneInformation;
    GetTimeZoneInformation(&lpTimeZoneInformation);
    if( lpTimeZoneInformation.Bias == lpTimeZoneInformation.StandardBias + lpTimeZoneInformation.DaylightBias )
      return true;
    return false;
  #else
    long lTm;
    struct timeval tv;
    struct timezone tzp;
    gettimeofday(&tv, &tzp);
    lTm = tv.tv_sec;

    // Convert from 'time_t' format to 'struct tm' format
    struct tm tmCurrent;
    localtime_r(&lTm, &tmCurrent);

    if( tmCurrent.tm_isdst > 0 )
      return true;
    return false;
  #endif
}

//----------------------------------------------------------------------------
// Time::microsecs
//----------------------------------------------------------------------------
int64 Time::microsecs()
{
  return CurrentTime().raw_value();
}

//----------------------------------------------------------------------------
// Time::get_month_from_name
//----------------------------------------------------------------------------
uint8 Time::get_month_from_name(const std::string& month_name)
{
  for( std::size_t i = 0; i < 12; ++i )
  {
    if( yat::StringUtil::is_equal_no_case(month_name, s_pszMonthEn[i]) )
      return i + 1;
    if( yat::StringUtil::is_equal_no_case(month_name, s_pszMonthEnAbbr[i]) )
      return i + 1;
  }
  std::ostringstream oss;
  oss << "Bad month name: " << month_name;
  throw yat::Exception("ERROR", oss.str(), "Time::get_month_from_name");
}

//===========================================================================
// CurrentTime
//===========================================================================
//----------------------------------------------------------------------------
// CurrentTime::CurrentTime
//----------------------------------------------------------------------------
CurrentTime::CurrentTime(bool bUT) : Time()
{
  set_current(bUT);
}

//===========================================================================
// CurrentUTime
//===========================================================================
//----------------------------------------------------------------------------
// CurrentUTime::CurrentUTime
//----------------------------------------------------------------------------
CurrentUTime::CurrentUTime() : CurrentTime(true)
{
}

//===========================================================================
// LocalTime
//===========================================================================
//----------------------------------------------------------------------------
// LocalTime::LocalTime
//----------------------------------------------------------------------------
LocalTime::LocalTime() : Time()
{
  set_local(0,0,0);
}

//----------------------------------------------------------------------------
// LocalTime::LocalTime
//----------------------------------------------------------------------------
LocalTime::LocalTime(int16 iYear, uint8 uiMonth, uint8 uiDay, uint8 uiHour, uint8 uiMin, double dSec)
: Time()
{
  set_local(iYear, uiMonth, uiDay, uiHour, uiMin, dSec);
}

//===========================================================================
// DurationFields
//===========================================================================
void DurationFields::clear()
{
  days = hours = minutes = seconds = micros = 0;
}

//===========================================================================
// Duration
//===========================================================================

//----------------------------------------------------------------------------
// Duration::Duration
//----------------------------------------------------------------------------
Duration::Duration(const yat::Time& t1, const yat::Time& t2)
{
  if( t1.raw_value() > t2.raw_value() )
    m_microsecs = t1.raw_value() - t2.raw_value();
  else
    m_microsecs = t2.raw_value() - t1.raw_value();
}

//----------------------------------------------------------------------------
// Duration::operator=
//----------------------------------------------------------------------------
Duration& Duration::operator=(const Duration& other)
{
  m_microsecs = other.m_microsecs;
  return *this;
}

//----------------------------------------------------------------------------
// Duration::operator+=
//----------------------------------------------------------------------------
Duration& Duration::operator+=(yat::int64 microsecs)
{
  m_microsecs += microsecs;
  return *this;
}

//----------------------------------------------------------------------------
// Duration::operator+=
//----------------------------------------------------------------------------
Duration& Duration::operator+=(const Duration& other)
{
  m_microsecs += other.m_microsecs;
  return *this;
}

//----------------------------------------------------------------------------
// Duration::operator-=
//----------------------------------------------------------------------------
Duration& Duration::operator-=(yat::int64 microsecs)
{
  if( m_microsecs > microsecs )
    m_microsecs -= microsecs;
  else
    m_microsecs = 0;
  return *this;
}

//----------------------------------------------------------------------------
// Duration::operator-=
//----------------------------------------------------------------------------
Duration& Duration::operator-=(const Duration& other)
{
  if( m_microsecs > other.m_microsecs )
    m_microsecs -= other.m_microsecs;
  else
    m_microsecs = 0;
  return *this;
}

//----------------------------------------------------------------------------
// Duration::operator+
//----------------------------------------------------------------------------
Duration Duration::operator+(const Duration& other)
{
  Duration d;
  d.m_microsecs = m_microsecs + other.m_microsecs;
  return d;
}

//----------------------------------------------------------------------------
// Duration::operator+
//----------------------------------------------------------------------------
Duration Duration::operator+(yat::int64 microsecs)
{
  Duration d;
  d.m_microsecs = m_microsecs + microsecs;
  return d;
}

//----------------------------------------------------------------------------
// Duration::operator-
//----------------------------------------------------------------------------
Duration Duration::operator-(const Duration& other)
{
  Duration d;
  if( m_microsecs > other.m_microsecs )
    d.m_microsecs = m_microsecs - other.m_microsecs;
  return d;
}

//----------------------------------------------------------------------------
// Duration::operator-
//----------------------------------------------------------------------------
Duration Duration::operator-(yat::int64 microsecs)
{
  Duration d;
  if( m_microsecs > microsecs )
    d.m_microsecs = m_microsecs - microsecs;
  return d;
}

//----------------------------------------------------------------------------
// Duration::operator*=
//----------------------------------------------------------------------------
Duration& Duration::operator*=(int n)
{
  if( n >= 0 )
    m_microsecs *= n;
  return *this;
}

//----------------------------------------------------------------------------
// Duration::operator/=
//----------------------------------------------------------------------------
Duration& Duration::operator/=(int n)
{
  if( n > 0 )
    m_microsecs /= n;
  return *this;
}

//----------------------------------------------------------------------------
// Duration::operator*=
//----------------------------------------------------------------------------
Duration& Duration::operator*=(double factor)
{
  if( factor >= 0 )
    m_microsecs = yat::int64( double(m_microsecs) * factor );
  return *this;
}

//----------------------------------------------------------------------------
// Duration::operator/=
//----------------------------------------------------------------------------
Duration& Duration::operator/=(double factor)
{
  if( factor > 0 )
    m_microsecs = yat::int64( double(m_microsecs) / factor );
  return *this;
}

//----------------------------------------------------------------------------
// Duration::operator*
//----------------------------------------------------------------------------
Duration Duration::operator*(int n)
{
  Duration d;
  if( n >= 0 )
    d.m_microsecs = m_microsecs * n;
  else
    d.m_microsecs = m_microsecs;
  return d;
}

//----------------------------------------------------------------------------
// Duration::operator/
//----------------------------------------------------------------------------
Duration Duration::operator/(int n)
{
  Duration d;
  if( n > 0 )
    d.m_microsecs = m_microsecs / n;
  else
    d.m_microsecs = m_microsecs;
  return d;
}

//----------------------------------------------------------------------------
// Duration::operator*
//----------------------------------------------------------------------------
Duration Duration::operator*(double factor)
{
  Duration d;
  if( factor >= 0 )
    d.m_microsecs = yat::int64(double(m_microsecs) * factor);
  else
    d.m_microsecs = m_microsecs;
  return d;
}

//----------------------------------------------------------------------------
// Duration::operator/
//----------------------------------------------------------------------------
Duration Duration::operator/(double factor)
{
  Duration d;
  if( factor > 0 )
    d.m_microsecs = yat::int64(double(m_microsecs) / factor);
  else
    d.m_microsecs = m_microsecs;
  return d;
}

//----------------------------------------------------------------------------
// Duration::get
//----------------------------------------------------------------------------
void Duration::get(DurationFields *pDF) const
{
  pDF->days = static_cast<yat::uint8>((m_microsecs / MICROSEC_PER_SEC) / SEC_PER_DAY);
  // Hour, minutes, seconds
  uint32 ulSec = uint32((m_microsecs / int64(MICROSEC_PER_SEC)) % int64(SEC_PER_DAY));
  pDF->hours = (uint8)(ulSec / SEC_PER_HOUR);
  ulSec -= pDF->hours * SEC_PER_HOUR;
  pDF->minutes = (uint8)(ulSec / SEC_PER_MIN);
  pDF->seconds = (uint8)(ulSec - pDF->minutes * SEC_PER_MIN);
  // Retreive micro-seconds
  pDF->micros = m_microsecs % MICROSEC_PER_SEC;
}

//----------------------------------------------------------------------------
// Duration::days
//----------------------------------------------------------------------------
uint16 Duration::days() const
{
  DurationFields df;
  get(&df);
  return df.days;
}

//----------------------------------------------------------------------------
// Duration::hours
//----------------------------------------------------------------------------
uint8 Duration::hours() const
{
  DurationFields df;
  get(&df);
  return df.hours;
}

//----------------------------------------------------------------------------
// Duration::minutes
//----------------------------------------------------------------------------
uint8 Duration::minutes() const
{
  DurationFields df;
  get(&df);
  return df.minutes;
}

//----------------------------------------------------------------------------
// Duration::seconds
//----------------------------------------------------------------------------
uint8 Duration::seconds() const
{
  DurationFields df;
  get(&df);
  return df.seconds;
}

//----------------------------------------------------------------------------
// Duration::millis
//----------------------------------------------------------------------------
uint16 Duration::millis() const
{
  DurationFields df;
  get(&df);
  return df.micros / 1000;
}

//----------------------------------------------------------------------------
// Duration::micros
//----------------------------------------------------------------------------
uint32 Duration::micros() const
{
  DurationFields df;
  get(&df);
  return df.micros;
}

//----------------------------------------------------------------------------
// Duration::total_secs
//----------------------------------------------------------------------------
double Duration::total_secs() const
{
  return (double)m_microsecs / MICROSEC_PER_SEC;
}

//----------------------------------------------------------------------------
// Duration::to_string_millis
//----------------------------------------------------------------------------
std::string Duration::to_string_millis(char sep) const
{
  DurationFields df;
  get(&df);
  std::string duration;

  if(  df.days > 0 )
    yat::StringUtil::printf(&duration, "%dd%c%02dh%c%02dm%c%02ds.%03d",
                                       df.days, sep, df.hours, sep, df.minutes, sep, df.seconds, df.micros / 1000);
  else if( df.hours > 0 )
    yat::StringUtil::printf(&duration, "%02dh%c%02dm%c%02ds.%03d",
                                       df.hours, sep, df.minutes, sep, df.seconds,  df.micros / 1000 );
  else if( df.minutes > 0 )
    yat::StringUtil::printf(&duration, "%02dm%c%02ds.%03d",
                                       df.minutes, sep, df.seconds,  df.micros / 1000 );
  else
    yat::StringUtil::printf(&duration, "%02ds.%03d",
                                       df.seconds,  df.micros / 1000 );

  return duration;
}

//----------------------------------------------------------------------------
// Duration::to_string
//----------------------------------------------------------------------------
std::string Duration::to_string(char sep) const
{
  DurationFields df;
  get(&df);
  std::string duration;

  if(  df.days > 0 )
    yat::StringUtil::printf(&duration, "%dd%c%02dh%c%02dm%c%02ds",
                                       df.days, sep, df.hours, sep, df.minutes, sep, df.seconds);
  else if( df.hours > 0 )
    yat::StringUtil::printf(&duration, "%02dh%c%02dm%c%02ds",
                                       df.hours, sep, df.minutes, sep, df.seconds);
  else if( df.minutes > 0 )
    yat::StringUtil::printf(&duration, "%02dm%c%02ds",
                                       df.minutes, sep, df.seconds);
  else
    yat::StringUtil::printf(&duration, "%02ds",
                                       df.seconds);

  return duration;
}

//----------------------------------------------------------------------------
// Duration::to_iso8601
//----------------------------------------------------------------------------
std::string Duration::to_iso8601() const
{
  DurationFields df;
  get(&df);
  std::string duration;

  if(  df.days > 0 )
    yat::StringUtil::printf(&duration, "%02dDT%02dH%02dM%02dS",
                                       df.days, df.hours, df.minutes, df.seconds );
  else if( df.hours > 0 )
    yat::StringUtil::printf(&duration, "T%02dH%02dM%02dS",
                                       df.hours, df.minutes, df.seconds );
  else if( df.minutes > 0 )
    yat::StringUtil::printf(&duration, "T%02dM%02dS",
                                       df.minutes, df.seconds );
  else
    yat::StringUtil::printf(&duration, "T%02dS",
                                       df.seconds );

  return duration;
}

}
