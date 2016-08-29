### Keep the GRASP top level execution environment curent.

EXECUTABLES = ..\Grasp\GraspPackData.bat ..\..\Executables \
	LoadCodaCals.bat ..\$(BUILDCONFIGURATION)\GraspHardwareStatus.exe \
	..\GraspTrackerDaemon\GraspTrackerDaemon.bat ..\$(BUILDCONFIGURATION)\GraspTrackerDaemon.exe \
	..\$(BUILDCONFIGURATION)\PsyPhyOculusDemo.exe ..\$(BUILDCONFIGURATION)\OculusMouse.exe \
	..\$(BUILDCONFIGURATION)\GraspScreenshots.exe ..\$(BUILDCONFIGURATION)\Grasp.exe ..\Grasp\Grasp.bat \
	..\$(BUILDCONFIGURATION)\AlignToRigidBodyGUI.exe ..\$(BUILDCONFIGURATION)\GraspGUI.exe  \
	..\$(BUILDCONFIGURATION)\TaskProcessUnitTester.exe  ..\$(BUILDCONFIGURATION)\CreateRigidBodyModelFile.exe \
	..\$(BUILDCONFIGURATION)\CharnwoodAlignment.exe

install: $(EXECUTABLES) InstallGrasp.mak 
	copy ..\$(BUILDCONFIGURATION)\OculusMouse.exe ..\..\Executables
	copy ..\$(BUILDCONFIGURATION)\Grasp.exe ..\..\Executables
	copy ..\Grasp\Grasp.bat ..\..\Executables
	copy ..\Grasp\GraspPackData.bat ..\..\Executables
	copy ..\$(BUILDCONFIGURATION)\GraspGUI.exe ..\..\Executables
	copy ..\$(BUILDCONFIGURATION)\AlignToRigidBodyGUI.exe ..\..\Executables
	copy ..\$(BUILDCONFIGURATION)\TaskProcessUnitTester.exe ..\..\Executables
	copy ..\$(BUILDCONFIGURATION)\CreateRigidBodyModelFile.exe ..\..\Executables
	copy ..\$(BUILDCONFIGURATION)\PsyPhyOculusDemo.exe ..\..\Executables
	copy ..\$(BUILDCONFIGURATION)\GraspTrackerDaemon.exe ..\..\Executables
	copy ..\GraspTrackerDaemon\GraspTrackerDaemon.bat ..\..\Executables
	copy ..\$(BUILDCONFIGURATION)\GraspHardwareStatus.exe ..\..\Executables
	copy ..\$(BUILDCONFIGURATION)\CharnwoodAlignment.exe ..\..\Executables
	copy LoadCodaCals.bat ..\..\Executables
	-mkdir ..\..\Executables\CodaCalFiles
	copy ..\..\..\Installers\CodaCalFiles\EM ..\..\Executables\CodaCalFiles
	copy ..\..\..\Installers\CodaCalFiles\FM2 ..\..\Executables\CodaCalFiles
	copy ..\..\..\Installers\CodaCalFiles\GM ..\..\Executables\CodaCalFiles
	copy ..\..\..\Installers\CodaCalFiles\SM ..\..\Executables\CodaCalFiles
	echo $(BUILDCONFIGURATION) %date% %time% > $@