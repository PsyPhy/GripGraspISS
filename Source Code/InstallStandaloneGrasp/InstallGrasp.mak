### Keep the GRASP top level execution environment curent.

EXECUTABLES = ..\$(BUILDCONFIGURATION)\PsyPhyOculusDemo.exe ..\$(BUILDCONFIGURATION)\OculusMouse.exe ..\$(BUILDCONFIGURATION)\GraspScreenshots.exe ..\$(BUILDCONFIGURATION)\Grasp.exe  ..\$(BUILDCONFIGURATION)\AlignToRigidBodyGUI.exe ..\$(BUILDCONFIGURATION)\GraspGUI.exe  ..\$(BUILDCONFIGURATION)\TaskProcessUnitTester.exe  ..\$(BUILDCONFIGURATION)\CreateRigidBodyModelFile.exe
install: $(EXECUTABLES) InstallGrasp.mak 
	copy ..\$(BUILDCONFIGURATION)\OculusMouse.exe ..\..\Executables
	copy ..\$(BUILDCONFIGURATION)\Grasp.exe ..\..\Executables
	copy ..\$(BUILDCONFIGURATION)\GraspGUI.exe ..\..\Executables
	copy ..\$(BUILDCONFIGURATION)\AlignToRigidBodyGUI.exe ..\..\Executables
	copy ..\$(BUILDCONFIGURATION)\TaskProcessUnitTester.exe ..\..\Executables
	copy ..\$(BUILDCONFIGURATION)\CreateRigidBodyModelFile.exe ..\..\Executables
	copy ..\$(BUILDCONFIGURATION)\PsyPhyOculusDemo.exe ..\..\Executables
	echo $(BUILDCONFIGURATION) %date% %time% > $@