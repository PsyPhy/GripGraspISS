
CONVERTER = ..\..\Executables\tiff2bmp.exe --height=512 --width=512 --quiet

STATICBMPS =	metal.bmp NightSky.bmp Rockwall.bmp 
FROMTIFFS = ReadyToStart.bmp BlockCompleted.bmp HeadMisalignment.bmp TimeLimit.bmp
DESTINATION = ..\..\Bmp

all:	$(FROMTIFFS) $(STATICBMPS)
	copy /Y /V *.bmp $(DESTINATION)
	echo BMPs %date% %time% > $@

metal.bmp: StaticBitmaps\metal.bmp
	copy /Y StaticBitmaps\metal.bmp .

NightSky.bmp: StaticBitmaps\NightSky.bmp
	copy /Y StaticBitmaps\NightSky.bmp .

Rockwall.bmp: StaticBitmaps\Rockwall.bmp
	copy /Y StaticBitmaps\Rockwall.bmp .

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

