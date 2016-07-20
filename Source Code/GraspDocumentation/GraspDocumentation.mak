### This Makefile creates some documentation automatically.
### It uses the GraspVR package to create images that are up to date.

DOCUMENTATION_DESTINATION=..\..\Documentation
EXECUTABLES=..\..\Executables
INSTRUCTIONS=..\..\Instructions
GUI=GUIScreenshots
VRMESSAGES=..\GraspBMPs
HERE = $(CD)
SCREENSHOTEXE="Source Code\Debug\GraspScreenshots.exe" --size=512
DOCUMENTS=GraspScreenReview.pdf  \
	GraspGUIScreens.pdf GraspGUIScreens.docx \
	InstallSeated.pdf InstallFloating.pdf ManualManualInstructions.pdf VisualManualInstructions.pdf VisualVisualInstructions.pdf \
	VisualVisualScenes.pdf VisualManualScenes.pdf  ManualManualScenes.pdf GraspIntro.pdf

SCREENSHOTS = ready.bmp leftFar.bmp leftCloser.bmp leftNear.bmp good.bmp rightNear.bmp rightCloser.bmp rightFar.bmp \
	vTarget.bmp kTargetFar.bmp kTargetNear.bmp kTargetGood.bmp vTool.bmp kTool.bmp trialCompleted.bmp blockCompleted.bmp
GUISHOTS = $(GUI)\Alert.png  $(GUI)\Execute.png  $(GUI)\Instruction.png   $(GUI)\Success.png   $(GUI)\Login.png  $(GUI)\SelectSubsession.png  $(GUI)\StartTask.png 

install: $(DOCUMENTS)
	rmdir /S /Q $(DOCUMENTATION_DESTINATION)
	mkdir $(DOCUMENTATION_DESTINATION)
	copy *.pdf $(DOCUMENTATION_DESTINATION)
	mkdir $(DOCUMENTATION_DESTINATION)\Images
	copy GUIScreenshots\*  $(DOCUMENTATION_DESTINATION)\Images
	copy *.bmp $(DOCUMENTATION_DESTINATION)\Images
	copy $(VRMESSAGES)\*.bmp $(DOCUMENTATION_DESTINATION)\Images
	echo %date% %time% > $@

ready.bmp: 
	cd ..\.. & $(SCREENSHOTEXE) --head=0.0 --ReadyToStart  $@
	move ..\..\$@ .
	
blockCompleted.bmp:  
	cd ..\.. & $(SCREENSHOTEXE) --head=0.0 --BlockCompleted  $@
	move ..\..\$@ .

trialCompleted.bmp:  
	cd ..\.. & $(SCREENSHOTEXE) --head=0.0 --TrialCompleted  $@
	move ..\..\$@ .

kTargetFar.bmp:  
	cd ..\.. & $(SCREENSHOTEXE) --targetSphere --head=0.0 --headError=0.0 --hand=35.0 --handError=15.0 --kkTool --handArrow  $@
	move ..\..\$@ .
	
kTargetNear.bmp:  
	cd ..\.. & $(SCREENSHOTEXE) --targetSphere --head=0.0 --headError=0.0 --hand=24.0 --handError=4.0 --kkTool  $@
	move ..\..\$@ .

kTargetGood.bmp:  
	cd ..\.. & $(SCREENSHOTEXE) --targetSphere --head=0.0 --headError=0.0 --hand=20.0 --handError=0.0 --kkTool  $@
	move ..\..\$@ .

vTarget.bmp:  
	cd ..\.. & $(SCREENSHOTEXE) --head=0.0 --headError=0.0 --target=20.0  $@
	move ..\..\$@ .

vTool.bmp:  
	cd ..\.. & $(SCREENSHOTEXE) --head=15.0 --headError=0.0 --vTool  $@
	move ..\..\$@ .

kTool.bmp:  
	cd ..\.. & $(SCREENSHOTEXE) --targetSphere --head=15.0 --headError=0.0 --kTool  $@
	move ..\..\$@ .

leftFar.bmp:  
	cd ..\.. & $(SCREENSHOTEXE) --head=-20.0 --headError=20.0 --headArrowRight  $@
	move ..\..\$@ .
	
leftCloser.bmp:  
	cd ..\.. & $(SCREENSHOTEXE) --head=-5.0 --headError=6.0   $@
	move ..\..\$@ .

leftNear.bmp:  
	cd ..\.. & $(SCREENSHOTEXE) --head=-3.0 --headError=3.0   $@
	move ..\..\$@ .

good.bmp:  
	cd ..\.. & $(SCREENSHOTEXE) --head=0.0 --headError=0.0   $@
	move ..\..\$@ .	

rightNear.bmp:  
	cd ..\.. & $(SCREENSHOTEXE) --head=3.0 --headError=-3.0   $@
	move ..\..\$@ .

rightCloser.bmp:  
	cd ..\.. & $(SCREENSHOTEXE) --head=5.0 --headError=-6.0   $@
	move ..\..\$@ .

