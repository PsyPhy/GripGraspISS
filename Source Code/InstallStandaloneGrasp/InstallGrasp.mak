### Keep the GRASP top level execution environment curent.

EXECUTABLES = ..\$(BUILDCONFIGURATION)\Grasp.exe ..\Grasp\Grasp.bat ..\Grasp\GraspPackData.bat \
	..\GraspGUI\Grasp.ico ..\$(BUILDCONFIGURATION)\GraspGUI.exe \
	..\$(BUILDCONFIGURATION)\GraspHardwareStatus.exe \
	..\GraspTrackerDaemon\StartGraspTrackerDaemon.bat ..\$(BUILDCONFIGURATION)\GraspTrackerDaemon.exe \
	..\$(BUILDCONFIGURATION)\WaitForCodaDaemon.exe \
	..\$(BUILDCONFIGURATION)\PsyPhyOculusDemo.exe \
	..\$(BUILDCONFIGURATION)\OculusMouse.exe \
	..\$(BUILDCONFIGURATION)\GraspScreenshots.exe \
	..\$(BUILDCONFIGURATION)\AlignToRigidBodyGUI.exe \
	..\$(BUILDCONFIGURATION)\TaskProcessUnitTester.exe  \
	..\$(BUILDCONFIGURATION)\CreateRigidBodyModelFile.exe \
	..\$(BUILDCONFIGURATION)\CharnwoodAlignment.exe \
	..\$(BUILDCONFIGURATION)\StartOculus.exe \
	LoadCodaCals.bat \
	RunGRASP.bat

install: $(EXECUTABLES) InstallGrasp.mak 
	-del /Q ..\..\Executables\*
	copy ..\$(BUILDCONFIGURATION)\OculusMouse.exe ..\..\Executables
	copy ..\$(BUILDCONFIGURATION)\Grasp.exe ..\..\Executables
	copy ..\Grasp\Grasp.bat ..\..\Executables
	copy ..\Grasp\GraspPackData.bat ..\..\Executables
	copy ..\$(BUILDCONFIGURATION)\GraspGUI.exe ..\..\Executables
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
	copy LoadCodaCals.bat ..\..\Executables
	copy RunGRASP.bat ..\..
	-mkdir ..\..\Executables\CodaCalFiles
	copy ..\..\..\Installers\CodaCalFiles\EM ..\..\Executables\CodaCalFiles
	copy ..\..\..\Installers\CodaCalFiles\FM2 ..\..\Executables\CodaCalFiles
	copy ..\..\..\Installers\CodaCalFiles\GM ..\..\Executables\CodaCalFiles
	copy ..\..\..\Installers\CodaCalFiles\SM ..\..\Executables\CodaCalFiles
	copy ..\DLLs\*.dll ..\..\Executables
	echo $(BUILDCONFIGURATION) %date% %time% > $@
