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

DESTINATION=..\..\GripMMIExecutables

install: $(EXECUTABLES) InstallGripMMI.mak 
	-rmdir /S /Q $(DESTINATION)
	mkdir $(DESTINATION)
	copy ..\$(BUILDCONFIGURATION)\GripMMI.exe $(DESTINATION)
	copy ..\$(BUILDCONFIGURATION)\GripMMIlite.exe $(DESTINATION)
	copy ..\$(BUILDCONFIGURATION)\CLWSemulator.exe $(DESTINATION)
	copy ..\$(BUILDCONFIGURATION)\DexGroundMonitorClient.exe $(DESTINATION)
	copy GripPacketsForSimulator.gpk $(DESTINATION)
	-copy ..\DLLs\glut32.dll $(DESTINATION)
	copy RunGripMMI.bat ..\..
	echo $(BUILDCONFIGURATION) %date% %time% > $@
