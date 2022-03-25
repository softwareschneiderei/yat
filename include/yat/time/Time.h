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

#ifndef __YAT_TIME_H__
#define __YAT_TIME_H__

// ============================================================================
//! \page TimePage Time documentation
//! \tableofcontents
//! The time utilities provide basic date manipulation functions and an implementation
//! of the timer & timeout concepts.
//!
//! \section secT1 Time utilities
//! The time utilities provide basic date manipulation functions such as:
//!  - various date format ("time" fields, microsecond precision, Julian day, Unix time),
//!  - conversion functions (Julian/Gregorian/Unix, UTC/local time),
//!  - output format (ISO8601, international format),
//!  - accessors & mutators,
//!  - comparison functions,
//!  - usual date & time constants.
//!
//! The time utilities also provide an implementation of the timer (chronometer) and
//! timeout (deadline) concepts.
//!
//! \section secT2 Time classes
//! Links to time structures & classes :
//!   - yat::DateFields
//!   - yat::Duration
//!   - yat::Time
//!   - yat::UnixTime
//!   - yat::CurrentTime
//!   - yat::CurrentUTime
//!   - yat::Timer
//!   - yat::Timeout
// ============================================================================

#include <yat/CommonHeader.h>
#include <yat/utils/String.h>

namespace yat
{

//! \brief Makes int64 from the long integers pair [higher part, lower part].
//! \param lHigh High part.
//! \param ulLow Low part.
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
//! Number of seconds per minute.
#define SEC_PER_MIN         60L
//! Number of seconds per hour.
#define SEC_PER_HOUR        3600L
//! Number of seconds per day.
#define SEC_PER_DAY         86400L
//! Number of seconds per month (30 days).
#define SEC_PER_MONTH       (SEC_PER_DAY*30)     // logical month (30 days)
//! Number of seconds per year (12 months).
#define SEC_PER_YEAR        (SEC_PER_DAY*30*12)  // logical year (12 logicals months)

#ifndef MS_SEC
//! Number of milliseconds per second.
  #define MS_SEC              1000L
#endif
//! Number of microseconds per second.
#define MICROSEC_PER_SEC      1000000L

//! Number of microseconds per day - High part.
#define MICROSEC_PER_DAY_H    20L
//! Number of microseconds per day - Low part.
#define MICROSEC_PER_DAY_L    500654080UL
//! Number of microseconds per day.
#define MICROSEC_PER_DAY      int64FromHLPair(MICROSEC_PER_DAY_H, MICROSEC_PER_DAY_L) // microseconds per day

#ifndef MS_OVERFLOW
  //! \brief Number of milliseconds indicating an int64 capacity overflow.
  //! Used in a date substraction.
  #define MS_OVERFLOW  (int64FromHLPair(0x80000000, 0x0) - int64(1))
#endif

//! 1970/01/01 at 00:00.
#define REF_UNIX        int64FromHLPair(0x2ed263d, 0x83a88000)


// Const for day, month and unit names.
// - 4 possible combinations for days/months and 6 possible combinations for units
// with plural possibilities.
// - ABBR, LONG, LONGPL, OTHERS are exclusive terms; INTER can be added.
//! Const for day, month and unit names: abbreviation.
#define TM_ABBR     0 // pas vraiment utile
//! Const for day, month and unit names: international.
#define TM_INTER    1
//! Const for day, month and unit names: long.
#define TM_LONG     2
//! Const for day, month and unit names: plural long (for durations).
#define TM_LONGPL   4
//! Const for day, month and unit names: other abbreviations (for parsing function, with a | separator).
#define TM_OTHERS   6
//! Const for day, month and unit names: default unit (for durations).
#define TM_DEFAULT 16

// Unit identifiers.
//! Second identifier.
#define TS_UNIT_SEC   0
//! Minute identifier.
#define TS_UNIT_MIN   1
//! Hour identifier.
#define TS_UNIT_HOUR  2
//! Day identifier.
#define TS_UNIT_DAY   3
//! Month identifier.
#define TS_UNIT_MONTH 4  // mois logique (=30 jours)
//! year identifier.
#define TS_UNIT_YEAR  5  // annee logique (=12 mois logiques)

// ============================================================================
//! \struct DateFields
//! \brief %Date split in fields.
//!
//! This structure provides a date split in "time fields", i.e. year, day, month,
//! hour,... to microseconds precision.
// ============================================================================
struct YAT_DECL DateFields
{
  //! Year.
  int16  year;
  //! Month.
  uint8  month;
  //! Day.
  uint8  day;
  //! Hour.
  uint8  hour;
  //! Minute.
  uint8  min;
  //! Second (with microsecond precision).
  double sec;
  //! Day of year (1-366).
  uint16 day_of_year;
  //! Day of week (1=Monday, 7=Sunday).
  uint8  day_of_week;
  //! Week of year (1-53).
  uint8  week_of_year;
  //! is local time
  bool local_time;
  //! Time zone bias in minutes
  int tz_bias;

