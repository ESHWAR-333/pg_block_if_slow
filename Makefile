EXTENSION = pg_block_if_slow
MODULES = pg_block_if_slow
DATA = pg_block_if_slow--1.0.sql
PG_CONFIG = pg_config

PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)

