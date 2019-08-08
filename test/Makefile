KOMPILE=llvm-kompile-testing
DEFNDIR = ../test/defn
INPUTDIR = ../test/input
OUTPUTDIR = ../test/output
INTDIR = ../test/int

SUFINKORE = .in.kore
SUFOUTDIFF = .out.diff.kore
SUFOUTGREP = .out.grep.kore

DEFN = $(wildcard $(DEFNDIR)/*.kore)
INPUT = $(wildcard $(INPUTDIR)/*$(SUFINKORE))
DOUTPUT= $(wildcard $(OUTPUTDIR)/*$(SUFOUTDIFF))
GOUTPUT= $(wildcard $(OUTPUTDIR)/*$(SUFOUTGREP))
INT = $(patsubst $(DEFNDIR)/%.kore, $(INTDIR)/%.interpreter, $(DEFN))

# For definitions that don't compare to an output file
NOOUTS = test7
NOTEST = test11 test17

# For definitions that have multiple tests associated with them
DIRTESTNAMES = $(notdir $(shell find $(INPUTDIR) -maxdepth 1 -mindepth 1 -type d))

TESTS = $(filter-out $(addprefix $(DEFNDIR)/, $(addsuffix .test, $(NOOUTS) $(DIRTESTNAMES) $(NOTEST))), $(addsuffix .test, $(basename $(DEFN))))
TESTSD = $(addprefix $(DEFNDIR)/, $(addsuffix .testd, $(DIRTESTNAMES)))
TESTSN = $(addprefix $(DEFNDIR)/, $(addsuffix .testn, $(NOOUTS)))

all: $(INT) test

testd: $(TESTSD)

test: $(TESTS) $(TESTSN) $(TESTSD) 

$(INTDIR)/%.interpreter: $(DEFNDIR)/%.kore
	$(KOMPILE) $< main -o $@

$(DEFNDIR)/%.test: $(INTDIR)/%.interpreter $(INPUTDIR)/%$(SUFINKORE) $(OUTPUTDIR)/%$(SUFOUTDIFF)
	$< $(word 2, $^) -1 /dev/stdout | diff - $(word 3, $^)

$(DEFNDIR)/%.test: $(INTDIR)/%.interpreter $(INPUTDIR)/%$(SUFINKORE) $(OUTPUTDIR)/%$(SUFOUTGREP)
	$< $(word 2, $^) -1 /dev/stdout | grep -f $(word 3, $^) -q

$(DEFNDIR)/%.testd: $(INTDIR)/%.interpreter $(INPUTDIR)/%/ $(OUTPUTDIR)/%/
	$(eval TMP_DIFF_OUT := $(wildcard $(word 3, $^)/*$(SUFOUTDIFF)))
	$(eval TMP_GREP_OUT := $(wildcard $(word 3, $^)/*$(SUFOUTGREP)))
	$(eval TMP_DIFF_NAME := $(subst $(SUFOUTDIFF),,$(notdir $(TMP_DIFF_OUT))))
	$(eval TMP_GREP_NAME := $(subst $(SUFOUTGREP),, $(notdir $(TMP_GREP_OUT))))
	$(eval TMP_DIR_NAME := $(basename $(notdir $@)))
	$(foreach file, $(TMP_DIFF_NAME), $< $(INPUTDIR)/$(TMP_DIR_NAME)/$(file)$(SUFINKORE) -1 /dev/stdout | diff - $(OUTPUTDIR)/$(TMP_DIR_NAME)/$(file)$(SUFOUTDIFF) ;)
	$(foreach file, $(TMP_GREP_NAME), $< $(INPUTDIR)/$(TMP_DIR_NAME)/$(file)$(SUFINKORE) -1 /dev/stdout | grep -f $(OUTPUTDIR)/$(TMP_DIR_NAME)/$(file)$(SUFOUTGREP) -q ;)

$(DEFNDIR)/%.testn: $(INTDIR)/%.interpreter $(INPUTDIR)/%$(SUFINKORE)
	$< $(word 2, $^) -1 /dev/null

.PHONY: clean

clean:
	rm $(INT)
