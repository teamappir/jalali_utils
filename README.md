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

CI tests PostgreSQL 12–19.

On Debian or Ubuntu, set `PG_VERSION` to your target PostgreSQL major version.
The following example uses 18; replace it with your installed version:

```sh
PG_VERSION=18
sudo apt install "postgresql-server-dev-$PG_VERSION"
PG_CONFIG="/usr/lib/postgresql/$PG_VERSION/bin/pg_config"
```

Build and install using the same PostgreSQL version:

```sh
make PG_CONFIG="$PG_CONFIG"
sudo make install PG_CONFIG="$PG_CONFIG"
```

In `psql`, connect to your database and enable the extension:

```sql
\connect your_db_name
CREATE EXTENSION jalali_utils;
```

# Running tests

With the target PostgreSQL server running, use the same `PG_CONFIG` set above:

```sh
make installcheck PG_CONFIG="$PG_CONFIG"
```

If the server uses a non-default port, pass it with `PGPORT`:

```sh
make installcheck PG_CONFIG="$PG_CONFIG" PGPORT=55432
```
