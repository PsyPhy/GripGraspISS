### Keep the GRASP top level execution environment current.

EXECUTABLES = \
	..\$(BUILDCONFIGURATION)\Grasp.exe \
	..\Grasp\Grasp.bat.src \
	..\Grasp\Grasp.ini.src \
	..\Grasp\GraspNew.ini.src \
	..\Grasp\GraspPackData.bat.src \
	..\GraspGUI\Grasp.ico ..\$(BUILDCONFIGURATION)\GraspGUI.exe \
	..\$(BUILDCONFIGURATION)\GraspHardwareStatus.exe \
	..\$(BUILDCONFIGURATION)\GripAimingTool.exe \
	..\$(BUILDCONFIGURATION)\GraspTrackerDaemon.exe \
	..\$(BUILDCONFIGURATION)\WaitForCodaDaemon.exe \
	..\$(BUILDCONFIGURATION)\PsyPhyOculusDemo.exe \
	..\$(BUILDCONFIGURATION)\OculusMouse.exe \
	..\$(BUILDCONFIGURATION)\MinimizeAll.exe \
	..\$(BUILDCONFIGURATION)\GraspScreenshots.exe \
	..\$(BUILDCONFIGURATION)\AlignToRigidBodyGUI.exe \
	..\$(BUILDCONFIGURATION)\AlignToAxisMarkersGUI.exe \
	..\$(BUILDCONFIGURATION)\TaskProcessUnitTester.exe  \
	..\$(BUILDCONFIGURATION)\CreateRigidBodyModelFile.exe \
	..\$(BUILDCONFIGURATION)\TweakRigidBodyModel.exe \
	..\$(BUILDCONFIGURATION)\CharnwoodAlignment.exe \
	..\$(BUILDCONFIGURATION)\StartOculus.exe \
	..\$(BUILDCONFIGURATION)\SendGraspData.exe \
	..\$(BUILDCONFIGURATION)\OculusStartupCheck.exe \
	..\$(BUILDCONFIGURATION)\CharnwoodAlignment.exe \
	..\$(BUILDCONFIGURATION)\CodaVisibility.exe \
	..\GraspScripts\UseBalls.bat.src \
	..\GraspScripts\UseBars.bat.src \
	GraspPacketsForSimulator.gpk \
	LoadCodaCals.bat.src \
	MakeRigidBodies.bat.src \
	RunDaemon.bat.src \
	RunGRASP.bat.src 

ROOT=..\..
DESTINATION = $(ROOT)\Executables

