MODULES = cformat_jalali
EXTENSION = cformat_jalali
DATA = cformat_jalali--0.0.1.sql
PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)