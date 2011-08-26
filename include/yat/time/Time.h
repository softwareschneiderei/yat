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


#ifndef __YAT_TIME_H__
#define __YAT_TIME_H__

#include <yat/CommonHeader.h>
#include <yat/utils/String.h>

namespace yat
{

/// Make int64 from the long integers pair [highpart, lowpart]
inline int64 int64FromHLPair(long lHigh, unsigned long ulLow) 
{ 
  int64 i64 = 1;
  i64 = i64 << 32;
  i64 = i64 * lHigh + ulLow;
  return i64;
}

//============================================================================
// Constants
//============================================================================

// Usual durations
#define SEC_PER_MIN         60L
#define SEC_PER_HOUR        3600L
#define SEC_PER_DAY         86400L
#define SEC_PER_MONTH       (SEC_PER_DAY*30)     // logical month (30 days)
#define SEC_PER_YEAR        (SEC_PER_DAY*30*12)  // logical year (12 logicals months)

#ifndef MS_SEC
  #define MS_SEC              1000L              // milliseconds per second
#endif
#define MICROSEC_PER_SEC      1000000L           // microseconds per second

#define MICROSEC_PER_DAY_H    20L
#define MICROSEC_PER_DAY_L    500654080UL
#define MICROSEC_PER_DAY      int64FromHLPair(MICROSEC_PER_DAY_H, MICROSEC_PER_DAY_L) // microseconds per day 

#ifndef MS_OVERFLOW
  // Nombre de ms indiquant un depassement de capacite d'un int64 lors du calcul
  // de difference entre deux dates
  #define MS_OVERFLOW	(int64FromHLPair(0x80000000, 0x0) - int64(1))
#endif

// 1970/01/01 at 0h
#define REF_UNIX        int64FromHLPair(0x2ed263d, 0x83a88000)

// Flags pour les noms de jours, mois et unités
// - on a 4 combinaisons pour les jours/mois et 6 pour les unités avec le pluriel
// - ABBR, LONG, LONGPL, OTHERS sont exclusifs ; on peut ajouter inter
#define TM_ABBR     0 // pas vraiment utile
#define TM_INTER    1 // international
#define TM_LONG     2 // long
#define TM_LONGPL   4 // long pluriel (pour les durées)
#define TM_OTHERS   6 // autres abbréviations autorisées en parsing (séparées par |)
#define TM_DEFAULT 16 // unité par défaut (pour les durées)

// identifiant des unités dans le tableau
#define TS_UNIT_SEC   0
#define TS_UNIT_MIN   1
#define TS_UNIT_HOUR  2
#define TS_UNIT_DAY   3
#define TS_UNIT_MONTH 4  // mois logique (=30 jours)
#define TS_UNIT_YEAR  5  // année logique (=12 mois logiques)

// Month names
static const pcsz s_pszMonth[] = 
{
  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

//============================================================================
/// DateFields
/// Date splitted in fields
//============================================================================
struct YAT_DECL DateFields
{
  int16  year;
  uint8  month;
  uint8  day;
  uint8  hour;
  uint8  min;
  double sec;      // Seconds with microsec precision

  uint16 day_of_year;   // Day of year (1-366)
  uint8  day_of_week;   // Day of week (1=lundi, 7=dimanche)
  uint8  week_of_year;  // Week of year (1-53)

  /// Clears all field
  void clear();

  /// Accessor
  int is_empty() const;
};

//===========================================================================
/// The class CDate represents a specific instant in time, with 
/// microsecond precision
//===========================================================================
class YAT_DECL Time
{
private:
  int64 m_llTime;   // Complete date with microsec precision

  // For debugging purpose only
  // NEVER USE THIS METHOD IN REAL CODE !!
  void txt() const;

public:
  /// Constructors
  Time() { m_llTime = 0; }

  /// Constructor from fields
  ///
  /// @param iYear Year from -4712 to ?
  /// @param uiMonth Month in range [1, 12]
  /// @param uiDay Day in range [1, 31]
  /// @param uiHour Hour in range [0, 23]
  /// @param uiMin Minute in range [0, 59]
  /// @param dSec Seconds in range [0, 59] with microsec precision
  /// 
  Time(int16 iYear, uint8 uiMonth, uint8 uiDay, uint8 uiHour=0, uint8 uiMin=0, double dSec=0)
  { set(iYear, uiMonth, uiDay, uiHour, uiMin, dSec); }

  //-----------------------------------------------------------------
  /// @name Accessors
  //@{

  /// Returns data internal value
  int64 raw_value() const { return m_llTime; }
  void set_raw_value(int64 i) { m_llTime = i; }

  /// return 'true' if date is empty
  bool is_empty() const { return m_llTime == 0; }

  /// return 'true' if date is 1970/01/01 0h
  int is_empty_unix() const { return REF_UNIX == m_llTime; }

  /// Splittes date in fields
  ///
  /// @param pTm structure to fill in
  ///
  void get(DateFields *pTm) const;

  /// Returns the microseconds part
  long micro() const;

  /// Returns the milliseconds part
  long ms() const;

  /// Returns the seconds part with microsecond precision
  double second() const;

  /// Returns the minutes part
  uint8 minute() const;

  /// Return the hour part
  uint8 hour() const;

  /// Returns the Day part
  uint8 day() const;

  /// Returns the hour part
  uint8 month() const;

  /// Return the year part
  int16 year() const;

  /// Returns the day number of the week, starting from monday (1) to sunday (7)
  uint8 day_of_week() const;

  /// Returns the day number in the range [1, 366]
  uint16 day_of_year() const;

  /// Returns the week number in the range [1, 53]
  uint8 week_of_year() const;

  /// Gets Julian day at 12h
  long julian_date() const;  // Jour julien de la date a 12h

  /// Gets real Julian day with respect with time
  double julian_day() const;

  //@} Accessors

  //-----------------------------------------------------------------
  /// @name Setting date
  //@{

  /// Empties Date
  void set_empty() { m_llTime = 0; }

  /// Empties Date in unix sense
  void set_empty_unix() { m_llTime = REF_UNIX; }

  /// Initialize date
  ///
  /// @param sTm structure containing splitted date
  ///
  void set(const DateFields& sTm) throw(yat::Exception);

  /// Initializes date from explicit values
  ///
  /// @param iYear Year from -4712 to ?
  /// @param uiMonth Month in range [1, 12]
  /// @param uiDay Day in range [1, 31]
  /// @param uiHour Hour in range [0, 23]
  /// @param uiMin Minute in range [0, 59]
  /// @param dSec Seconds in range [0, 59] with microsec precision
  ///
  void set(int16 iYear, uint8 uiMonth, uint8 uiDay, 
           uint8 uiHour=0, uint8 uiMin=0, double dSec=0);

  // Fixe la partie date depuis le numéro de jour dans l'année / clear le time
  void set_day_of_year(uint16 iDayOfYear, int16 iYear);

  /// Sets the seconds part with microsecond precision
  void set_second(double dSec);

  /// Sets the minute part
  void set_minute(uint8 uiMin);

  /// Sets the hour part
  void set_hour(uint8 uiHour);

  /// Sets the day part
  void set_day(uint8 uiDay);

  /// Sets the month part
  void set_month(uint8 uiMonth);

  /// Sets the year part
  void set_year(int16 iYear);

  /// Sets the Julian date
  void set_julian_date(long lJulianDate);

  /// Sets the internal value
  void set(int64 ui64) { m_llTime = ui64; }

  /// Clears the time part (hour, min, sec)
  void clear_time();

  /// Clears the date part (year, month, day)
  void clear_date();
  
  /// Sets to current time
  /// 
  /// @param bUT if true sets to the universal time (UTC)
  ///        otherwise sets to local time
  void set_current(bool bUT=false);

  /// Convert from local time to universal time (UTC)
  void local_to_UT();

  /// Convert from universal time (UTC) to local time
  void UT_to_local();

  /// Adds seconds
  void add_sec(double dSec) { m_llTime += int64(dSec * 1e6); }
  void operator +=(double dSec) { add_sec(dSec); }
  void operator -=(double dSec) { add_sec(-dSec); }
  
  //@}

  //-----------------------------------------------------------------
  /// @name Text methods
  //@{

  /// Gets date from a ISO8601 string
  void from_ISO8601(const char* pszISO8601);

  /// Gets local time in ISO8601 format
  String to_local_ISO8601() const;

  /// Gets a ISO8601 string with milliseconds
  String to_local_ISO8601_ms() const;

  /// Gets universal time in ISO8601 format
  String to_ISO8601() const;

  /// Gets time with milliseconds in ISO8601 format
  String to_ISO8601_ms() const;

  /// Gets universal time with milliseconds in ISO8601 format
  String to_ISO8601_ms_TU() const;

  /// Gets a string in the international format
  ///
  /// @param bMillis with milliseconds if true
  ///
  String to_inter(bool bMillis=true) const;

  //@} Text methods

  //-----------------------------------------------------------------
  /// @name UNIX reference
  ///
  /// @note UNIX dates start from 1970/01/01. So theses methods
  /// are conveniance methods to set and get a number of seconds since
  /// the UNIX reference
  ///
  //@{
  
  /// Gets a (integer) number of second since 1970/01/01 0h
  long long_unix() const;
  /// Sets the date from a (integer) number of seconds since 1970/01/01 0h
  void set_long_unix(long lRefSec);
  /// Gets a (double) number of seconds since 1970/01/01 0h with microseconds precision
  double double_unix() const;
  /// Sets the date from a (double) number seconds since 1970/01/01 0h with microseconds precision
  void set_double_unix(double dRefSec);

  //@} UNIX reference

  //-----------------------------------------------------------------
  /// @name Compare operators
  //@{

  bool operator ==(const Time& tm) const
    { return m_llTime == tm.m_llTime; }
  bool operator >(const Time& tm) const
    { return m_llTime > tm.m_llTime; }
  bool operator >=(const Time& tm) const
    { return m_llTime >= tm.m_llTime; }
  bool operator <(const Time& tm) const
    { return m_llTime < tm.m_llTime; }
  bool operator <=(const Time& tm)  const
    { return m_llTime <= tm.m_llTime; }
  bool operator !=(const Time& tm) const
    { return m_llTime != tm.m_llTime; }

  //@} Compare operators

  //-----------------------------------------------------------------
  /// @name Static methods
  //@{

  /// Number of days for a given month
  static uint8 nb_days_in_month(uint8 iMonth, int16 iYear);

  /// Number of days in a given year
  static uint16 nb_days_in_year(int16 iYear);

  /// Month name
  static pcsz month_name(uint8 iMonth);

  /// Unix time
  static uint32 unix_time();

  //@} Static methods

};

/// Create synonymes for convenience
typedef Time Date;
typedef Time DateTime;

//===========================================================================
/// CCurrentDate is CDate initialized with current date and time
//===========================================================================
class YAT_DECL CurrentTime : public Time
{
public:
  /// Constructor
  ///
  /// @param bUT if true hte date is initialized with Universal Time instead of local time
  ///
  CurrentTime(bool bUT=false);
};

/// Create synonyme for convenience
typedef CurrentTime CurrentDate;
typedef CurrentTime CurrentDateTime;

} // namespace

#endif
