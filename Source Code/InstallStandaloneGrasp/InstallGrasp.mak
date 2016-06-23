### Keep the GRASP top level execution environment curent.

EXECUTABLES = ..\Debug\Grasp.exe  ..\Debug\AlignToRigidBody.exe ..\Debug\GraspGUI.exe  ..\Debug\TaskProcessUnitTester.exe  ..\Debug\CreateRigidBodyModelFile.exe
install: $(EXECUTABLES) InstallGrasp.mak  
	copy ..\Debug\Grasp.exe ..\..\Executables
	copy ..\Debug\GraspGUI.exe ..\..\Executables
	copy ..\Debug\AlignToRigidBody.exe ..\..\Executables
	copy ..\Debug\TaskProcessUnitTester.exe ..\..\Executables
	copy ..\Debug\CreateRigidBodyModelFile.exe ..\..\Executables
	echo %date% %time% > $@