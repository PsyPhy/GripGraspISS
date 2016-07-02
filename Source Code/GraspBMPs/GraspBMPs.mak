###	
### Create the .bmp files needed by the Grasp VR environment.
###

# A program that converts .tiff to .bmp.
# This is used to convert images generated in PowerPoint into usable .bmp files.
# In theory, PowerPoint can save directly in .bmp format, but it gives strange results for me.
CONVERTER = ..\..\Executables\tiff2bmp.exe --height=512 --width=512

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

all:	$(FROMTIFFS) $(STATICBMPS)
	copy /Y /V *.bmp $(DESTINATION)
	copy /Y StaticBitmaps\metal.bmp $(DESTINATION)
	copy /Y StaticBitmaps\NightSky.bmp $(DESTINATION)
	copy /Y StaticBitmaps\Rockwall.bmp $(DESTINATION)
	echo BMPs %date% %time% > $@

# The next set of bitmaps are generated from a Powerpoint file entitled GraspCircularPrompts.pptx.
# To genearate the bitmaps, you must first save the latest version of the Powerpoint file as .bmp image files
# (see 'Save as images ...' or 'Enregistrer comme images ...' in the Powerpoint File menu).
# Here we convert the individual images into bitmap files. Note that the images created by Powerpoint have 
# filenames based on the slide number in the file. If you change the order of the slides or insert any slides
# you have to edit here below to link each slide to the correct bitmap filename.
ReadyToStart.bmp: GraspCircularPrompts/Diapositive1.tiff
	$(CONVERTER) --input=GraspCircularPrompts/Diapositive1.tiff 
	rename GraspCircularPrompts\Diapositive1.bmp $@
	move /Y GraspCircularPrompts\$@ .\

BlockCompleted.bmp: GraspCircularPrompts/Diapositive2.tiff
	$(CONVERTER) --input=GraspCircularPrompts/Diapositive2.tiff 
	rename GraspCircularPrompts\Diapositive2.bmp $@
	move /Y GraspCircularPrompts\$@ .\

HeadMisalignment.bmp: GraspCircularPrompts/Diapositive3.tiff
	$(CONVERTER) --input=GraspCircularPrompts/Diapositive3.tiff 
	rename GraspCircularPrompts\Diapositive3.bmp $@
	move /Y GraspCircularPrompts\$@ .\

TimeLimit.bmp: GraspCircularPrompts/Diapositive4.tiff
	$(CONVERTER) --input=GraspCircularPrompts/Diapositive4.tiff 
	rename GraspCircularPrompts\Diapositive4.bmp $@
	move /Y GraspCircularPrompts\$@ .\

RaiseArm.bmp: GraspCircularPrompts/Diapositive5.tiff
	$(CONVERTER) --input=GraspCircularPrompts/Diapositive5.tiff 
	rename GraspCircularPrompts\Diapositive5.bmp $@
	move /Y GraspCircularPrompts\$@ .\

LowerArm.bmp: GraspCircularPrompts/Diapositive6.tiff
	$(CONVERTER) --input=GraspCircularPrompts/Diapositive6.tiff 
	rename GraspCircularPrompts\Diapositive6.bmp $@
	move /Y GraspCircularPrompts\$@ .\

TimeoutTilt.bmp: GraspCircularPrompts/Diapositive7.tiff
	$(CONVERTER) --input=GraspCircularPrompts/Diapositive7.tiff 
	rename GraspCircularPrompts\Diapositive7.bmp $@
	move /Y GraspCircularPrompts\$@ .\

TimeoutResponse.bmp: GraspCircularPrompts/Diapositive8.tiff
	$(CONVERTER) --input=GraspCircularPrompts/Diapositive8.tiff 
	rename GraspCircularPrompts\Diapositive8.bmp $@
	move /Y GraspCircularPrompts\$@ .\