  //! c-tor
  DateFields() { clear(); }

  //! \brief Clears all fields.
  void clear();

  //! \brief Returns true if date is null, false otherwise.
  int is_empty() const;
};

// ============================================================================
//! \class Time
//! \brief The YAT time class.
//!
//! This class represents a specific instant in time (date & time), with a microsecond precision.
//! It provides various date manipulation functions (accessors & mutators, format conversions,
//! comparison operators...).
//! \remark This class uses the Julian day system, which takes its reference at
//! -4712/01/01 12:00.
// ============================================================================
class YAT_DECL Time
{
private:
  int64 m_llTime;         // Complete date with microsecond precision
  bool  m_local_time;     // true if time is local
  mutable int m_tz_bias;  // Time zone bias in minutes (..., -120, -60, 0, 60, 120...)
                          // -1 if not specified

  void set_tz(const std::string& tz);

public:

  //! Standard date/time output formats
  //@{

  //! Obsolete format
  //! 2020-04-07 11:50:32
  static const std::string date_inter;

  //! standard iso8601 string.
  //! 2020-04-07T11:50:32
  static const std::string iso8601;

  //! Date only
  //! 2020-04-07
  static const std::string iso8601_d;

  //! Time only
  //! 11:50:32
  static const std::string iso8601_t;

  //! Time only with time zone bias
  //! 11:50:32+0200
  static const std::string iso8601_tz;

  //! full iso8601 string
  //! 2020-04-07T11:50:32+0200
  static const std::string iso8601_z;

  //@}

  //! Standard seconds output precision to be used in 'to_string' family methods
  static const unsigned short millisec;
  static const unsigned short microsec;

  //! \brief Default constructor.
  Time() { m_llTime = 0; m_local_time = false; m_tz_bias = -1; }

  //! \brief Constructor from time fields.
  //!
  //! \param iYear Year from -4712 to ?
  //! \param uiMonth Month in range [1, 12].
  //! \param uiDay Day in range [1, 31].
  //! \param uiHour Hour in range [0, 23].
  //! \param uiMin Minute in range [0, 59].
  //! \param dSec Seconds in range [0, 59] with microsecond precision.
  Time(int16 iYear, uint8 uiMonth, uint8 uiDay, uint8 uiHour=0, uint8 uiMin=0, double dSec=0);

  //! \brief Constructor from string.
  //! \see to_string
  Time(const std::string& date_time, const std::string& format);

  //-----------------------------------------------------------------
  //! \name Accessors
  //@{

  //! \brief Returns the date's internal value.
  int64 raw_value() const { return m_llTime; }

  //! \brief Sets the date's internal value.
  //! \param i Complete date with microsecond precision (from Julian reference).
  void set_raw_value(int64 i) { m_llTime = i; }

  //! \brief Returns true if the date is empty (null value).
  bool is_empty() const { return m_llTime == 0; }

  //! \brief Returns true if the date is 1970/01/01 00:00.
  int is_empty_unix() const { return REF_UNIX == m_llTime; }

  //! \brief Splits the date in "time fields".
  //! \param pTm Structure to fill in.
  void get(DateFields *pTm) const;

  //! \brief Splits the date in "time fields".
  void get_local(DateFields *df_p) const;
  void get_utc(DateFields *df_p) const;

