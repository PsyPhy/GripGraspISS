### Create runtime package for Aiming tool.

EXECUTABLES = \
	..\$(BUILDCONFIGURATION)\GripAimingTool.exe \
	..\$(BUILDCONFIGURATION)\GraspTrackerDaemon.exe \
	..\$(BUILDCONFIGURATION)\WaitForCodaDaemon.exe \
	..\Grasp\Grasp.ini.src \
	..\GripAimingTool\GripAimingTool.ini.src \
	RunAiming.bat.src 

ROOT=..\..
DESTINATION = $(ROOT)\Executables

install: $(EXECUTABLES) InstallAiming.mak 
	-rmdir /S /Q $(DESTINATION)
	mkdir $(DESTINATION)
	-copy ..\DLLs\*.dll $(DESTINATION)
	-rmdir /S /Q $(ROOT)\InitFiles
	mkdir $(ROOT)\InitFiles
	copy ..\InitFiles\*.* $(ROOT)\InitFiles
	copy $(ROOT)\InitFiles\EpmRTnet.ini.FM2 $(ROOT)\EpmRTnet.ini
	copy ..\GripAimingTool\GripAimingTool.ini.src $(ROOT)\GripAimingTool.ini
	copy ..\$(BUILDCONFIGURATION)\GripAimingTool.exe $(DESTINATION)
	copy ..\$(BUILDCONFIGURATION)\GraspTrackerDaemon.exe $(DESTINATION)
	copy ..\$(BUILDCONFIGURATION)\WaitForCodaDaemon.exe $(DESTINATION)
	copy RunAiming.bat.src $(ROOT)\RunAiming.bat
	copy Hook2.bat.src $(ROOT)\Hook2.bat
