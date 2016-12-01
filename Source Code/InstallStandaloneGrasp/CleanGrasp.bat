REM Clean up the GRASP top level execution environment.
REM All executables will be removes, so you will need to rebuild to run.
REM Note that the executable for both Grasp and GraspMMI are removed.

rmdir /S /Q ..\..\Executables & echo Deleting Executables\ while ignoring any errors.
mkdir ..\..\Executables

REM Because we deleted all of the executables, we delete both batch files.
del /F /Q ..\..\RunGRASP.bat & echo Deleting RunGrasp.bat while ignoring any errors.
del /F /Q ..\..\RunGraspMMI.bat & echo Deleting RunGraspMMI.bat while ignoring any errors.

REM Delete any files that were created while running GraspGUI or Grasp.
del /F /Q ..\..\*.dxl & echo Deleting logs and such while ignoring any errors.

REM Remove the timestamp cookie file.
del /F /Q install & echo Deleting timestamp cookie while ignoring any errors.

