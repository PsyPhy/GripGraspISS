all: GraspWelcome.html

PANDOC=pandoc.exe

GraspWelcome.html: GraspWelcome.md
	echo Joe
	echo $? -> $@
	$(PANDOC) $(PANDOC_OPTIONS) $(PANDOC_HTML_OPTIONS) -o $@ $?

%.html : %.md
	echo Joe
	echo $< -> $@
	$(PANDOC) $(PANDOC_OPTIONS) $(PANDOC_HTML_OPTIONS) -o $@ $<