### Create runtime package for Aiming tool.

EXECUTABLES = \
	..\$(BUILDCONFIGURATION)\GripAimingTool.exe \
	..\$(BUILDCONFIGURATION)\GraspTrackerDaemon.exe \
	..\$(BUILDCONFIGURATION)\WaitForCodaDaemon.exe \
	..\Grasp\Grasp.ini.src \
	RunAiming.bat.src 

ROOT=..\..
DESTINATION = $(ROOT)\Executables

install: $(EXECUTABLES) InstallAiming.mak 
	-rmdir /S /Q $(DESTINATION)
	mkdir $(DESTINATION)
	-copy ..\DLLs\*.dll $(DESTINATION)
	copy ..\$(BUILDCONFIGURATION)\GripAimingTool.exe $(DESTINATION)
	copy ..\$(BUILDCONFIGURATION)\GraspTrackerDaemon.exe $(DESTINATION)
	copy ..\$(BUILDCONFIGURATION)\WaitForCodaDaemon.exe $(DESTINATION)
	copy RunAiming.bat.src $(ROOT)\RunAiming.bat
	copy Hook2.bat.src $(ROOT)\Hook2.bat
