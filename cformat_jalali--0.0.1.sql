CREATE OR REPLACE FUNCTION cformat_jalali(
    timestamp WITH TIME ZONE,
    with_time BOOLEAN DEFAULT TRUE
)
RETURNS TEXT AS 'MODULE_PATHNAME','cformat_jalali_timestamp'
LANGUAGE C STRICT;

CREATE OR REPLACE FUNCTION cformat_jalali(
    date
)
RETURNS TEXT AS 'MODULE_PATHNAME','cformat_jalali_date'
LANGUAGE C STRICT;


CREATE OR REPLACE FUNCTION week_number_jalali(
    date
)
RETURNS TEXT AS 'MODULE_PATHNAME','week_number_jalali'
LANGUAGE C STRICT;