### Keep the GRASP top level execution environment curent.

EXECUTABLES =  ..\Release\OculusMouse.exe  ..\Release\Grasp.exe  ..\Release\AlignToRigidBodyGUI.exe ..\Release\GraspGUI.exe  ..\Release\TaskProcessUnitTester.exe  ..\Release\CreateRigidBodyModelFile.exe
install: $(EXECUTABLES) InstallGraspRelease.mak 
	copy ..\Release\OculusMouse.exe ..\..\Executables
	copy ..\Release\Grasp.exe ..\..\Executables
	copy ..\Release\GraspGUI.exe ..\..\Executables
	copy ..\Release\AlignToRigidBodyGUI.exe ..\..\Executables
	copy ..\Release\TaskProcessUnitTester.exe ..\..\Executables
	copy ..\Release\CreateRigidBodyModelFile.exe ..\..\Executables
	echo Release %date% %time% > $@
