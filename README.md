Jalali Utils
===
jalali_utils is a postgres extension 

Features
===
* Convert gregorian datetime to jalali datetime text(at Asia/Tehran timezone)

Installation
===
Requirements
* postgres 12

install package
```
gg
```

Create extension for datebase
```
CREATE EXTENSION jalali_utils;
```

Simple Usage
```
SELECT format_jalali(now());
```

Development
===
install dependencies:
```
sudo apt install postgresql-server-dev-12
```

run make to build and install
```
sudo make install
```
