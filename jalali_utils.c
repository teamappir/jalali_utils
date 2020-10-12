#include "postgres.h"
#include "parser/scansup.h"
#include "pgtime.h"
#include "nodes/makefuncs.h"
#include "utils/timestamp.h"
#include "utils/datetime.h"
#include "utils/date.h"

const int g_days_in_month[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
const int j_days_in_month[12] = {31, 31, 31, 31, 31, 31, 30, 30, 30, 30, 30, 29};

PG_MODULE_MAGIC;
PG_FUNCTION_INFO_V1(format_jalali_timestamp);
PG_FUNCTION_INFO_V1(format_jalali_date);
PG_FUNCTION_INFO_V1(jalali_part);

struct pg_tm tm_to_jalai(struct pg_tm *tm);

struct pg_tm tm_to_jalai(struct pg_tm *tm){
  struct pg_tm jalali;
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

  if(jalali.tm_mon<=6){
    jalali.tm_yday = (jalali.tm_mon - 1) * 31 + jalali.tm_mday;
  }else{
    jalali.tm_yday = 186 + (jalali.tm_mon - 7) * 30 + jalali.tm_mday;
  }
  jalali.tm_wday = (tm->tm_wday + 1) % 7;

  return jalali;
};

Datum format_jalali_timestamp(PG_FUNCTION_ARGS){
    Timestamp timestamp = PG_GETARG_TIMESTAMPTZ(0);
    bool with_time = PG_GETARG_BOOL(1);

    pg_time_t t = timestamptz_to_time_t(timestamp);
    pg_tz *tehran = pg_tzset("Asia/Tehran");
    struct pg_tm *tm = pg_localtime(&t, tehran);
    struct pg_tm jdate = tm_to_jalai(tm);

    if (with_time) {
        text *output = (text *) palloc(VARHDRSZ + 19);
        char *result = psprintf("%d/%02d/%02d %02d:%02d:%02d",
                                jdate.tm_year, jdate.tm_mon, jdate.tm_mday,
                                jdate.tm_hour, jdate.tm_min, jdate.tm_sec);
        SET_VARSIZE(output, VARHDRSZ + 19);
        memcpy(output->vl_dat, result, 19);
        PG_RETURN_CSTRING(output);
        PG_RETURN_TEXT_P(output);
    } else {
        text *output = (text *) palloc(VARHDRSZ + 10);
        char *result = psprintf("%d/%02d/%02d",
                                jdate.tm_year, jdate.tm_mon, jdate.tm_mday);
        SET_VARSIZE(output, VARHDRSZ + 10);
        memcpy(output->vl_dat, result, 10);
        PG_RETURN_TEXT_P(output);
    }
}

Datum format_jalali_date(PG_FUNCTION_ARGS){
    DateADT date = PG_GETARG_DATEADT(0);
    Timestamp timestamp = date2timestamp_no_overflow(date);

    pg_time_t t = timestamptz_to_time_t(timestamp);
    struct pg_tm tm = *pg_gmtime(&t);
    struct pg_tm jdate = tm_to_jalai(&tm);
    text *output = (text *) palloc(VARHDRSZ + 10);
    char *result = psprintf("%d/%02d/%02d",
                            jdate.tm_year, jdate.tm_mon, jdate.tm_mday);
    SET_VARSIZE(output, VARHDRSZ + 10);
    memcpy(output->vl_dat, result, 10);
    PG_RETURN_TEXT_P(output);
}

Datum jalali_part(PG_FUNCTION_ARGS) {
  text *units = PG_GETARG_TEXT_PP(0);
  Timestamp timestamp = PG_GETARG_TIMESTAMPTZ(1);
  char       *lowunits;
  int  type, val;
  float8      result;

  pg_time_t t = timestamptz_to_time_t(timestamp);
  pg_tz *tehran = pg_tzset("Asia/Tehran");
  struct pg_tm *tm = pg_localtime(&t, tehran);
  struct pg_tm jdatetime = tm_to_jalai(tm);

  lowunits = downcase_truncate_identifier(VARDATA_ANY(units),
                                          VARSIZE_ANY_EXHDR(units),
                                          false);

  type = DecodeUnits(0, lowunits, &val);

  if (type == UNKNOWN_FIELD)
    type = DecodeSpecial(0, lowunits, &val);


  switch (val)
    {
    case DTK_SECOND:
      result = (&jdatetime)->tm_sec  / 1000000.0;
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
    /* case DTK_DOW: */
    /*   break; */
    case DTK_DOY:
      if ((&jdatetime)-> tm_mon < 6)
        result = ((&jdatetime)-> tm_mon * 31) + (&jdatetime)-> tm_mday;

      result = 186 + (&jdatetime)-> tm_mday;
      result += ((&jdatetime)-> tm_mon - 7) * 30;
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
      if ((&jdatetime)->tm_year > 0)
        result = ((&jdatetime)->tm_year + 99) / 100;
      else
        result = -((99 - ((&jdatetime)->tm_year - 1)) / 100);
      break;
    case DTK_MILLENNIUM:
      if ((&jdatetime)->tm_year > 0)
        result = (((&jdatetime)->tm_year + 999) / 1000) * 1000 - 999;
      else
        result = -((999 - ((&jdatetime)->tm_year - 1)) / 1000) * 1000 + 1;
      break;

    default:
      ereport(ERROR,
              (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
               errmsg("\"time\" units \"%s\" not recognized",
                      lowunits)));
      result = 0;
    }
  PG_RETURN_INT32(result);
}
