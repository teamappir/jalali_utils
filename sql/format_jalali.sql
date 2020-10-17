CREATE EXTENSION jalali_utils;

SELECT format_jalali('2019-07-07 14:10:52.84937+04:30'::timestamptz, false);
SELECT format_jalali('2019-07-07 14:10:52.84937+04:30'::timestamptz);
SELECT format_jalali('2019-07-07 14:10:52.84937+04:30'::date);
SELECT format_jalali('2001-10-15'::date);
SELECT format_jalali('2021-03-20'::date);
SELECT format_jalali('2020-03-19'::date);
SELECT format_jalali('2019-03-21'::date);
DROP EXTENSION jalali_utils;
