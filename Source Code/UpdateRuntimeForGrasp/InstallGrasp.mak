### Keep the GRASP top level execution environment current.

EXECUTABLES = \
	..\$(BUILDCONFIGURATION)\Grasp.exe \
	..\Grasp\Grasp.bat \
	..\Grasp\GraspPackData.bat \
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
	..\$(BUILDCONFIGURATION)\OculusStartupCheck.exe \
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
	copy ..\Grasp\Grasp.bat $(DESTINATION)
	copy ..\Grasp\GraspPackData.bat $(DESTINATION)
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
	copy ..\$(BUILDCONFIGURATION)\MinimizeAll.exe $(DESTINATION)
	copy ..\$(BUILDCONFIGURATION)\OculusStartupCheck.exe $(DESTINATION)
	copy GraspPacketsForSimulator.gpk $(DESTINATION)
	copy LoadCodaCals.bat $(DESTINATION)
	-copy ..\DLLs\*.dll $(DESTINATION)
	copy MakeRigidBodies.bat $(ROOT)\Bdy
	copy RunGRASP.bat.src $(ROOT)\RunGRASP.bat
	copy RunDaemon.bat.src $(ROOT)\RunDaemon.bat
	-mkdir $(DESTINATION)\CodaCalFiles
	-copy ..\CodaCalFiles\EM $(DESTINATION)\CodaCalFiles
	-copy ..\CodaCalFiles\FM2 $(DESTINATION)\CodaCalFiles
	-copy ..\CodaCalFiles\GM $(DESTINATION)\CodaCalFiles
	-copy ..\CodaCalFiles\SM $(DESTINATION)\CodaCalFiles
	echo Grasp Version Information > $(DESTINATION)\GraspVersionInfo.txt
	..\$(BUILDCONFIGURATION)\GripGraspShowVersionInfo.exe >> $(DESTINATION)\GraspVersionInfo.txt
	echo $(BUILDCONFIGURATION) %date% %time% > $@