rightFar.bmp:  
	cd ..\.. & $(SCREENSHOTEXE) --head=20.0 --headError=-20.0 --headArrowLeft  $@
	move ..\..\$@ .


# Define the path to the pandoc.exe program that does the conversion.
PANDOC=pandoc.exe
PANDOC_OPTIONS=

# Define the preprocessor.
PREPROCESSOR=cl.exe
# The preprocessor that we are using is actually the Visual C++ compiler.
# The /EP option tells the compiler to preprocess only. 
# The /FI forces the inclusion of the preprocessor macros that we have defined. 
PREPROCESSOR_OPTIONS=/EP /nologo /FI macros.h

GraspIntro.html: GraspIntro.md GraspDocumentation.mak
	$(PREPROCESSOR) $(PREPROCESSOR_OPTIONS)  GraspIntro.md | $(PANDOC) $(PANDOC_OPTIONS)  -o $@

GraspIntro.pdf: GraspIntro.html
	$(EXECUTABLES)\wkhtmltopdf.exe  --minimum-font-size 24 --page-size A4  --default-header --header-left "Introduction" --header-font-size 12 --margin-bottom 25 --margin-top 35  --header-spacing 10 GraspIntro.html $@

VisualVisualScenes.html: VisualVisualScenes.md $(SCREENSHOTS)
	$(PREPROCESSOR) $(PREPROCESSOR_OPTIONS)  VisualVisualScenes.md | $(PANDOC) $(PANDOC_OPTIONS)  -o $@

VisualVisualScenes.pdf: VisualVisualScenes.html
	$(EXECUTABLES)\wkhtmltopdf.exe  --minimum-font-size 24 --page-size A4  --default-header --header-left "Visual-Visual VR Sequence" --header-font-size 12 --margin-bottom 25 --margin-top 35  --header-spacing 10 VisualVisualScenes.html $@

VisualManualScenes.html: VisualManualScenes.md $(SCREENSHOTS)
	$(PREPROCESSOR) $(PREPROCESSOR_OPTIONS)  VisualManualScenes.md | $(PANDOC) $(PANDOC_OPTIONS)  -o $@

VisualManualScenes.pdf: VisualManualScenes.html
	$(EXECUTABLES)\wkhtmltopdf.exe  --minimum-font-size 24 --page-size A4  --default-header --header-left "Visual-Manual VR Sequence" --header-font-size 12 --margin-bottom 25 --margin-top 35  --header-spacing 10 VisualManualScenes.html $@

ManualManualScenes.html: ManualManualScenes.md $(SCREENSHOTS)
	$(PREPROCESSOR) $(PREPROCESSOR_OPTIONS)  ManualManualScenes.md | $(PANDOC) $(PANDOC_OPTIONS)  -o $@

ManualManualScenes.pdf: ManualManualScenes.html
	$(EXECUTABLES)\wkhtmltopdf.exe  --minimum-font-size 24 --page-size A4  --default-header --header-left "Manual-Manual VR Sequence" --header-font-size 12 --margin-bottom 25 --margin-top 35  --header-spacing 10  ManualManualScenes.html $@

# Create a document describing the GUI pages.
GraspGUIScreens.html: $(GUISHOTS) GraspGUIScreens.md GraspDocumentation.mak
	$(PREPROCESSOR) $(PREPROCESSOR_OPTIONS)  GraspGUIScreens.md | $(PANDOC) $(PANDOC_OPTIONS)  -o $@

GraspGUIScreens.docx: $(GUISHOTS) GraspGUIScreens.html GraspDocumentation.mak
	$(PREPROCESSOR) $(PREPROCESSOR_OPTIONS)  GraspGUIScreens.md | $(PANDOC) $(PANDOC_OPTIONS) --to=docx -o $@

GraspGUIScreens.pdf: GraspGUIScreens.html
	$(EXECUTABLES)\wkhtmltopdf.exe  --page-size A4  --default-header --header-left "GRASP GUI Operation" --header-font-size 8 --margin-bottom 10 --margin-top 20  --header-spacing 4 $(**) $@

# The idea here is to create documents to show all the instruction screens, all the prompt screen and all the status screens, one for each category.
# Note that because of the trick I am using in order to wild card the input files to wkhtmltopdf.exe, I cannot make these outputs dependent
# on the makefile. So if you change this makefile you have to remember to rebuild.

GraspAllInstructionScreens.pdf: $(INSTRUCTIONS)\*.instruction.html  
	$(EXECUTABLES)\wkhtmltopdf.exe  --page-size A6  --default-header --header-left "GRASP Instruction Screens" --header-font-size 8 --margin-bottom 10 --margin-top 15  --header-spacing 4 $(**) $@

GraspAllPromptScreens.pdf: $(INSTRUCTIONS)\*.prompt.html
	$(EXECUTABLES)\wkhtmltopdf.exe  --page-size A6  --default-header --header-left "GRASP Prompt Screens" --header-font-size 8 --header-spacing 5 --margin-top 15  --margin-bottom 10 $(**) $@

