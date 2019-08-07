KOMPILE=llvm-kompile-testing
DEFNDIR = ../test/defn
INPUTDIR = ../test/input
OUTPUTDIR = ../test/output
INTDIR = ../test/int

DEFN = $(wildcard $(DEFNDIR)/*.kore)
INPUT = $(wildcard $(INPUTDIR)/*.in.kore)
DOUTPUT= $(wildcard $(OUTPUTDIR)/*.out.diff.kore)
GOUTPUT= $(wildcard $(OUTPUTDIR)/*.out.grep.kore)
INT = $(patsubst $(DEFNDIR)/%.kore, $(INTDIR)/%.interpreter, $(DEFN))

NOOUTS = test7
DIRTESTNAMES = lambda test12 io
SPECIAL = test11 test17

TESTS = $(filter-out $(addprefix $(DEFNDIR)/, $(addsuffix .test, $(NOOUTS) $(DIRTESTNAMES) $(SPECIAL))), $(addsuffix .test, $(basename $(DEFN))))
TESTSD = $(addprefix $(DEFNDIR)/, $(addsuffix .testd, $(DIRTESTNAMES)))
TESTSN = $(addprefix $(DEFNDIR)/, $(addsuffix .testn, $(NOOUTS)))

all: $(INT) test noout-test dir-test

test: $(TESTS)

noout-test: $(TESTN)

dir-test: $(TESTSD)

$(INTDIR)/%.interpreter: $(DEFNDIR)/%.kore
	$(KOMPILE) $< main -o $@

$(DEFNDIR)/%.test: $(INTDIR)/%.interpreter $(INPUTDIR)/%.in.kore $(OUTPUTDIR)/%.out.diff.kore
	$< $(word 2, $^) -1 /dev/stdout | diff - $(word 3, $^)

$(DEFNDIR)/%.test: $(INTDIR)/%.interpreter $(INPUTDIR)/%.in.kore $(OUTPUTDIR)/%.out.grep.kore
	$< $(word 2, $^) -1 /dev/stdout | grep -f $(word 3, $^) -q

$(DEFNDIR)/%.testd: $(INTDIR)/%.interpreter $(INPUTDIR)/%/ $(OUTPUTDIR)/%/
	$(eval TMP_DIFF_OUT := $(wildcard $(word 3, $^)/*.out.diff.kore))
	$(eval TMP_GREP_OUT := $(wildcard $(word 3, $^)/*.out.grep.kore))
	$(eval TMP_DIFF_NAME := $(basename $(basename $(basename $(notdir $(TMP_DIFF_OUT))))))
	$(eval TMP_GREP_NAME := $(basename $(basename $(basename $(notdir $(TMP_GREP_OUT))))))
	$(eval TMP_DIR := $(basename $(notdir $@)))
	$(foreach file, $(TMP_DIFF_NAME), $< $(INPUTDIR)/$(TMP_DIR)/$(file).in.kore -1 /dev/stdout | diff - $(OUTPUTDIR)/$(TMP_DIR)/$(file).out.diff.kore ;)
	$(foreach file, $(TMP_GREP_NAME), $< $(INPUTDIR)/$(TMP_DIR)/$(file).in.kore -1 /dev/stdout | grep -f $(OUTPUTDIR)/$(TMP_DIR)/$(file).out.grep.kore -q ;)

$(DEFNDIR)/%.testn: $(INTDIR)/%.interpreter $(INPUTDIR)/%.in.kore
	$< $(word 2, $^) -1 /dev/null

.PHONY: clean

clean:
	rm $(INT)
