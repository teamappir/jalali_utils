MODULES = jalali_utils
EXTENSION = jalali_utils
DATA = jalali_utils--0.0.1.sql
PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
