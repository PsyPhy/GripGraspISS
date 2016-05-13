all: GraspWelcome.html

PANDOC=pandoc.exe

GraspWelcome.html: GraspWelcome.md

%.html : %.md
	echo Joe
	echo $< -> $@
	$(PANDOC) $(PANDOC_OPTIONS) $(PANDOC_HTML_OPTIONS) -o $@ $<