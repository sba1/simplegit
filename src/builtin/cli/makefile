GENOPTS_FILES = $(wildcard *genopts)
GENOPTS_SRCS = $(GENOPTS_FILES:%.genopts=%_cli.c)

GENOPTS := ../../../genopts/genopts.py

.PHONY: all
all: $(GENOPTS_SRCS)

$(GENOPTS_SRCS): %_cli.c: %.genopts $(GENOPTS)
	cat $< | $(GENOPTS) >$@

.PHONY: clean
clean:
	rm $(GENOPTS_SRCS)
