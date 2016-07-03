###	
### Create the .bmp files needed by the Grasp VR environment.
###

# A program that converts .tiff to .bmp.
# This is used to convert images generated in PowerPoint into usable .bmp files.
# In theory, PowerPoint can save directly in .bmp format, but it gives strange results for me.
CONVERTER = ..\..\Executables\tiff2bmp.exe --height=512 --width=512

# Define the path to the pandoc.exe program that does the conversion.
PANDOC=pandoc.exe
PANDOC_OPTIONS=

# This is the list of btimaps generated from Powerpoint. In general, they are messages that are
# presented in a circular format with the VR presentation.
FROMTIFFS = ReadyToStart.bmp BlockCompleted.bmp HeadMisalignment.bmp RaiseArm.bmp LowerArm.bmp TimeoutTilt.bmp TimeoutResponse.bmp TimeLImit.bmp

# The GRASP VR world requires a few bitmaps to decorate the walls. We keep copies in the source file tree
# then copy as needed to the exection tree. That way, by maintaining this makefile up to date, only those
# .bmp files that are actually needed get distributed in the release repositories.
# Static files (ones that are not automatically generated here from other sources) are
# kept in a subdirectory. That way we can do a del *.bmp to clean the project directory.
STATICBMPS =	StaticBitmaps\metal.bmp StaticBitmaps\NightSky.bmp StaticBitmaps\Rockwall.bmp 

# The bitmaps are moved to this directory to be included in the execution environment.
DESTINATION = ..\..\Bmp

# This is where we can find the tools need to do the build.
EXECUTABLES=..\..\Executables

all:	$(FROMTIFFS) $(STATICBMPS) GraspVRMessages.pdf
	copy /Y /V *.bmp $(DESTINATION)
	copy /Y StaticBitmaps\metal.bmp $(DESTINATION)
	copy /Y StaticBitmaps\NightSky.bmp $(DESTINATION)
	copy /Y StaticBitmaps\Rockwall.bmp $(DESTINATION)
	echo BMPs %date% %time% > $@

# Combine all the messages into a pdf document. This is useful when it comes time to generate the documention for the program.
GraspVRMessages.pdf: *.bmp.html
	type $(**) > $(@B).html
	$(EXECUTABLES)\wkhtmltopdf.exe  --page-size A6  --default-header --header-left "Apendix II: Grasp VR Messages" --header-font-size 8 --header-spacing 5 --margin-bottom 20 $(@B).html $@

# The next set of bitmaps are generated from a Powerpoint file entitled GraspCircularPrompts.pptx.
# To genearate the bitmaps, you must first save the latest version of the Powerpoint file as .bmp image files
# (see 'Save as images ...' or 'Enregistrer comme images ...' in the Powerpoint File menu).
# Here we convert the individual images into bitmap files. Note that the images created by Powerpoint have 
# filenames based on the slide number in the file. If you change the order of the slides or insert any slides
# you have to edit here below to link each slide to the correct bitmap filename.

ReadyToStart.tiff: GraspCircularPrompts\Diapositive1.tiff
	copy $(**) $@

BlockCompleted.tiff: GraspCircularPrompts\Diapositive2.tiff
	copy $(**) $@

HeadMisalignment.tiff: GraspCircularPrompts\Diapositive3.tiff
	copy $(**) $@

TimeLimit.tiff: GraspCircularPrompts\Diapositive4.tiff
	copy $(**) $@

LowerArm.tiff: GraspCircularPrompts\Diapositive5.tiff
	copy $(**) $@

RaiseArm.tiff: GraspCircularPrompts\Diapositive6.tiff
	copy $(**) $@

TimeoutTilt.tiff: GraspCircularPrompts\Diapositive7.tiff
	copy $(**) $@

TimeoutResponse.tiff: GraspCircularPrompts\Diapositive8.tiff
	copy $(**) $@

.SUFFIXES: .tiff .bmp

# Here are the commands used to convert from a .tiff to a .bmp
# We also create an .html version that is used above to generate a pdf file with all the messages.
# project to create pdf files showing all the screens seen by the subject.
.tiff.bmp: 
	$(CONVERTER) --input=$?
	echo ^<img src="$@" size=50 /^> >$@.html
