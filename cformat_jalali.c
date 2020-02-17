#include "postgres.h"
#include "pgtime.h"
#include "nodes/makefuncs.h"
#include "utils/timestamp.h"
#include "utils/date.h"

const int g_days_in_month[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
const int j_days_in_month[12] = {31, 31, 31, 31, 31, 31, 30, 30, 30, 30, 30, 29};

PG_MODULE_MAGIC;
PG_FUNCTION_INFO_V1(cformat_jalali_timestamp);
PG_FUNCTION_INFO_V1(cformat_jalali_date);

struct datetime {
  short year;
  short month;
  short day;
  short hour;
  short minute;
  short second;
};

struct datetime tm_to_jalai(struct pg_tm *tm);

struct datetime tm_to_jalai(struct pg_tm *tm)
{
  struct datetime jalali;
  int j_day_no, j_np;
  int g_day_no, i;

  int gy = tm->tm_year + 300;

  g_day_no = 365 * gy + (gy + 3) / 4 - (gy + 99) / 100 + (gy + 399) / 400;

  for (i = 0; i < tm->tm_mon; i++) {
    g_day_no += g_days_in_month[i];
  }

  if (tm->tm_mon > 1 && ((gy % 4 == 0 && gy % 100 != 0) || (gy % 400 == 0))) {
    g_day_no += 1;
  }
  g_day_no += (tm->tm_mday - 1);
  j_day_no = g_day_no - 79;

  j_np = j_day_no / 12053;
  j_day_no %= 12053;
  jalali.year = 979 + (33 * j_np) + 4 * (j_day_no / 1461);

  j_day_no %= 1461;

  if (j_day_no >= 366) {
    jalali.year += (j_day_no - 1) / 365;
    j_day_no = (j_day_no - 1) % 365;
  }
  for (i = 0; i < 11; i++){
    if (j_day_no < j_days_in_month[i]){
      i -= 1;
      break;
    }
    j_day_no -= j_days_in_month[i];
  }

  jalali.month = i + (i == 11 ? 1 : 2);
  jalali.day = j_day_no + 1;
  jalali.hour = tm->tm_hour;
  jalali.minute = tm->tm_min;
  jalali.second = tm->tm_sec;
  return jalali;
};

Datum cformat_jalali_timestamp(PG_FUNCTION_ARGS){
    Timestamp timestamp = PG_GETARG_TIMESTAMPTZ(0);
    bool with_time = PG_GETARG_BOOL(1);

    pg_time_t t = timestamptz_to_time_t(timestamp);
    pg_tz *tehran = pg_tzset("Asia/Tehran");
    struct pg_tm *tm = pg_localtime(&t, tehran);
    struct datetime jdate = tm_to_jalai(tm);

    char *result;
    if (with_time) {
        result = psprintf("-%d/%02d/%02d %02d:%02d:%02d",
                          jdate.year, jdate.month, jdate.day,
                          jdate.hour, jdate.minute, jdate.second);
    } else {
        result = psprintf("-%d/%02d/%02d", jdate.year, jdate.month, jdate.day);
    }
    PG_RETURN_CSTRING(result);
}

Datum cformat_jalali_date(PG_FUNCTION_ARGS){
    DateADT date = PG_GETARG_DATEADT(0);
    Timestamp timestamp = date2timestamp_no_overflow(date);

    pg_time_t t = timestamptz_to_time_t(timestamp);
    struct pg_tm tm = *pg_gmtime(&t);
    struct datetime jdate = tm_to_jalai(&tm);
    char * result  = psprintf("-%d/%02d/%02d", jdate.year, jdate.month, jdate.day);
    PG_RETURN_CSTRING(result);
}
