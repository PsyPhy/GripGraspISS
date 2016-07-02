### This Makefile processes Markdown files into HTML 
### The Markdown files are first run through a preprocessor that defines some useful macros:
###     HEADER(x)		creates a centered header in large type.
###		CENTER(x)		centers what is passed to it on the line.
###		IMAGE(img,size)	Inserts an image and allows you to set the size.

INSTRUCTIONS_DESTINATION=..\..\Instructions
DOCUMENTATION_DESTINATION=..\..\Documentation
EXECUTABLES=..\..\Executables

# This should be a list of all the HTML files that you want to generate.
ALL_HTML=GraspWelcome.html \
	00IntroV-V.html 00IntroV-K.html 00IntroK-K.html \
	CodaAlignFloating.html CodaAlignSeated.html StepReadySeated.html StepReadyFloating.html \
	01StraightenHead.html 02TargetK.html 02TargetV.html 03TiltHead.html 04RespondK.html 04RespondV.html 05Feedback.html \
	StepReady.html StepRunning.html StepFinished.html StepNormalFinish.html StepErrorFinish.html \
	TaskFinished.html ProtocolFinished.html 

install: $(ALL_HTML)
	rmdir /S /Q $(INSTRUCTIONS_DESTINATION)
	mkdir $(INSTRUCTIONS_DESTINATION) & echo Ignoring any failures of the mkdir command.
	copy *.html $(INSTRUCTIONS_DESTINATION)
	mkdir $(INSTRUCTIONS_DESTINATION)\Pictures & echo Ignoring any failures of the mkdir command.
	copy Pictures\*.* $(INSTRUCTIONS_DESTINATION)\Pictures
	echo %date% %time% > $@

# Define the path to the pandoc.exe program that does the conversion.
PANDOC=pandoc.exe
PANDOC_OPTIONS=

# Define the preprocessor.
PREPROCESSOR=cl.exe
# The preprocessor that we are using is actually the Visual C++ compiler.
# The /EP option tells the compiler to preprocess only. 
# The /FI forces the inclusion of the preprocessor macros that we have defined. 
PREPROCESSOR_OPTIONS=/EP /nologo /FI PsyPhyMDmacros.h

.SUFFIXES: .html .md .tex
.md.html:
	$(PREPROCESSOR) $(PREPROCESSOR_OPTIONS) $? | $(PANDOC) $(PANDOC_OPTIONS)  -o $@
