### Keep the GRASP top level execution environment curent.

EXECUTABLES = \
	..\$(BUILDCONFIGURATION)\GraspMMI.exe \
	GraspPacketsForSimulator.gpk \
	RunGraspMMI.bat

DESTINATION=..\..\GraspMMIExecutables

install: $(EXECUTABLES) InstallGraspMMI.mak 
	-rmdir /S /Q $(DESTINATION)
	mkdir $(DESTINATION)
	copy ..\$(BUILDCONFIGURATION)\GraspMMI.exe $(DESTINATION)
	copy ..\$(BUILDCONFIGURATION)\CLWSemulator.exe $(DESTINATION)
	copy ..\$(BUILDCONFIGURATION)\DexGroundMonitorClient.exe $(DESTINATION)
	copy RunGraspMMI.bat ..\..
	copy GraspPacketsForSimulator.gpk $(DESTINATION)
	-copy ..\DLLs\*.dll $(DESTINATION)
	echo $(BUILDCONFIGURATION) %date% %time% > $@
