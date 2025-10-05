# Jalali Utils (A Jalali extension for PostgreSQL)
Jalali Utils is a package for converting Gregorian dates to Jalali.
This package consists of the following functions:


```sql
format_jalali(TIMESTAMP WITH TIME ZONE, with_time BOOLEAN DEFAULT TRUE)
format_jalali(DATE)
jalali_part(TEXT, TIMESTAMP WITH TIME ZONE)
jalali_part(TEXT, DATE)
```

Examples:
```sql
SELECT format_jalali(now()); -->   1399/07/08 23:45:37
SELECT format_jalali(now(), false); -->   1399/07/08
SELECT format_jalali('2019-07-07', false); -->  1398/04/16
SELECT format_jalali('2021-03-20'); -->  1399/12/30 00:00:00
SELECT format_jalali('2021-03-20'::date); -->  1399/12/30
SELECT jalali_part('year', '2019-07-07 14:10:52.84937+04:30'); --> 1398
SELECT jalali_part('minute', '2019-07-07 14:10:52.84937+04:30'); --> 10
SELECT jalali_part('doy', '2019-07-07 14:10:52.84937+04:30'); --> 109
SELECT jalali_part('dow', '2019-07-07'); --> 4
```

`jalali_part` function can accept these values as the first parameter:
* year
* month
* day
* hour
* minute
* second
* doy (day of year)
* dow (day of week with index zero as saturday)
* quarter
* decade
* century

# Installation

Dependencies:
> sudo apt install postgresql-server-dev-17

Run make to build and install
> make

> sudo make install

Run this command in your psql shell:
> \d your_db_name

> CREATE EXTENSION jalali_utils;

# Running tests

> make installcheck
