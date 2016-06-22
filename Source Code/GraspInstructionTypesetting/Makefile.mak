### This Makefile processes Markdown files into HTML 
### The Markdown files are first run through a preprocessor that defines some useful macros:
###     HEADER(x)		creates a centered header in large type.
###		CENTER(x)		centers what is passed to it on the line.
###		IMAGE(img,size)	Inserts an image and allows you to set the size.

INSTRUCTIONS_DESTINATION=..\..\Instructions

install: Grasp.html
	copy *.html $(INSTRUCTIONS_DESTINATION)
	mkdir $(INSTRUCTIONS_DESTINATION)\Pictures & echo Ignoring any errors from the mkdir command.
	copy Pictures\*.* $(INSTRUCTIONS_DESTINATION)\Pictures
	mkdir $(INSTRUCTIONS_DESTINATION)\Figures_Instructions & echo Ignoring any errors from the mkdir command.
	copy Figures_Instructions\*.* $(INSTRUCTIONS_DESTINATION)\Figures_Instructions
	mkdir $(INSTRUCTIONS_DESTINATION)\Figures_Instructions\V-V & echo Ignoring any errors from the mkdir command.
	copy Figures_Instructions\V-V\*.* $(INSTRUCTIONS_DESTINATION)\Figures_Instructions\V-V
	mkdir $(INSTRUCTIONS_DESTINATION)\Figures_Instructions\V-K & echo Ignoring any errors from the mkdir command.
	copy Figures_Instructions\V-K\*.* $(INSTRUCTIONS_DESTINATION)\Figures_Instructions\V-K
	mkdir $(INSTRUCTIONS_DESTINATION)\Figures_Instructions\K-K & echo Ignoring any errors from the mkdir command.
	copy Figures_Instructions\K-K\*.* $(INSTRUCTIONS_DESTINATION)\Figures_Instructions\K-K
	echo %date% %time% > $@

# This should be a list of all the HTML files that you want to generate.
ALL_HTML=GraspWelcome.html NotYetImplemented.html Invalid.html \
	StepReady.html StepRunning.html StepFinished.html StepNormalFinish.html StepErrorFinish.html \
	TaskFinished.html ProtocolFinished.html \
	InstallSeated1.html InstallSeated2.html InstallFloating1.html InstallFloating2.html \
	CodaInstallationCheckSeated.html CodaInstallationCheckFloating.html InstallLocker.html \
	CodaAlignFloating.html CodaAlignSeated.html \
	01StraightenHead.html 02TargetK.html 02TargetV.html 03TiltHead.html 04RespondK.html 04RespondV.html 05Feedback.html \
	V-VFloatingProtocolReady.html V-VSeatedProtocolReady.html V-V1.html V-V2.html V-V3.html V-V4.html V-V5.html V-V6.html \
	V-KFloatingProtocolReady.html V-KSeatedProtocolReady.html V-K1.html V-K2.html V-K3.html V-K4.html V-K5.html V-K6.html \
	K-KFloatingProtocolReady.html K-KSeatedProtocolReady.html K-K1.html K-K2.html K-K3.html K-K4.html K-K5.html K-K6.html 
	
# The idea here is to create a single document to show all the instruction screens.
# The HTML document created by pandoc is not entirely well suited to our needs, but it's a start.
Grasp.html: $(ALL_HTML)
	pandoc -t S5 --standalone $(ALL_HTML) -o $@

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
