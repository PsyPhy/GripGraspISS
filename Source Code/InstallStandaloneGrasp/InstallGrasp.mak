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
	MakeRigidBodies.bat \
	RunDaemon.bat \
	RunGRASP.bat 

install: $(EXECUTABLES) InstallGrasp.mak 
	-rmdir /S /Q ..\..\GraspExecutables
	mkdir ..\..\GraspExecutables
	copy ..\$(BUILDCONFIGURATION)\OculusMouse.exe ..\..\GraspExecutables
	copy ..\$(BUILDCONFIGURATION)\Grasp.exe ..\..\GraspExecutables
	copy ..\Grasp\Grasp.bat ..\..\GraspExecutables
	copy ..\Grasp\GraspPackData.bat ..\..\GraspExecutables
	copy ..\$(BUILDCONFIGURATION)\GraspGUI.exe ..\..\GraspExecutables
	copy ..\$(BUILDCONFIGURATION)\GraspMMI.exe ..\..\GraspExecutables
	copy ..\GraspGUI\Grasp.ico ..\..\GraspExecutables
	copy ..\$(BUILDCONFIGURATION)\AlignToRigidBodyGUI.exe ..\..\GraspExecutables
	copy ..\$(BUILDCONFIGURATION)\TaskProcessUnitTester.exe ..\..\GraspExecutables
	copy ..\$(BUILDCONFIGURATION)\CreateRigidBodyModelFile.exe ..\..\GraspExecutables
	copy ..\$(BUILDCONFIGURATION)\PsyPhyOculusDemo.exe ..\..\GraspExecutables
	copy ..\$(BUILDCONFIGURATION)\GraspTrackerDaemon.exe ..\..\GraspExecutables
	copy ..\GraspTrackerDaemon\StartGraspTrackerDaemon.bat ..\..\GraspExecutables
	copy ..\$(BUILDCONFIGURATION)\GraspHardwareStatus.exe ..\..\GraspExecutables
	copy ..\$(BUILDCONFIGURATION)\CharnwoodAlignment.exe ..\..\GraspExecutables
	copy ..\$(BUILDCONFIGURATION)\WaitForCodaDaemon.exe ..\..\GraspExecutables
	copy ..\$(BUILDCONFIGURATION)\StartOculus.exe ..\..\GraspExecutables
	copy ..\$(BUILDCONFIGURATION)\OculusStartupCheck.exe ..\..\GraspExecutables
	copy ..\$(BUILDCONFIGURATION)\CLWSemulator.exe ..\..\GraspExecutables
	copy ..\$(BUILDCONFIGURATION)\DexGroundMonitorClient.exe ..\..\GraspExecutables
	copy LoadCodaCals.bat ..\..\GraspExecutables
	copy MakeRigidBodies.bat ..\..\Bdy
	copy RunGRASP.bat ..\..
	copy RunDaemon.bat ..\..
	copy GraspPacketsForSimulator.gpk ..\..\GraspExecutables
	-mkdir ..\..\GraspExecutables\CodaCalFiles
	-copy ..\CodaCalFiles\EM ..\..\GraspExecutables\CodaCalFiles
	-copy ..\CodaCalFiles\FM2 ..\..\GraspExecutables\CodaCalFiles
	-copy ..\CodaCalFiles\GM ..\..\GraspExecutables\CodaCalFiles
	-copy ..\CodaCalFiles\SM ..\..\GraspExecutables\CodaCalFiles
	-copy ..\DLLs\*.dll ..\..\GraspExecutables
	echo $(BUILDCONFIGURATION) %date% %time% > $@
