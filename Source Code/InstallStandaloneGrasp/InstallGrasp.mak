### Keep the GRASP top level execution environment curent.

install: ..\Debug\Grasp.exe
	copy ..\Debug\Grasp.exe ..\..\bin
	copy ..\Debug\GraspGUI.exe ..\..\bin
	copy ..\Debug\AlignToRigidBody.exe ..\..\bin
	echo %date% %time% > $@