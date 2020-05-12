TESTS=$(wildcard */*.cfg.kore)

.PHONY: all
all: $(TESTS)

%.cfg.kore: kprint
	kprint $(dir $@) $@ true | diff - $@.out

.PHONY: kprint
kprint:
