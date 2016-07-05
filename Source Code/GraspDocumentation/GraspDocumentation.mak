### This Makefile creates some documentation automatically.
### It uses the GraspVR package to create images that are up to date.

DOCUMENTATION_DESTINATION=..\..\Documentation
EXECUTABLES=..\..\Executables
INSTRUCTIONS=..\..\Instructions
GUI=GUIScreenshots
VRMESSAGES=..\GraspBMPs
HERE = $(CD)

SCREENSHOTS = ready.bmp leftFar.bmp leftCloser.bmp leftNear.bmp leftGood.bmp vTarget.bmp kTargetFar.bmp kTargetNear.bmp kTargetGood.bmp vTool.bmp kTool.bmp trialCompleted.bmp blockCompleted.bmp
GUISHOTS = $(GUI)\Alert.png  $(GUI)\Execute.png  $(GUI)\Instruction.png   $(GUI)\Success.png   $(GUI)\Login.png  $(GUI)\SelectSubsession.png  $(GUI)\StartTask.png 

install: GraspScreens.pdf  GraspVRScenes.pdf GraspGUI.pdf
	echo %date% %time% > $@

ready.bmp: 
	cd ..\.. & Executables\GraspScreenshots.exe --head=0.0 --ReadyToStart  $@
	move ..\..\$@ .
	
blockCompleted.bmp: 
	cd ..\.. & Executables\GraspScreenshots.exe --head=0.0 --BlockCompleted  $@
	move ..\..\$@ .

trialCompleted.bmp: 
	cd ..\.. & Executables\GraspScreenshots.exe --head=0.0 --TrialCompleted  $@
	move ..\..\$@ .

kTargetFar.bmp:
	cd ..\.. & Executables\GraspScreenshots.exe --head=0.0 --headError=0.0 --hand=35.0 --handError=15.0 --kkTool --handArrow  $@
	move ..\..\$@ .
	
kTargetNear.bmp:
	cd ..\.. & Executables\GraspScreenshots.exe --head=0.0 --headError=0.0 --hand=24.0 --handError=4.0 --kkTool  $@
	move ..\..\$@ .

kTargetGood.bmp:
	cd ..\.. & Executables\GraspScreenshots.exe --head=0.0 --headError=0.0 --hand=20.0 --handError=0.0 --kkTool  $@
	move ..\..\$@ .

vTarget.bmp:
	cd ..\.. & Executables\GraspScreenshots.exe --head=0.0 --headError=0.0 --target=20.0  $@
	move ..\..\$@ .

vTool.bmp:
	cd ..\.. & Executables\GraspScreenshots.exe --head=15.0 --headError=0.0 --vTool  $@
	move ..\..\$@ .

kTool.bmp:
	cd ..\.. & Executables\GraspScreenshots.exe --head=15.0 --headError=0.0 --kTool  $@
	move ..\..\$@ .

leftFar.bmp:
	cd ..\.. & Executables\GraspScreenshots.exe --head=-20.0 --headError=20.0 --headArrowRight  $@
	move ..\..\$@ .
	
leftCloser.bmp:
	cd ..\.. & Executables\GraspScreenshots.exe --head=-5.0 --headError=5.0   $@
	move ..\..\$@ .

leftNear.bmp:
	cd ..\.. & Executables\GraspScreenshots.exe --head=-3.0 --headError=3.0   $@
	move ..\..\$@ .

leftGood.bmp:
	cd ..\.. & Executables\GraspScreenshots.exe --head=0.0 --headError=0.0   $@
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

# Create a document describing the VR scenes.
GraspVRScenes.html: $(SCREENSHOTS) GraspVRScenes.md
	$(PREPROCESSOR) $(PREPROCESSOR_OPTIONS)  GraspVRScenes.md | $(PANDOC) $(PANDOC_OPTIONS)  -o $@

GraspVRScenes.pdf: GraspVRScenes.html
	$(EXECUTABLES)\wkhtmltopdf.exe  --page-size A5  --default-header --header-left "VR Scenes" --header-font-size 8 --margin-bottom 0 --margin-top 10  --header-spacing 2 $(**) $@

# Create a document describing the GUI pages.
GraspGUI.html: $(GUISHOTS) GraspGUI.md
	$(PREPROCESSOR) $(PREPROCESSOR_OPTIONS)  GraspGUI.md | $(PANDOC) $(PANDOC_OPTIONS)  -o $@

GraspGUI.pdf: GraspGUI.html
	$(EXECUTABLES)\wkhtmltopdf.exe  --page-size A5  --default-header --header-left "GUI Operation" --header-font-size 8 --margin-bottom 10 --margin-top 10  --header-spacing 2 $(**) $@

# The idea here is to create a single document to show all the instruction screens.
GraspInstructionScreens.pdf: $(INSTRUCTIONS)\*.instruction.html
	$(EXECUTABLES)\wkhtmltopdf.exe  --page-size A6  --default-header --header-left "Instruction Screens" --header-font-size 8 --header-spacing 5 --margin-bottom 10 $(**) $@

GraspPromptScreens.pdf: $(INSTRUCTIONS)\*.prompt.html
	$(EXECUTABLES)\wkhtmltopdf.exe  --page-size A6  --default-header --header-left "Prompt Screens" --header-font-size 8 --header-spacing 5 --margin-bottom 10 $(**) $@

GraspStatusScreens.pdf: $(INSTRUCTIONS)\*.status.html
	$(EXECUTABLES)\wkhtmltopdf.exe  --page-size A6  --default-header --header-left "Status and Alert Screens" --header-font-size 8 --header-spacing 5 --margin-bottom 10 $(**) $@

# Here we combine the Instruction screens and the VR prompts into a single document.
# The file containing the VR prompts was created in the project where the bitmaps are created because it is easier to do it there.
GraspScreens.pdf: GraspGUI.pdf GraspInstructionScreens.pdf GraspPromptScreens.pdf GraspStatusScreens.pdf  GraspVRScenes.pdf $(VRMESSAGES)\GraspVRMessages.pdf 
	 $(EXECUTABLES)\pdftk.exe $(**) cat output $@
	 copy $@ $(DOCUMENTATION_DESTINATION)