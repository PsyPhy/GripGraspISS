### Keep the GRASP top level execution environment curent.

install: ..\Debug\Grasp.exe InstallGrasp.mak
	copy ..\Debug\Grasp.exe ..\..\bin
	copy ..\Debug\GraspGUI.exe ..\..\bin
	copy ..\Debug\AlignToRigidBody.exe ..\..\bin
	copy ..\Debug\TaskProcessUnitTester.exe ..\..\bin
	copy ..\Debug\CreateRigidBodyModelFile.exe ..\..\bin
	echo %date% %time% > $@