  //! \brief Returns the microsecond part.
  long micro() const;

  //! \brief Returns the millisecond part.
  long ms() const;

  //! \brief Returns the second part with microsecond precision.
  double second() const;

  //! \brief Returns the minute part.
  uint8 minute() const;

  //! \brief Returns the hour part.
  uint8 hour() const;

  //! \brief Returns the Day part
  uint8 day() const;

  //! \brief Returns the month part
  uint8 month() const;

  //! \brief Returns the year part.
  int16 year() const;

  //! \brief Returns the day number of the week.
  //!
  //! Starts from monday (1) to sunday (7).
  uint8 day_of_week() const;

  //! \brief Returns the day number in the range [1, 366].
  uint16 day_of_year() const;

  //! \brief Returns the week number in the range [1, 53].
  uint8 week_of_year() const;

  //! \brief Gets Julian day at 12:00.
  long julian_date() const;

  //! \brief Gets real Julian day taking the time field into account.
  double julian_day() const;

  //! \brief Is local time
  bool local_time() const { return m_local_time; }

  //! \brief Is coordinated universal time (UTC)
  bool utc() const { return !m_local_time; }

  //! \brief Time zone bias in minutes
  int time_zone_bias() const;

  //@} Accessors

  //-----------------------------------------------------------------
  //! \name Setting date
  //@{

  //! \brief Empties the date (sets date to null value).
  void set_empty() { m_llTime = 0; }

  //! \brief Empties the date in Unix sense (sets to Unix reference value).
  void set_empty_unix() { m_llTime = REF_UNIX; }

  //! \brief Initializes the date from DateField value.
  //!
  //! \param sTm Structure containing split date.
  //! \exception BAD_ARGS Thrown if date is not correct.
  void set(const DateFields& sTm);

  //! \brief Initializes universal time (UT) from explicit values.
  //!
  //! \param iYear Year from -4712 to ?
  //! \param uiMonth Month in range [1, 12].
  //! \param uiDay Day in range [1, 31].
  //! \param uiHour Hour in range [0, 23].
  //! \param uiMin Minute in range [0, 59].
  //! \param dSec Seconds in range [0, 59] with microsecond precision.
  void set(int16 iYear, uint8 uiMonth, uint8 uiDay,
           uint8 uiHour=0, uint8 uiMin=0, double dSec=0);

  //! \brief Initializes the local time from explicit values.
  void set_local(int16 iYear, uint8 uiMonth, uint8 uiDay,
           uint8 uiHour=0, uint8 uiMin=0, double dSec=0);

  //! \brief Initializes the universal time from explicit values.
  //! This method has the same effect as the method 'set'
  void set_utc(int16 iYear, uint8 uiMonth, uint8 uiDay,
           uint8 uiHour=0, uint8 uiMin=0, double dSec=0);

  //! \brief Initializes the date from the number of the day in the specified year.
  //!
  //! Clears the time part of the date.
  //! \param iDayOfYear Number of the day in year.
  //! \param iYear Year value.
  void set_day_of_year(uint16 iDayOfYear, int16 iYear);

  //! \brief Sets the second part with microsecond precision.
  //! \param dSec Seconds in range [0, 59] with microsecond precision.
  void set_second(double dSec);

  //! \brief Sets the minute part.
  //! \param uiMin Minute in range [0, 59].
  void set_minute(uint8 uiMin);

  //! \brief Sets the hour part.
  //! \param uiHour Hour in range [0, 23].
  void set_hour(uint8 uiHour);

  //! \brief Sets the day part.
  //! \param uiDay Day in range [1, 31].
  void set_day(uint8 uiDay);

  //! \brief Sets the month part.
  //! \param uiMonth Month in range [1, 12].
  void set_month(uint8 uiMonth);

  //! \brief Sets the month part as string.
  //! \param uiMonth Month in range [1, 12].
  void set_month(const std::string& month_name);

  //! \brief Sets the year part.
  //! \param iYear Year value.
  void set_year(int16 iYear);

  //! \brief Sets the Julian date.
  //! \param lJulianDate Julian date (in number of days from Julian reference).
  void set_julian_date(long lJulianDate);

