### This Makefile processes Markdown files into HTML 
### The Markdown files are first run through a preprocessor that defines some useful macros:
###     HEADER(x)		creates a centered header in large type.
###		CENTER(x)		centers what is passed to it on the line.
###		IMAGE(img,size)	Inserts an image and allows you to set the size.

# This should be a list of all the HTML files that you want to generate.
all: GraspWelcome.html Joe.html

# Define the path to the pandoc.exe program that does the conversion.
PANDOC=pandoc.exe
PANDOC_OPTIONS=

# Define the preprocessor.
PREPROCESSOR=cl.exe
# The preprocessor that we are using is actually the Visual C++ compiler.
# The /EP option tells the compiler to preprocess only. 
# The /FI forces the inclusion of the preprocessor macros that we have defined. 
PREPROCESSOR_OPTIONS=/EP /FI PsyPhyMDmacros.h

.SUFFIXES: .html .md .tex
.md.html:
	$(PREPROCESSOR) $(PREPROCESSOR_OPTIONS) $? | $(PANDOC) $(PANDOC_OPTIONS)  -o $@