install: $(EXECUTABLES) InstallGrasp.mak 
	-rmdir /S /Q $(DESTINATION)
	mkdir $(DESTINATION)
	copy ..\$(BUILDCONFIGURATION)\OculusMouse.exe $(DESTINATION)
	copy ..\$(BUILDCONFIGURATION)\Grasp.exe $(DESTINATION)
	copy ..\Grasp\Grasp.bat.src $(DESTINATION)\Grasp.bat
	copy ..\Grasp\Grasp.ini.src $(ROOT)\Grasp.ini
	copy ..\Grasp\GraspNew.ini.src $(ROOT)\Grasp.ini
	copy ..\Grasp\GraspPackData.bat.src $(DESTINATION)\GraspPackData.bat
	copy ..\$(BUILDCONFIGURATION)\GraspScreenshots.exe $(DESTINATION)
	copy ..\$(BUILDCONFIGURATION)\GraspGUI.exe $(DESTINATION)
	copy ..\GraspGUI\Grasp.ico $(DESTINATION)
	copy ..\$(BUILDCONFIGURATION)\AlignToRigidBodyGUI.exe $(DESTINATION)
	copy ..\$(BUILDCONFIGURATION)\AlignToAxisMarkersGUI.exe $(DESTINATION)
	copy ..\$(BUILDCONFIGURATION)\GripAimingTool.exe $(DESTINATION)
	copy ..\$(BUILDCONFIGURATION)\TaskProcessUnitTester.exe $(DESTINATION)
	copy ..\$(BUILDCONFIGURATION)\CreateRigidBodyModelFile.exe $(DESTINATION)
	copy ..\$(BUILDCONFIGURATION)\TweakRigidBodyModel.exe $(DESTINATION)
	copy ..\$(BUILDCONFIGURATION)\PsyPhyOculusDemo.exe $(DESTINATION)
	copy ..\$(BUILDCONFIGURATION)\GraspTrackerDaemon.exe $(DESTINATION)
	copy ..\$(BUILDCONFIGURATION)\GraspHardwareStatus.exe $(DESTINATION)
	copy ..\$(BUILDCONFIGURATION)\CharnwoodAlignment.exe $(DESTINATION)
	copy ..\$(BUILDCONFIGURATION)\CodaVisibility.exe $(DESTINATION)
	copy ..\$(BUILDCONFIGURATION)\WaitForCodaDaemon.exe $(DESTINATION)
	copy ..\$(BUILDCONFIGURATION)\StartOculus.exe $(DESTINATION)
	copy ..\$(BUILDCONFIGURATION)\SendGraspData.exe $(DESTINATION)
	copy ..\$(BUILDCONFIGURATION)\MinimizeAll.exe $(DESTINATION)
	copy ..\$(BUILDCONFIGURATION)\OculusStartupCheck.exe $(DESTINATION)
	copy ..\GraspScripts\UseBalls.bat.src $(DESTINATION)\UseBalls.bat
	copy ..\GraspScripts\UseBars.bat.src $(DESTINATION)\UseBars.bat
	copy GraspPacketsForSimulator.gpk $(DESTINATION)
	copy LoadCodaCals.bat.src $(DESTINATION)\LoadCodaCals.bat
	-copy ..\DLLs\*.dll $(DESTINATION)
	copy MakeRigidBodies.bat.src $(ROOT)\Bdy\MakeRigidBodies.bat
	copy RunGRASP.bat.src $(ROOT)\RunGRASP.bat
	copy RunDaemon.bat.src $(ROOT)\RunDaemon.bat
	copy ..\GraspScreenshots\MakeCommScreenshots.bat.src $(ROOT)\MakeCommScreenshots.bat
	-mkdir $(DESTINATION)\CodaCalFiles
	-copy ..\CodaCalFiles\EM $(DESTINATION)\CodaCalFiles
	-copy ..\CodaCalFiles\FM2 $(DESTINATION)\CodaCalFiles
	-copy ..\CodaCalFiles\GM $(DESTINATION)\CodaCalFiles
	-copy ..\CodaCalFiles\SM $(DESTINATION)\CodaCalFiles
	-rmdir /S /Q $(ROOT)\InitFiles
	mkdir $(ROOT)\InitFiles
	-copy ..\InitFiles\*.* $(ROOT)\InitFiles
	copy ..\Grasp\Grasp.bat.src $(DESTINATION)\Grasp.bat
	copy ..\Grasp\Grasp.ini.src $(ROOT)\Grasp.ini
	copy ..\Grasp\Grasp.ini.src $(ROOT)\Grasp.01.ini
	copy ..\Grasp\Grasp.ini.src $(ROOT)\Grasp.02.ini
	copy ..\Grasp\Grasp.ini.src $(ROOT)\Grasp.03.ini
	copy ..\Grasp\Grasp.ini.src $(ROOT)\Grasp.04.ini
	copy ..\Grasp\Grasp.ini.src $(ROOT)\Grasp.05.ini
	copy ..\Grasp\Grasp.ini.src $(ROOT)\Grasp.06.ini
	copy ..\Grasp\GraspNew.ini.src $(ROOT)\Grasp.ini
	copy ..\Grasp\GraspNew.ini.src $(ROOT)\Grasp.07.ini
	copy ..\Grasp\GraspNew.ini.src $(ROOT)\Grasp.08.ini
	copy ..\Grasp\GraspNew.ini.src $(ROOT)\Grasp.09.ini
	copy ..\Grasp\GraspNew.ini.src $(ROOT)\Grasp.10.ini
	copy ..\Grasp\GraspNew.ini.src $(ROOT)\Grasp.11.ini
	copy ..\Grasp\GraspNew.ini.src $(ROOT)\Grasp.12.ini
	copy ..\Grasp\GraspNew.ini.src $(ROOT)\Grasp.13.ini
	copy ..\Grasp\GraspNew.ini.src $(ROOT)\Grasp.14.ini
	copy ..\Grasp\GraspNew.ini.src $(ROOT)\Grasp.15.ini
	copy ..\Grasp\Grasp.ini.src $(ROOT)\Grasp.97.ini
	copy ..\Grasp\GraspNew.ini.src $(ROOT)\Grasp.98.ini
	copy ..\Grasp\GraspNew.ini.src $(ROOT)\Grasp.99.ini
	echo Grasp Version Information > $(DESTINATION)\GraspVersionInfo.txt
	..\$(BUILDCONFIGURATION)\GripGraspShowVersionInfo.exe >> $(DESTINATION)\GraspVersionInfo.txt
	echo $(BUILDCONFIGURATION) %date% %time% > $@
