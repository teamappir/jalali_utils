#include "postgres.h"
#if PG_VERSION_NUM >= 160000
#include "varatt.h"
#endif
#include "parser/scansup.h"
#include "pgtime.h"
#include "nodes/makefuncs.h"
#include "utils/timestamp.h"
#include "utils/datetime.h"
#include "utils/date.h"
#include "utils/builtins.h"

static const int g_days_in_month[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
static const int j_days_in_month[12] = {31, 31, 31, 31, 31, 31, 30, 30, 30, 30, 30, 29};

PG_MODULE_MAGIC;
PG_FUNCTION_INFO_V1(format_jalali_timestamp);
PG_FUNCTION_INFO_V1(format_jalali_date);
PG_FUNCTION_INFO_V1(jalali_part_timestamp);
PG_FUNCTION_INFO_V1(jalali_part_date);

static int floordiv(int a, int b) {
  int q = a / b;
  if ((a % b != 0) && ((a < 0) != (b < 0))) {
    q--;
  }
  return q;
}

static struct pg_tm tm_to_jalali(struct pg_tm *tm){
  struct pg_tm jalali;
  int j_day_no, j_np;
  int g_day_no, i;

  int gy = tm->tm_year + 300;

  g_day_no = 365 * gy + floordiv(gy + 3, 4) - floordiv(gy + 99, 100) + floordiv(gy + 399, 400);

  for (i = 0; i < tm->tm_mon; i++) {
    g_day_no += g_days_in_month[i];
  }

  if (tm->tm_mon > 1 && ((gy % 4 == 0 && gy % 100 != 0) || (gy % 400 == 0))) {
    g_day_no += 1;
  }
  g_day_no += (tm->tm_mday - 1);
  j_day_no = g_day_no - 79;

  j_np = floordiv(j_day_no, 12053);
  j_day_no -= j_np * 12053;
  jalali.tm_year = 979 + (33 * j_np) + 4 * (j_day_no / 1461);

  j_day_no %= 1461;

  if (j_day_no >= 366) {
    jalali.tm_year += (j_day_no - 1) / 365;
    j_day_no = (j_day_no - 1) % 365;
  }
  for (i = 0; i < 11; i++){
    if (j_day_no < j_days_in_month[i]){
      i -= 1;
      break;
    }
    j_day_no -= j_days_in_month[i];
  }

  jalali.tm_mon = i + (i == 11 ? 1 : 2);
  jalali.tm_mday = j_day_no + 1;
  jalali.tm_hour = tm->tm_hour;
  jalali.tm_min = tm->tm_min;
  jalali.tm_sec = tm->tm_sec;

  if (jalali.tm_mon <= 6) {
    jalali.tm_yday = (jalali.tm_mon - 1) * 31 + jalali.tm_mday;
  } else {
    jalali.tm_yday = 186 + (jalali.tm_mon - 7) * 30 + jalali.tm_mday;
  }
  jalali.tm_wday = (tm->tm_wday + 1) % 7;

  return jalali;
};

static pg_time_t
whole_second_time_t(TimestampTz timestamp)
{
    pg_time_t t = timestamptz_to_time_t(timestamp);

    /* PostgreSQL truncates toward zero; pre-2000 fractions need the earlier second. */
    if (timestamp % USECS_PER_SEC < 0)
        t--;
    return t;
}

static struct pg_tm *
tehran_localtime(const pg_time_t *t)
{
    pg_tz *tehran = pg_tzset("Asia/Tehran");
    struct pg_tm *tm;

    if (tehran == NULL)
        ereport(ERROR,
                (errcode(ERRCODE_INTERNAL_ERROR),
                 errmsg("time zone \"Asia/Tehran\" is not available")));
    tm = pg_localtime(t, tehran);
    if (tm == NULL)
        ereport(ERROR,
                (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE),
                 errmsg("timestamp out of range")));
    return tm;
}

static TimestampTz
date_to_timestamptz(DateADT date)
{
    if (date >= (TIMESTAMP_END_JULIAN - POSTGRES_EPOCH_JDATE))
        ereport(ERROR,
                (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE),
                 errmsg("date out of range for timestamp")));
    return (TimestampTz) date * USECS_PER_DAY;
}

Datum
format_jalali_timestamp(PG_FUNCTION_ARGS)
{
    TimestampTz timestamp = PG_GETARG_TIMESTAMPTZ(0);
    bool with_time = PG_GETARG_BOOL(1);
    pg_time_t t;
    struct pg_tm jdate;
    char buf[64];
    int len;

    if (TIMESTAMP_IS_NOBEGIN(timestamp))
        PG_RETURN_TEXT_P(cstring_to_text("-infinity"));
    if (TIMESTAMP_IS_NOEND(timestamp))
        PG_RETURN_TEXT_P(cstring_to_text("infinity"));

    t = whole_second_time_t(timestamp);
    jdate = tm_to_jalali(tehran_localtime(&t));

    if (with_time)
        len = snprintf(buf, sizeof(buf), "%d/%02d/%02d %02d:%02d:%02d",
                       jdate.tm_year, jdate.tm_mon, jdate.tm_mday,
                       jdate.tm_hour, jdate.tm_min, jdate.tm_sec);
    else
        len = snprintf(buf, sizeof(buf), "%d/%02d/%02d",
                       jdate.tm_year, jdate.tm_mon, jdate.tm_mday);
    PG_RETURN_TEXT_P(cstring_to_text_with_len(buf, len));
}

