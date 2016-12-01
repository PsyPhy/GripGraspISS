### Keep the GRASP top level execution environment current.

EXECUTABLES = \
	..\$(BUILDCONFIGURATION)\Grasp.exe \
	..\Grasp\Grasp.bat \
	..\Grasp\GraspPackData.bat \
	..\GraspGUI\Grasp.ico ..\$(BUILDCONFIGURATION)\GraspGUI.exe \
	..\$(BUILDCONFIGURATION)\GraspHardwareStatus.exe \
	..\GraspTrackerDaemon\StartGraspTrackerDaemon.bat \
	..\$(BUILDCONFIGURATION)\GraspTrackerDaemon.exe \
	..\$(BUILDCONFIGURATION)\WaitForCodaDaemon.exe \
	..\$(BUILDCONFIGURATION)\PsyPhyOculusDemo.exe \
	..\$(BUILDCONFIGURATION)\OculusMouse.exe \
	..\$(BUILDCONFIGURATION)\GraspScreenshots.exe \
	..\$(BUILDCONFIGURATION)\AlignToRigidBodyGUI.exe \
	..\$(BUILDCONFIGURATION)\TaskProcessUnitTester.exe  \
	..\$(BUILDCONFIGURATION)\CreateRigidBodyModelFile.exe \
	..\$(BUILDCONFIGURATION)\CharnwoodAlignment.exe \
	..\$(BUILDCONFIGURATION)\StartOculus.exe \
	..\$(BUILDCONFIGURATION)\OculusStartupCheck.exe \
	GraspPacketsForSimulator.gpk \
	LoadCodaCals.bat \
	RunGRASP.bat 

install: $(EXECUTABLES) InstallGrasp.mak 
	-rmdir /S /Q ..\..\Executables
	-del /F /Q ..\..\RunGraspMMI.bat
	mkdir ..\..\Executables
	copy ..\$(BUILDCONFIGURATION)\OculusMouse.exe ..\..\Executables
	copy ..\$(BUILDCONFIGURATION)\Grasp.exe ..\..\Executables
	copy ..\Grasp\Grasp.bat ..\..\Executables
	copy ..\Grasp\GraspPackData.bat ..\..\Executables
	copy ..\$(BUILDCONFIGURATION)\GraspGUI.exe ..\..\Executables
	copy ..\$(BUILDCONFIGURATION)\GraspMMI.exe ..\..\Executables
	copy ..\GraspGUI\Grasp.ico ..\..\Executables
	copy ..\$(BUILDCONFIGURATION)\AlignToRigidBodyGUI.exe ..\..\Executables
	copy ..\$(BUILDCONFIGURATION)\TaskProcessUnitTester.exe ..\..\Executables
	copy ..\$(BUILDCONFIGURATION)\CreateRigidBodyModelFile.exe ..\..\Executables
	copy ..\$(BUILDCONFIGURATION)\PsyPhyOculusDemo.exe ..\..\Executables
	copy ..\$(BUILDCONFIGURATION)\GraspTrackerDaemon.exe ..\..\Executables
	copy ..\GraspTrackerDaemon\StartGraspTrackerDaemon.bat ..\..\Executables
	copy ..\$(BUILDCONFIGURATION)\GraspHardwareStatus.exe ..\..\Executables
	copy ..\$(BUILDCONFIGURATION)\CharnwoodAlignment.exe ..\..\Executables
	copy ..\$(BUILDCONFIGURATION)\WaitForCodaDaemon.exe ..\..\Executables
	copy ..\$(BUILDCONFIGURATION)\StartOculus.exe ..\..\Executables
	copy ..\$(BUILDCONFIGURATION)\OculusStartupCheck.exe ..\..\Executables
	copy ..\$(BUILDCONFIGURATION)\CLWSemulator.exe ..\..\Executables
	copy ..\$(BUILDCONFIGURATION)\DexGroundMonitorClient.exe ..\..\Executables
	copy LoadCodaCals.bat ..\..\Executables
	copy RunGRASP.bat ..\..
	copy GraspPacketsForSimulator.gpk ..\..\Executables
	-mkdir ..\..\Executables\CodaCalFiles
	-copy ..\CodaCalFiles\EM ..\..\Executables\CodaCalFiles
	-copy ..\CodaCalFiles\FM2 ..\..\Executables\CodaCalFiles
	-copy ..\CodaCalFiles\GM ..\..\Executables\CodaCalFiles
	-copy ..\CodaCalFiles\SM ..\..\Executables\CodaCalFiles
	-copy ..\DLLs\*.dll ..\..\Executables
	echo $(BUILDCONFIGURATION) %date% %time% > $@
