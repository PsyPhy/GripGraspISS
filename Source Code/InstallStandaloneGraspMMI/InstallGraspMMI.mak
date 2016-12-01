### Keep the GRASP top level execution environment curent.

EXECUTABLES = \
	..\$(BUILDCONFIGURATION)\GraspMMI.exe \
	GraspPacketsForSimulator.gpk \
	RunGraspMMI.bat

install: $(EXECUTABLES) InstallGraspMMI.mak 
	-rmdir /S /Q ..\..\Executables
	-del /F /Q ..\..\RunGrasp.bat
	mkdir ..\..\Executables
	copy ..\$(BUILDCONFIGURATION)\GraspMMI.exe ..\..\Executables
	copy ..\$(BUILDCONFIGURATION)\CLWSemulator.exe ..\..\Executables
	copy ..\$(BUILDCONFIGURATION)\DexGroundMonitorClient.exe ..\..\Executables
	copy RunGraspMMI.bat ..\..
	copy GraspPacketsForSimulator.gpk ..\..\Executables
	-copy ..\DLLs\*.dll ..\..\Executables
	echo $(BUILDCONFIGURATION) %date% %time% > $@
