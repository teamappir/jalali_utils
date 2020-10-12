Jalali Utils is a package for converting date time with timezones to jalali string.
There are two functions:

format_jalali(timestamp WITH TIME ZONE, with_time BOOLEAN DEFAULT TRUE)
format_jalali(date)

Examples:
select format_jalali(now()); -->   1399/07/08 23:45:37
select format_jalali(now(), false); -->   1399/07/08
SELECT format_jalali('2019-07-07 14:10:52.84937+04:30'::date); -->  1398/04/16
select format_jalali('2021-03-20 14:10:52.84937+04:30'::date); -->


Dependencies:
> sudo apt install postgresql-server-dev-12

Run make to build and install
> make
> sudo make install

Run this command in your psql shell:
> \d your_db_name
> CREATE EXTENSION jalali_utils;
