### Keep the GRASP top level execution environment current.

EXECUTABLES = \
	..\$(BUILDCONFIGURATION)\Grasp.exe \
	..\Grasp\Grasp.bat.src \
	..\Grasp\Grasp.ini.src \
	..\Grasp\GraspPackData.bat.src \
	..\GraspGUI\Grasp.ico ..\$(BUILDCONFIGURATION)\GraspGUI.exe \
	..\$(BUILDCONFIGURATION)\GraspHardwareStatus.exe \
	..\GraspTrackerDaemon\StartGraspTrackerDaemon.bat \
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
	..\GraspScripts\UseBalls.bat.src \
	..\GraspScripts\UseBars.bat.src \
	GraspPacketsForSimulator.gpk \
	LoadCodaCals.bat \
	MakeRigidBodies.bat \
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
	copy ..\Grasp\GraspPackData.bat.src $(DESTINATION)\GraspPackData.bat
	copy ..\$(BUILDCONFIGURATION)\GraspScreenshots.exe $(DESTINATION)
	copy ..\$(BUILDCONFIGURATION)\GraspGUI.exe $(DESTINATION)
	copy ..\GraspGUI\Grasp.ico $(DESTINATION)
	copy ..\$(BUILDCONFIGURATION)\AlignToRigidBodyGUI.exe $(DESTINATION)
	copy ..\$(BUILDCONFIGURATION)\AlignToAxisMarkersGUI.exe $(DESTINATION)
	copy ..\$(BUILDCONFIGURATION)\TaskProcessUnitTester.exe $(DESTINATION)
	copy ..\$(BUILDCONFIGURATION)\CreateRigidBodyModelFile.exe $(DESTINATION)
	copy ..\$(BUILDCONFIGURATION)\TweakRigidBodyModel.exe $(DESTINATION)
	copy ..\$(BUILDCONFIGURATION)\PsyPhyOculusDemo.exe $(DESTINATION)
	copy ..\$(BUILDCONFIGURATION)\GraspTrackerDaemon.exe $(DESTINATION)
	copy ..\GraspTrackerDaemon\StartGraspTrackerDaemon.bat $(DESTINATION)
	copy ..\$(BUILDCONFIGURATION)\GraspHardwareStatus.exe $(DESTINATION)
	copy ..\$(BUILDCONFIGURATION)\CharnwoodAlignment.exe $(DESTINATION)
	copy ..\$(BUILDCONFIGURATION)\WaitForCodaDaemon.exe $(DESTINATION)
	copy ..\$(BUILDCONFIGURATION)\StartOculus.exe $(DESTINATION)
	copy ..\$(BUILDCONFIGURATION)\SendGraspData.exe $(DESTINATION)
	copy ..\$(BUILDCONFIGURATION)\MinimizeAll.exe $(DESTINATION)
	copy ..\$(BUILDCONFIGURATION)\OculusStartupCheck.exe $(DESTINATION)
	copy ..\GraspScripts\UseBalls.bat.src $(DESTINATION)\UseBalls.bat
	copy ..\GraspScripts\UseBars.bat.src $(DESTINATION)\UseBars.bat
	copy GraspPacketsForSimulator.gpk $(DESTINATION)
	copy LoadCodaCals.bat $(DESTINATION)
	-copy ..\DLLs\*.dll $(DESTINATION)
	copy MakeRigidBodies.bat $(ROOT)\Bdy
	copy RunGRASP.bat.src $(ROOT)\RunGRASP.bat
	copy RunDaemon.bat.src $(ROOT)\RunDaemon.bat
	copy ..\GraspScreenshots\MakeCommScreenshots.bat.src $(ROOT)\MakeCommScreenshots.bat
	-mkdir $(DESTINATION)\CodaCalFiles
	-copy ..\CodaCalFiles\EM $(DESTINATION)\CodaCalFiles
	-copy ..\CodaCalFiles\FM2 $(DESTINATION)\CodaCalFiles
	-copy ..\CodaCalFiles\GM $(DESTINATION)\CodaCalFiles
	-copy ..\CodaCalFiles\SM $(DESTINATION)\CodaCalFiles
	echo Grasp Version Information > $(DESTINATION)\GraspVersionInfo.txt
	..\$(BUILDCONFIGURATION)\GripGraspShowVersionInfo.exe >> $(DESTINATION)\GraspVersionInfo.txt
	echo $(BUILDCONFIGURATION) %date% %time% > $@
