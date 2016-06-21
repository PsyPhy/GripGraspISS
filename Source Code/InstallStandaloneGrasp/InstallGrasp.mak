### Keep the GRASP top level execution environment curent.

EXECUTABLES = ..\Debug\Grasp.exe  ..\Debug\AlignToRigidBody.exe ..\Debug\GraspGUI.exe  ..\Debug\TaskProcessUnitTester.exe  ..\Debug\CreateRigidBodyModelFile.exe
install: $(EXECUTABLES) InstallGrasp.mak  
	copy ..\Debug\Grasp.exe ..\..\bin
	copy ..\Debug\GraspGUI.exe ..\..\bin
	copy ..\Debug\AlignToRigidBody.exe ..\..\bin
	copy ..\Debug\TaskProcessUnitTester.exe ..\..\bin
	copy ..\Debug\CreateRigidBodyModelFile.exe ..\..\bin
	echo %date% %time% > $@