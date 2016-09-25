### This Makefile processes Markdown files into HTML 
### The Markdown files are first run through a preprocessor that defines some useful macros:
###     HEADER(x)		creates a centered header in large type.
###		CENTER(x)		centers what is passed to it on the line.
###		IMAGE(img,size)	Inserts an image and allows you to set the size.

INSTRUCTIONS_DESTINATION=..\..\Instructions
EXECUTABLES=..\..\Executables
SCREENSHOTEXE="Source Code\"$(BUILDCONFIGURATION)"\GraspScreenshots.exe" --size=512

# This should be a list of all the HTML files that you want to generate.
ALL_HTML=GraspWelcome.html \
	00IntroV-V.instruction.html 00IntroV-K.instruction.html 00IntroK-K.instruction.html \
	CodaAlignFloating.prompt.html CodaAlignSeated.prompt.html \
	HMDStructureAssembly.instruction.html \
	StepReadySeated.prompt.html StepReadyFloating.prompt.html \
	ObjectPositioningSeated.instruction.html ObjectPositioningFloating.instruction.html\
	01StraightenHead.instruction.html 02TargetK.instruction.html 02TargetV.instruction.html \
	03TiltHead.instruction.html 04RespondK.instruction.html 04RespondV.instruction.html 05Feedback.instruction.html \
	StepReady.prompt.html StepRunning.instruction.html StepNormalFinish.status.html StepErrorFinish.status.html \
	TaskFinished.instruction.html ProtocolFinished.instruction.html SetNoCodaReady.prompt.html SetWithCodaReady.prompt.html \
	MaintenanceWelcome.instruction.html

# This is a list of images that are created here in the makefile.
# There are other images that are needed to create the html files, but they sit statically in the Pictures subdirectory.
VRIMAGES=Pictures/StraightenHeadRed.bmp Pictures/StraightenHeadGreen.bmp Pictures/StraightenHeadYellow.bmp Pictures/StraightenHeadCyan.bmp

# This is going to install the instruction screens in the execution arboresence.
# We delete the destination directory so that we eliminate any previous files that are no longer needed
# and then copy in the newly created files.
install: $(ALL_HTML)  $(VRIMAGES) GraspInstructions.mak
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

redo: ..\$(BUILDCONFIGURATION)\GraspScreenshots.exe
	echo echo %date% %time% > $@

Pictures\StraightenHeadGreen.bmp: redo
	cd ..\.. & $(SCREENSHOTEXE) --NoRoom --Redress --size=512 --headError=0.0  $(@F)
	move ..\..\$(@F) Pictures\.

Pictures\StraightenHeadCyan.bmp: redo
	cd ..\.. & $(SCREENSHOTEXE)  --NoRoom --Redress --size=512 --headError=3.0  $(@F)
	move ..\..\$(@F) Pictures\.

Pictures\StraightenHeadYellow.bmp: redo
	cd ..\.. & $(SCREENSHOTEXE)  --NoRoom --Redress --size=512 --headError=10.0  $(@F)
	move ..\..\$(@F) Pictures\.

Pictures\StraightenHeadRed.bmp: redo
	cd ..\.. & $(SCREENSHOTEXE)  --NoRoom --Redress --size=512 --headError=30.0  $(@F)
	move ..\..\$(@F) Pictures\.


.SUFFIXES: .html .md .tex
.md.html:
	$(PREPROCESSOR) $(PREPROCESSOR_OPTIONS) $? | $(PANDOC) $(PANDOC_OPTIONS)  -o $@
