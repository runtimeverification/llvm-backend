KOMPILE=llvm-kompile-testing
DEFNDIR = ../test/defn
INPUTDIR = ../test/input
OUTPUTDIR = ../test/output
INTDIR = ../test/int
DEFN = $(wildcard $(DEFNDIR)/*.kore)
INPUT = $(wildcard $(INPUTDIR)/*.in.kore)
OUTPUT= $(wildcard $(OUTPUTDIR)/*.out.kore)
INT = $(patsubst $(DEFNDIR)/%.kore, $(INTDIR)/%.interpreter, $(DEFN))

all: $(INT)
	@echo $(INT)

$(INTDIR)/%.interpreter: $(DEFNDIR)/%.kore $(INPUTDIR)/%.in.kore $(OUTPUTDIR)/%.out.kore
	$(KOMPILE) $< main -o $@
	$@ $(word 2, $^) -1 /dev/stdout | diff - $(word 3, $^)


.PHONY: clean

clean:
	rm $(INT)
