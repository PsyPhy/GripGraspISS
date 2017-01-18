### Keep the GRASP top level execution environment curent.

## This makefile cannot be executed directly because BUILDCONFIGURATION is not defined.
## We define makefiles specific to each configuration (Release and Debug) that define
## BUILDCONFIGURATION and then include this generic makefile to do the work.

EXECUTABLES = \
	..\$(BUILDCONFIGURATION)\GraspMMIMirror.exe \
	..\$(BUILDCONFIGURATION)\GraspMMIDataPlots.exe \
	..\$(BUILDCONFIGURATION)\CLWSemulator.exe \
	..\$(BUILDCONFIGURATION)\DexGroundMonitorClient.exe \
	GraspPacketsForSimulator.gpk \
	RunGraspMMI.bat

DESTINATION=..\..\GraspMMIExecutables

install: $(EXECUTABLES) InstallGraspMMI.mak 
	-rmdir /S /Q $(DESTINATION)
	mkdir $(DESTINATION)
	copy ..\$(BUILDCONFIGURATION)\GraspMMIMirror.exe $(DESTINATION)
	copy ..\$(BUILDCONFIGURATION)\GraspMMIDataPlots.exe $(DESTINATION)
	copy ..\$(BUILDCONFIGURATION)\CLWSemulator.exe $(DESTINATION)
	copy ..\$(BUILDCONFIGURATION)\DexGroundMonitorClient.exe $(DESTINATION)
	copy GraspPacketsForSimulator.gpk $(DESTINATION)
	-copy ..\DLLs\*.dll $(DESTINATION)
	copy RunGraspMMI.bat ..\..
	echo $(BUILDCONFIGURATION) %date% %time% > $@
