### Keep the GRASP top level execution environment curent.

## This makefile cannot be executed directly because BUILDCONFIGURATION is not defined.
## We define makefiles specific to each configuration (Release and Debug) that define
## BUILDCONFIGURATION and then include this generic makefile to do the work.

EXECUTABLES = \
	..\$(BUILDCONFIGURATION)\GripMMI.exe \
	..\$(BUILDCONFIGURATION)\GripMMIlite.exe \
	..\$(BUILDCONFIGURATION)\CLWSemulator.exe \
	..\$(BUILDCONFIGURATION)\DexGroundMonitorClient.exe \
	GripPacketsForSimulator.gpk \
	RunGripMMI.bat

# Runtime executables will go here.
DESTINATION=..\..\GripMMIExecutables
# The .bat file is placed in the highest level directory.
ROOT=..\..

install: $(EXECUTABLES) InstallGripMMI.mak 
	-rmdir /S /Q $(DESTINATION)
	mkdir $(DESTINATION)
	copy ..\$(BUILDCONFIGURATION)\GripMMI.exe $(DESTINATION)
	copy ..\$(BUILDCONFIGURATION)\GripMMIlite.exe $(DESTINATION)
	copy ..\$(BUILDCONFIGURATION)\CLWSemulator.exe $(DESTINATION)
	copy ..\$(BUILDCONFIGURATION)\DexGroundMonitorClient.exe $(DESTINATION)
	copy GripPacketsForSimulator.gpk $(DESTINATION)
	-copy ..\DLLs\glut32.dll $(DESTINATION)
	copy RunGripMMI.bat $(ROOT)
	echo $(BUILDCONFIGURATION) %date% %time% > $@
