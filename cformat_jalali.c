
#include "postgres.h"
// #include "fmgr.h"

#include "nodes/makefuncs.h"
#include "utils/timestamp.h"
// #include "access/xact.h"
// #include "catalog/pg_type.h"
// #include "common/int128.h"
// #include "funcapi.h"
// #include "libpq/pqformat.h"
// #include "miscadmin.h"
// #include "nodes/nodeFuncs.h"
// #include "nodes/supportnodes.h"
// #include "parser/scansup.h"
// #include "utils/array.h"
// #include "utils/builtins.h"
// #include "utils/datetime.h"
// #include "utils/float.h"
// #include "pgtime.h"

const int g_days_in_month[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
const int j_days_in_month[12] = {31, 31, 31, 31, 31, 31, 30, 30, 30, 30, 30, 29};

PG_MODULE_MAGIC;
PG_FUNCTION_INFO_V1(cformat_jalali);

Datum
    cformat_jalali(PG_FUNCTION_ARGS)
{
    Timestamp timestamp = PG_GETARG_TIMESTAMPTZ(0);
    bool with_time = PG_GETARG_BOOL(1);
    bool use_utc = PG_GETARG_BOOL(2);

    int gyear, gmonth, gday, ghour, gminute, gsecond;
    int gy, gm, gd;
    int j_day_no, j_np;
    int jy, jm, jd;
    int g_day_no, i;

    struct tm tm;

    char *result;

    pg_time_t t = timestamptz_to_time_t(timestamp);
    tm = use_utc ? *gmtime(&t) : *localtime(&t);

    gyear = tm.tm_year + 1900;
    gmonth = tm.tm_mon + 1;
    gday = tm.tm_mday;
    ghour = tm.tm_hour;
    gminute = tm.tm_min;
    gsecond = tm.tm_sec;

    gy = gyear - 1600;
    gm = gmonth - 1;
    gd = gday - 1;

    g_day_no = 365 * gy + (gy + 3) / 4 - (gy + 99) / 100 + (gy + 399) / 400;

    for (i = 0; i < gm; i++)
    {
        g_day_no += g_days_in_month[i];
    }

    if (gm > 1 && ((gy % 4 == 0 && gy % 100 != 0) || (gy % 400 == 0)))
    {
        g_day_no += 1;
    }
    g_day_no += gd;

    j_day_no = g_day_no - 79;

    j_np = j_day_no / 12053;
    j_day_no %= 12053;
    jy = 979 + (33 * j_np) + 4 * (j_day_no / 1461);

    j_day_no %= 1461;

    if (j_day_no >= 366)
    {
        jy += (j_day_no - 1) / 365;
        j_day_no = (j_day_no - 1) % 365;
    }
    for (i = 0; i < 11; i++)
    {
        if (!(j_day_no >= j_days_in_month[i]))
        {
            i -= 1;
            break;
        }
        j_day_no -= j_days_in_month[i];
    }

    if (i == 11)
    {
        jm = i + 1;
    }
    else
    {
        jm = i + 2;
    }
    jd = j_day_no + 1;

    if (with_time)
    {
        result = psprintf("-%d-%02d-%02d %02d:%02d:%02d", with_time, use_utc, jd, ghour, gminute, gsecond);
    }
    else
    {
        result = psprintf("-%d-%02d-%02d", jy, jm, jd);
    }

    PG_RETURN_CSTRING(result);
}