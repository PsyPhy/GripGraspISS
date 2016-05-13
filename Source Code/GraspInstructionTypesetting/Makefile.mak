all: GraspWelcome.html

PANDOC=pandoc.exe

all: GraspWelcome.html Latex.html

.SUFFIXES: .html .md .tex
.md.html:
	$(PANDOC) $(PANDOC_OPTIONS) $(PANDOC_HTML_OPTIONS) -o $@ $?

.tex.html:
	$(PANDOC) $(PANDOC_OPTIONS) $(PANDOC_LATEX_OPTIONS) -o $@ $?