Datum
format_jalali_date(PG_FUNCTION_ARGS)
{
    DateADT date = PG_GETARG_DATEADT(0);
    pg_time_t t;
    struct pg_tm *tm;
    struct pg_tm jdate;
    char buf[64];
    int len;

    if (DATE_IS_NOBEGIN(date))
        PG_RETURN_TEXT_P(cstring_to_text("-infinity"));
    if (DATE_IS_NOEND(date))
        PG_RETURN_TEXT_P(cstring_to_text("infinity"));

    /*
     * Midnight UTC of a date falls on the same civil day in Tehran (the
     * zone is east of Greenwich throughout tzdb history), so the UTC
     * breakdown is enough here and skips the time-zone lookup.
     */
    t = timestamptz_to_time_t(date_to_timestamptz(date));
    tm = pg_gmtime(&t);
    if (tm == NULL)
        ereport(ERROR,
                (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE),
                 errmsg("timestamp out of range")));
    jdate = tm_to_jalali(tm);
    len = snprintf(buf, sizeof(buf), "%d/%02d/%02d",
                   jdate.tm_year, jdate.tm_mon, jdate.tm_mday);
    PG_RETURN_TEXT_P(cstring_to_text_with_len(buf, len));
}


static int time_t_to_jalali_part(text* units, pg_time_t* t) {
  char       *lowunits;
  struct pg_tm *tm;
  struct pg_tm jdatetime;
  int  val;
  float8      result;

  tm = tehran_localtime(t);
  jdatetime = tm_to_jalali(tm);

  lowunits = downcase_truncate_identifier(VARDATA_ANY(units),
                                          VARSIZE_ANY_EXHDR(units),
                                          false);

  /* Both decoders store 0 in val for unknown units; the default case reports it. */
  if (DecodeUnits(0, lowunits, &val) == UNKNOWN_FIELD)
    DecodeSpecial(0, lowunits, &val);

  switch (val)
    {
    case DTK_SECOND:
      result = (&jdatetime)->tm_sec;
      break;

    case DTK_MINUTE:
      result = (&jdatetime)->tm_min;
      break;

    case DTK_HOUR:
      result = (&jdatetime)->tm_hour;
      break;

    case DTK_DAY:
      result = (&jdatetime)->tm_mday;
      break;
    case DTK_MONTH:
      result = (&jdatetime)->tm_mon;
      break;
    case DTK_YEAR:
      result = (&jdatetime)->tm_year;
      break;
    case DTK_DOW:
      result = j2day(date2j(tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday)) - 6;
      if (result < 0) {
        result += 7;
      }
      break;
    case DTK_DOY:
      if ((&jdatetime)-> tm_mon <= 6) {
        result = (((&jdatetime)-> tm_mon - 1) * 31)  + (&jdatetime)-> tm_mday;
      } else {
        result = 186 + (&jdatetime)-> tm_mday;
        result += ((&jdatetime)-> tm_mon - 7) * 30;
      }
      break;

    case DTK_QUARTER:
      result = ((&jdatetime)->tm_mon - 1) / 3 + 1;
      break;
    case DTK_DECADE:
      if ((&jdatetime)->tm_year > 0)
        result = ((&jdatetime)->tm_year / 10) * 10;
      else
        result = -((8 - ((&jdatetime)->tm_year - 1)) / 10) * 10;
      break;
    case DTK_CENTURY:
      result = ((&jdatetime)->tm_year + 99) / 100;
      break;

    default:
      ereport(ERROR,
              (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
               errmsg("\"time\" units \"%s\" not recognized",
                      lowunits)));
      result = 0;
    }
  return result;
}

Datum jalali_part_timestamp(PG_FUNCTION_ARGS) {
  text *units = PG_GETARG_TEXT_PP(0);
  TimestampTz timestamp = PG_GETARG_TIMESTAMPTZ(1);
  pg_time_t t;

  if (TIMESTAMP_NOT_FINITE(timestamp))
    PG_RETURN_NULL();
  t = whole_second_time_t(timestamp);
  PG_RETURN_INT32(time_t_to_jalali_part(units, &t));
}

Datum jalali_part_date(PG_FUNCTION_ARGS) {
  text *units = PG_GETARG_TEXT_PP(0);
  DateADT date = PG_GETARG_DATEADT(1);
  pg_time_t t;

  if (DATE_NOT_FINITE(date))
    PG_RETURN_NULL();
  t = timestamptz_to_time_t(date_to_timestamptz(date));
  PG_RETURN_INT32(time_t_to_jalali_part(units, &t));
}