  //! \brief Sets date internal value.
  //! \param ui64 Complete date with microsecond precision (from Julian reference).
  void set(int64 ui64) { m_llTime = ui64; }

  //! \brief force the time zone bias
  //! \param minutes bias in minutes
  void set_time_zone_bias(int minutes);

  //! \brief Clears the time part (hour, min, sec).
  void clear_time();

  //! \brief Clears the date part (year, month, day).
  void clear_date();

  //! \brief Initializes the date from current time.
  //! \param utc If set to true, uses coordinated universal time (UTC) reference,
  //! otherwise uses local time reference.
  Time& set_current(bool utc=false);

  //! \brief Initializes the date from current UTC time.
  Time& set_current_utc();

  //! \brief Initializes the date from current local time.
  Time& set_current_local();

  //! \brief Initializes the date from current time.
  //!        Take care of utc/local time status
  Time& now();

  //! \brief Converts the date from local time to universal time (UTC).
  Time& to_utc();

  //! \brief Converts the date from universal time (UTC) to local time.
  Time& to_local();

  //! \brief Adds seconds to the date.
  //! \param dSec Number of seconds to add.
  void add_sec(double dSec) { m_llTime += int64(dSec * 1e6); }

  //! \brief operator+=.
  //!
  //! Adds seconds to the date.
  //! \param dSec Number of seconds to add.
  void operator +=(double dSec) { add_sec(dSec); }

  //! \brief operator-=.
  //!
  //! Subtracts seconds to the date.
  //! \param dSec Number of seconds to subtract.
  void operator -=(double dSec) { add_sec(-dSec); }

  //@}

  //-----------------------------------------------------------------
  //! \name Text methods
  //@{

  //! \brief Initializes date from a string.
  //!
  //! \param format date-time format
  //!        ex: %Y-%M-%DT%h:%m:%s.%ms for a ISO8601 string
  //!          %a 	Abbreviated weekday name (ex: Sun)
  //!          %A 	Full weekday name (ex: Sunday)
  //!          %b 	Abbreviated month name (ex: Mar)
  //!          %B 	Full month name (ex: March)
  //!          %d 	Day of the month (01-31)
  //!          %h   Equivalent to %b.
  //!          %H 	Hour in 24h format (00-23)
  //!          %j 	Day of the year (001-366)
  //!          %m 	Month as a decimal number (01-12)
  //!          %M 	Minute (00-59)
  //!          %S 	Second (00-60)
  //!          %<n> Expected seconds precision in number of digits after dot (0-6)
  //!          %y   Year, last two digits (00-99)
  //!          %Y 	Year
  void from_string(const std::string& date_time, const std::string& format);

  //! Format date/time according to the given format
  //! \see to_string(const DateFields& df, const std::string& format, unsigned short precision=0)
  yat::String to_string(const std::string& format, unsigned short precision=0) const;

  //! Format convert to local if necessary & format date according to the given format
  //! \see to_string
  yat::String to_string_local(const std::string& format, unsigned short precision=0) const ;

  //! Format convert to utc if necessary & format date according to the given format
  //! \see to_string
  yat::String to_string_utc(const std::string& format, unsigned short precision=0) const;

  //! \brief Gets the date in the international format.
  //!
  //! \param bMillis If set to true, the output format is with milliseconds.
  std::string to_inter(bool bMillis=true) const;

  //@} Text methods


  //-----------------------------------------------------------------
  //! \name UNIX reference
  //
  //! \note UNIX dates start from 1970/01/01. So, theses methods
  //! are convenient methods to set and get a number of seconds since
  //! the UNIX reference.
  //@{


  //! \brief Gets *this* utc date as a number of seconds since 1970/01/01 00:00.
  //!
  //! Returns an integer value.
  long long_unix() const;

  //! \brief Initializes utc date from a number of seconds since 1970/01/01 00:00.
  //! \param lRefSec Number of seconds (integer value).
  void set_long_unix(long lRefSec);

  //! \brief Gets *this* date as a number of seconds since 1970/01/01 00:00 with microseconds precision.
  //!
  //! Returns a double value.
  double double_unix() const;

