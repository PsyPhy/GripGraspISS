### Clean up the GRASP top level execution environment.
### All executables will be removes, so you will need to rebuild to run.
### Note that the executable for both Grasp and GraspMMI are removed.
### Because we deleted all of the executables, we delete both batch files.
### Also delete incidental files that were created while running GraspGUI or Grasp.

default:
	-rmdir /S /Q ..\..\GraspExecutables
	mkdir ..\..\GraspExecutables	
	-del /F /Q ..\..\RunGRASP.bat 
	-del /F /Q ..\..\RunGraspMMI.bat 
	-del /F /Q ..\..\*.dxl 
	-del /F /Q install 

