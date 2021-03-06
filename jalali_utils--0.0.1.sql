CREATE OR REPLACE FUNCTION format_jalali(
    timestamp WITH TIME ZONE,
    with_time BOOLEAN DEFAULT TRUE
)
RETURNS TEXT AS 'MODULE_PATHNAME','format_jalali_timestamp'
LANGUAGE C STRICT;

CREATE OR REPLACE FUNCTION format_jalali(
    date
)
RETURNS TEXT AS 'MODULE_PATHNAME','format_jalali_date'
LANGUAGE C STRICT;


CREATE OR REPLACE FUNCTION jalali_part(
    text,
    timestamp WITH TIME ZONE
)
RETURNS INTEGER AS 'MODULE_PATHNAME','jalali_part_timestamp'
LANGUAGE C STRICT;

CREATE OR REPLACE FUNCTION jalali_part(
    text,
    date
)
RETURNS INTEGER AS 'MODULE_PATHNAME','jalali_part_date'
LANGUAGE C STRICT;