  //! \brief Initializes date from a number seconds since 1970/01/01 00:00 with microseconds precision.
  //! \param dRefSec Number of seconds (double value).
  void set_double_unix(double dRefSec);

  //@} UNIX reference

  //-----------------------------------------------------------------
  //! \name Compare operators
  //@{

  //! \brief operator==.
  //! \param tm The source date.
  bool operator ==(const Time& tm) const
    { return m_llTime == tm.m_llTime; }

  //! \brief operator>.
  //! \param tm The source date.
  bool operator >(const Time& tm) const
    { return m_llTime > tm.m_llTime; }

  //! \brief operator>=.
  //! \param tm The source date.
  bool operator >=(const Time& tm) const
    { return m_llTime >= tm.m_llTime; }

  //! \brief operator<.
  //! \param tm The source date.
  bool operator <(const Time& tm) const
    { return m_llTime < tm.m_llTime; }

  //! \brief operator<=.
  //! \param tm The source date.
  bool operator <=(const Time& tm)  const
    { return m_llTime <= tm.m_llTime; }

  //! \brief operator!=.
  //! \param tm The source date.
  bool operator !=(const Time& tm) const
    { return m_llTime != tm.m_llTime; }

  //@} Compare operators

  //-----------------------------------------------------------------
  //! \name Static methods
  //@{

  //! \brief Format date according to the given format
  //! \param format The format
  //! \param precision modify the format field '%S' specification to
  //!        the requested precision: 0 (second) to 6 (microseconds)
  //!        one can also use yat::Time::millisec for millisecond precision and
  //!        yat::Time::microsec for microsecond precision
  //!
  //! Supported format specifications are
  //!          %a   Abbreviated weekday name            Sun
  //!          %A   Full weekday name                   Sunday
  //!          %b   Abbreviated month name              Mar
  //!          %h   Abbreviated month name              Mar
  //!          %B   Full month name                     March
  //!          %C   The century number (year/100) as a 2-digit integer.
  //!          %d   Day of the month (01-31)            19
  //!          %e   Like %d, the day of the month as a decimal number, but
  //!               a leading zero is replaced by a space.
  //!          %H   Hour in 24h format (00-23)          14
  //!          %j   Day of the year (001-366)           231
  //!          %m   Month as a decimal number (01-12)   08
  //!          %M   Minute (00-59)                      55
  //!          %R   Equivalent to %H:%M
  //!          %s   The number of seconds since the Epoch, that is, since 1970-01-01 00:00:00 UTC
  //!          %S   Second (00-60)                      02
  //!          %t   A tab character
  //!          %T   The time in 24-hour notation (%H:%M:%S)
  //!          %u   The day of the week as a decimal, range 1 to 7, Monday being 1
  //!          %U   The week number of the current year as a decimal number, range 00 to 53
  //!          %w   The day of the week as a decimal, range 0 to 6, Sunday being 0
  //!          %y   Year, last two digits (00-99)       01
  //!          %Y   Year                                2012
  //!          %%   A literal '%' character.
  static yat::String to_string(const DateFields& df, const std::string& format, unsigned short precision=0);

  //! \brief Time zone bias returned by the operating system in minutes
  static int sys_time_zone_bias();

  //! \brief Number of days for a given month.
  //! \param iMonth Month number.
  //! \param iYear Year value.
  static uint8 nb_days_in_month(uint8 iMonth, int16 iYear);

  //! \brief Number of days in a given year.
  //! \param iYear Year value.
  static uint16 nb_days_in_year(int16 iYear);

  //! \brief Month name.
  //! \param iMonth Month number.
  static pcsz month_name(uint8 iMonth, bool long_name = false);

  //! \brief Day name.
  //! \param day day number.
  static pcsz day_name(uint8 day, bool long_name = false);

  //! \brief Gets Unix current time.
  static int32 unix_time();

  //! \brief Gets system time in microseconds
  static int64 microsecs();

  //! \brief return true if daylight saving time (summer time in british english) is effective
  //! at the moment
  static bool is_daylight_saving_time();

  //! \brief return month index
  //!
  //! \param month_name
  static uint8 get_month_from_name(const std::string& month_name);

