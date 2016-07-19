### This Makefile processes Markdown files into HTML 
### The Markdown files are first run through a preprocessor that defines some useful macros:
###     HEADER(x)		creates a centered header in large type.
###		CENTER(x)		centers what is passed to it on the line.
###		IMAGE(img,size)	Inserts an image and allows you to set the size.

INSTRUCTIONS_DESTINATION=..\..\Instructions
EXECUTABLES=..\..\Executables

# This should be a list of all the HTML files that you want to generate.
ALL_HTML=GraspWelcome.html \
	00IntroV-V.instruction.html 00IntroV-K.instruction.html 00IntroK-K.instruction.html \
	CodaAlignFloating.prompt.html CodaAlignSeated.prompt.html StepReadySeated.prompt.html StepReadyFloating.prompt.html \
	01StraightenHead.instruction.html 02TargetK.instruction.html 02TargetV.instruction.html \
	03TiltHead.instruction.html 04RespondK.instruction.html 04RespondV.instruction.html 05Feedback.instruction.html \
	StepReady.prompt.html StepRunning.instruction.html StepFinished.status.html StepNormalFinish.status.html StepErrorFinish.status.html \
	TaskFinished.instruction.html ProtocolFinished.instruction.html SetNoCodaReady.prompt.html SetWithCodaReady.prompt.html \
	MaintenanceWelcome.instruction.html

# This is going to install the instruction screens in the execution arboresence.
# We delete the destination directory so that we eliminate any previous files that are no longer needed
# and then copy in the newly created files.
install: $(ALL_HTML) GraspInstructions.mak
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
