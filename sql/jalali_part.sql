CREATE EXTENSION jalali_utils;

SELECT jalali_part('year', '2019-07-07 14:10:52.84937+04:30');

SELECT jalali_part('month', '2019-07-07 14:10:52.84937+04:30');

SELECT jalali_part('day', '2019-07-07 14:10:52.84937+04:30');

SELECT jalali_part('hour', '2019-07-07 14:10:52.84937+04:30');

SELECT jalali_part('minute', '2019-07-07 14:10:52.84937+04:30');

SELECT jalali_part('second', '2019-07-07 14:10:52.84937+04:30');

SELECT jalali_part('doy', '2019-07-07'); -- 1398/04/16

SELECT jalali_part('doy', '2019-09-22'); -- 1398/06/31

SELECT jalali_part('doy', '2020-03-19'); -- 1398/12/29

SELECT jalali_part('doy', '2021-03-20'); -- 1399/12/30

SELECT jalali_part('doy', '2021-03-21'); -- 1400/01/01

SELECT jalali_part('quarter', '2021-03-21');

SELECT jalali_part('quarter', '2021-03-20');

SELECT jalali_part('decade', '2021-03-21');

SELECT jalali_part('decade', '2021-03-20');

SELECT jalali_part('century', '2020-03-20');

SELECT jalali_part('century', '2022-03-20');

SELECT jalali_part('century', '2022-03-21');

SELECT jalali_part('dow', '2001-10-16');

SELECT jalali_part('dow', '2020-10-17');

SELECT jalali_part('dow', '2020-10-23');

SELECT jalali_part('year', '-infinity'::timestamptz);

SELECT jalali_part('year', '-infinity'::date);

SELECT jalali_part('year', '5874897-12-31'::date);

SELECT jalali_part('day', '1000-01-01'::date);

SELECT jalali_part('month', '1599-12-31'::date);

DROP EXTENSION jalali_utils;