  //! \brief get current date time fields without Time object
  //!
  //! Be aware that using those methods the following fields are not set:
  //! - week_of_year & day_of_year on Windows plateform
  //! - week_of_year on Linux plateform
  //!
  //! \param df_p DateFields struct
  static void get_current_local(DateFields* df_p);
  static void get_current_utc(DateFields* df_p);

  //@} Static methods

  //-----------------------------------------------------------------
  //! \name Deprecated methods
  //@{

  //! \deprecated use to_string_utc(yat::Time::iso8601, yat::Time::millisec) instead
  std::string to_ISO8601_ms_TU() const;
  //! \deprecated use to_string_utc(yat::Time::iso8601, yat::Time::microsec) instead
  std::string to_ISO8601_micro_TU() const;
  //! \deprecated use to_string_local(yat::Time::iso8601)
  std::string to_local_ISO8601() const;
  //! \deprecated use to_string_local(yat::Time::iso8601, yat::Time::millisec)
  std::string to_local_ISO8601_ms() const;
  //! \deprecated use to_string_local(yat::Time::iso8601, yat::Time::microsec)
  std::string to_local_ISO8601_micro() const;
  //! \deprecated use to_string(yat::Time::iso8601)
  std::string to_ISO8601() const;
  //! \deprecated use to_string(yat::Time::iso8601, yat::Time::millisec)
  std::string to_ISO8601_ms() const;
  //! \deprecated use to_string(yat::Time::iso8601, yat::Time::microsec)
  std::string to_ISO8601_micro() const;
  //! \deprecated use to_utc()
  void local_to_UT();
  //! \deprecated use to_local()
  void UT_to_local();


  //@} Deprecated methods
};

//- Create synonyms for convenience.
typedef Time Date;
typedef Time DateTime;

// ============================================================================
//! \class CurrentTime
//! \brief Current date class.
//!
//! This class is a Time class initialized with local current date and time.
//! Inherits from Time class.
// ============================================================================
class YAT_DECL CurrentTime : public Time
{
public:
  //! \brief Constructor.
  //!
  //! \param bUT If set to true, the date is initialized with Universal %Time instead
  //! of local time.
  CurrentTime(bool utc=false);
};

// ============================================================================
//! \class UnixTime
//! \brief data based on unix time value.
//!
//! This class is a Time class initialized with a UNIX time value.
//! Inherits from Time class.
// ============================================================================
class YAT_DECL UnixTime : public Time
{
public:
  //! \brief Constructor.
  //!
  //! \param bUT If set to true, the date is initialized with Universal %Time instead
  //! of local time.
  UnixTime(double unix_time = 0.) : Time()
  {
    set_double_unix(unix_time);
  }
};

// ============================================================================
//! \class CurrentUTime
//! \brief Current time class initialized at current Coordinated Universal Time (UTC).
//!
//! Inherits from CurrentTime class.
// ============================================================================
class YAT_DECL CurrentUTime : public CurrentTime
{
public:
  //! \brief Constructor.
  //!
  CurrentUTime();
};

// ============================================================================
//! \class LocalTime
//! \brief Time class initialized with local time flag set
//!
//! Inherits from Time class.
// ============================================================================
class YAT_DECL LocalTime : public Time
{
public:
  //! \brief Constructor.
  //!
  LocalTime();

  //! \brief Constructor from time fields.
  //!
  //! \param iYear Year from -4712 to ?
  //! \param uiMonth Month in range [1, 12].
  //! \param uiDay Day in range [1, 31].
  //! \param uiHour Hour in range [0, 23].
  //! \param uiMin Minute in range [0, 59].
  //! \param dSec Seconds in range [0, 59] with microsecond precision.
  LocalTime(int16 iYear, uint8 uiMonth, uint8 uiDay, uint8 uiHour=0, uint8 uiMin=0, double dSec=0);
};

//- Create synonym for convenience.
typedef CurrentTime CurrentDate;
typedef CurrentTime CurrentDateTime;

// ============================================================================
//! \struct DurationFields
//! \brief %Date split in fields.
//!
//! This structure provides a duration split in "duration fields", i.e. days,
//! hours, minutes, seconds... to microseconds precision.
// ============================================================================
struct YAT_DECL DurationFields
{
  //! nb Day.
  uint8  days;
  //! Hour.
  uint8  hours;
  //! Minute.
  uint8  minutes;
  //! Second
  uint8  seconds;
  //! microsecs
  uint32 micros;

