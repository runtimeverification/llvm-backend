KOMPILE=llvm-kompile-testing
DEFNDIR = ../test/defn
INPUTDIR = ../test/input
OUTPUTDIR = ../test/output
INTDIR = ../test/int

DEFN = $(wildcard $(DEFNDIR)/*.kore)
INPUT = $(wildcard $(INPUTDIR)/*.in.kore)
OUTPUT= $(wildcard $(OUTPUTDIR)/*.out.kore)
INT = $(patsubst $(DEFNDIR)/%.kore, $(INTDIR)/%.interpreter, $(DEFN))

NOOUTS = test7
GTESTNAMES = test2 test3 test-gc-int test-gc-float test-gc-alwaysgc imp
DIRTESTNAMES = lambda test12 io
SPECIAL = test11 test17

TESTS = $(filter-out $(addprefix $(DEFNDIR)/, $(addsuffix .test, $(NOOUTS) $(GTESTNAMES) $(DIRTESTNAMES) $(SPECIAL))), $(addsuffix .test, $(basename $(DEFN))))
TESTSG = $(addprefix $(DEFNDIR)/, $(addsuffix .testg, $(GTESTNAMES)))
TESTSD = $(addprefix $(DEFNDIR)/, $(addsuffix .testd, $(DIRTESTNAMES)))
TESTSN = $(addprefix $(DEFNDIR)/, $(addsuffix .testn, $(NOOUTS)))

all: $(INT) grep-test noout-test test

grep-test: $(TESTSG)

noout-test: $(TESTN)

test: $(TESTS)

dir-tests: $(TESTSD)

$(INTDIR)/%.interpreter: $(DEFNDIR)/%.kore
	$(KOMPILE) $< main -o $@

$(DEFNDIR)/%.test: $(INTDIR)/%.interpreter $(INPUTDIR)/%.in.kore $(OUTPUTDIR)/%.out.kore
	$< $(word 2, $^) -1 /dev/stdout | diff - $(word 3, $^)

$(DEFNDIR)/%.testg: $(INTDIR)/%.interpreter $(INPUTDIR)/%.in.kore $(OUTPUTDIR)/%.out.kore
	$< $(word 2, $^) -1 /dev/stdout | grep -f $(word 3, $^) -q

$(DEFNDIR)/%.testn: $(INTDIR)/%.interpreter $(INPUTDIR)/%.in.kore
	$< $(word 2, $^) -1 /dev/null

.PHONY: clean %.test %.test

clean:
	rm $(INT)