GraspAllStatusScreens.pdf: $(INSTRUCTIONS)\*.status.html
	$(EXECUTABLES)\wkhtmltopdf.exe  --page-size A6  --default-header --header-left "Status and Alert Screens" --header-font-size 8 --header-spacing 5 --margin-top 15  --margin-bottom 10 $(**) $@

INSTALL_SEATED = $(INSTRUCTIONS)\CodaAlignSeated.prompt.html  $(INSTRUCTIONS)\StepNormalFinish.status.html $(INSTRUCTIONS)\StepErrorFinish.status.html
InstallSeated.pdf: $(INSTALL_SEATED) 
	$(EXECUTABLES)\wkhtmltopdf.exe  --page-size A6  --default-header --header-left "Install Seated Instruction Sequence" --header-font-size 8 --header-spacing 5 --margin-top 15  --margin-bottom 10 $(**) $@	

INSTALL_FLOATING = $(INSTRUCTIONS)\CodaAlignFloating.prompt.html  $(INSTRUCTIONS)\StepNormalFinish.status.html $(INSTRUCTIONS)\StepErrorFinish.status.html
InstallFloating.pdf: $(INSTALL_FLOATING) 
	$(EXECUTABLES)\wkhtmltopdf.exe  --page-size A6  --default-header --header-left "Install Floating Instruction Sequence" --header-font-size 8 --header-spacing 5 --margin-top 15  --margin-bottom 10 $(**) $@	

MM = $(INSTRUCTIONS)\00IntroK-K.instruction.html  $(INSTRUCTIONS)\01StraightenHead.instruction.html  \
	 $(INSTRUCTIONS)\02TargetK.instruction.html   $(INSTRUCTIONS)\03TiltHead.instruction.html \
	 $(INSTRUCTIONS)\04RespondK.instruction.html  $(INSTRUCTIONS)\05Feedback.instruction.html 

ManualManualInstructions.pdf: $(MM) 
	$(EXECUTABLES)\wkhtmltopdf.exe  --page-size A6  --default-header --header-left "Manual-Manual Instruction Sequence" --header-font-size 8 --header-spacing 5 --margin-top 15  --margin-bottom 10 $(**) $@	

VM = $(INSTRUCTIONS)\00IntroV-K.instruction.html  $(INSTRUCTIONS)\01StraightenHead.instruction.html  \
	 $(INSTRUCTIONS)\02TargetV.instruction.html   $(INSTRUCTIONS)\03TiltHead.instruction.html \
	 $(INSTRUCTIONS)\04RespondK.instruction.html  $(INSTRUCTIONS)\05Feedback.instruction.html 

VisualManualInstructions.pdf: $(VM) 
	$(EXECUTABLES)\wkhtmltopdf.exe  --page-size A6  --default-header --header-left "Visual-Manual Instruction Sequence" --header-font-size 8 --header-spacing 5 --margin-top 15  --margin-bottom 10 $(**) $@	

VV = $(INSTRUCTIONS)\00IntroV-V.instruction.html  $(INSTRUCTIONS)\01StraightenHead.instruction.html  \
	 $(INSTRUCTIONS)\02TargetV.instruction.html   $(INSTRUCTIONS)\03TiltHead.instruction.html \
	 $(INSTRUCTIONS)\04RespondV.instruction.html  $(INSTRUCTIONS)\05Feedback.instruction.html 

VisualVisualInstructions.pdf: $(VV) 
	$(EXECUTABLES)\wkhtmltopdf.exe  --page-size A6  --default-header --header-left "Visual-Visual Instruction Sequence" --header-font-size 8 --header-spacing 5 --margin-top 15  --margin-bottom 10 $(**) $@	

GraspVRMessages.pdf: $(VRMESSAGES)\GraspVRMessages.pdf
	copy $(VRMESSAGES)\GraspVRMessages.pdf .

# Here we combine the Instruction screens and the VR prompts into a single document.
# The file containing the VR prompts (GraspVRMessages.pdf) was created in the project 
# where the bitmaps are created because it is easier to do it there.

COMBINED = GraspIntro.pdf GraspGUIScreens.pdf \
	InstallSeated.pdf InstallFloating.pdf \
	VisualVisualInstructions.pdf VisualVisualScenes.pdf \
	ManualManualInstructions.pdf ManualManualScenes.pdf \
	VisualManualInstructions.pdf VisualManualScenes.pdf \
	GraspVRMessages.pdf

GraspScreenReview.pdf: $(COMBINED) $(VRMESSAGES)\GraspVRMessages.pdf 
	 $(EXECUTABLES)\pdftk.exe $(**) cat output $@

.SUFFIXES: .html .md .tex
.md.html:
	$(PREPROCESSOR) $(PREPROCESSOR_OPTIONS) $? | $(PANDOC) $(PANDOC_OPTIONS)  -o $@