  //! \brief Clears all fields.
  void clear();
};

// ============================================================================
//! \class Duration
//! \brief duration class.
//! \note  complete the implementation !!!
//! \note add some methods in Time class (operator-, operator+,...)
//! This class hold a duration
// ============================================================================
class YAT_DECL Duration
{

public:
/* DO NOT DELETE, its on progress...
  class Micro
  {
    Micro( yat::int64 micros ) : m_microsecs(micros) {}
  private:
    yat::int64 m_microsecs;
  };

  class Seconds
  {
    Seconds( std::size_t secs) : Micro( secs * 1000000 ) {}
  };
*/
public:
  Duration() : m_microsecs(0) { }

  //! c-tor
  //! \param microsecs duration in microseconds
  //! a negative value will be converted to positive one
  Duration(yat::int64 microsecs) : m_microsecs(abs(microsecs)) { }

  //! c-tor
  //! \param double duration in seconds with microseconds precision
  //! a negative value will be converted to positive one
  Duration(double seconds) : m_microsecs( yat::int64(fabs(seconds) * MICROSEC_PER_SEC) ) { }

  /// Distance between t1 and t2, always positive
  Duration(const yat::Time& t1, const yat::Time& t2);

  /// Set value
  void total_micros(yat::int64 microsecs) { m_microsecs = microsecs; }

  /// Get total value
  int64 total_micros() const { return m_microsecs; }

  /// Get duration fields
  void get(DurationFields *pDF) const;

  /// Assignement operators
  Duration  operator+(const Duration& other);
  Duration  operator-(const Duration& other);
  Duration  operator/(int n);
  Duration  operator*(int n);
  Duration  operator/(double factor);
  Duration  operator*(double factor);
  Duration  operator+(yat::int64 microsecs);
  Duration  operator-(yat::int64 microsecs);
  Duration& operator=(const Duration& other);
  Duration& operator+=(const Duration& other);
  Duration& operator+=(yat::int64 microsecs);
  Duration& operator-=(const Duration& other);
  Duration& operator-=(yat::int64 microsecs);
  Duration& operator*=(int n);
  Duration& operator/=(int n);
  Duration& operator*=(double factor);
  Duration& operator/=(double factor);

  /// Duration fields
  uint16 days() const;
  uint8 hours() const;
  uint8 minutes() const;
  uint8 seconds() const;
  uint16 millis() const;
  uint32 micros() const;

  // duration in seconds
  double total_secs() const;

  ///
  /// Export duration to a string using this format:
  /// <d>d<sep><hh>h<sep><mm>m<sep><ss>s
  /// @param field separator
  /// example: 1d:12h:05m:25s, with sep = ':'
  std::string to_string(char sep=':') const;

  ///
  /// Export duration to a string using this format:
  /// <d>d<sep><hh>h<sep><mm>m<sep><ss>s.micros
  /// @param field separator
  /// example: 1d:12h:05m:25s.123, with sep = ':'
  std::string to_string_millis(char sep=':') const;

  /// Export duration to a string using the ISO8601 format
  /// but limited to day periods:
  /// PnDTnHnMnS
  std::string to_iso8601() const;

private:
  int64 m_microsecs;   // duration in microsecond precision
};

//==================================================================================================
// MeanDuration
//==================================================================================================
class YAT_DECL MeanDuration
{
public:
  MeanDuration(): m_count(0), m_mean() {}

  void add( const Duration& d )
       { ++m_count; m_mean.total_micros( ( m_mean.total_micros() * (m_count - 1) + d.total_micros() ) / m_count ); }
  std::size_t n() const { return m_count; }
  const Duration& get() const { return m_mean; }

private:
  std::size_t m_count;
  Duration    m_mean;
};


} // namespace

#endif
