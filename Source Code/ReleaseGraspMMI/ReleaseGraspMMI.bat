@echo OFF

REM 
REM ReleaseGraspMMI.bat
REM

REM A batch file that creates a tar archive with the GRASPonISS runtime installation.

setlocal
set TAR=Utils\tar.exe
set VERBOSE=

REM Point to the root level directory from which the tar archive will be created.
set ROOT=..\..

REM Copy the latest installation instructions to be included with the release.
copy /Y /V GraspMMI_Installation_Instructions.txt %ROOT%\GraspMMIDocumentation

REM This gets executed inside the Visual Studio project directory. We move to the GRASPonISS root directory.
pushd %ROOT%

REM Create a file tag with date and time, making sure that the hour has a leading zero.
if "%time:~0,1%" EQU " " (set HOUR=0%time:~1,1%) else (set HOUR=%time:~0,2%)
REM Create a date string in the form YYYY-MM-DD, trying to take into account the data format on the machine.
REM If the first field is the day of the week, then the 4th character will be a blank.
if "%date:~3,1%" EQU " " (set DTT=%date:~10,4%-%date:~4,2%-%date:~7,2%) else (set DTT=%date:~6,4%-%date:~3,2%-%date:~0,2%)
set TAG=(%DTT% %HOUR%h%time:~3,2%m%time:~6,2%s)
set ARCHIVE="..\GraspMMI %1 %TAG%.tar"
echo Creating GRASPonISS Runtime Release %ARCHIVE%

%TAR% --append %VERBOSE% --file=%ARCHIVE% GraspMMIExecutables/*
%TAR% --append %VERBOSE% --file=%ARCHIVE% GraspMMIMirrorEnvironments/*
%TAR% --append %VERBOSE% --file=%ARCHIVE% GraspMMIDocumentation/*
%TAR% --append %VERBOSE% --file=%ARCHIVE% RunGraspMMI.bat

REM Keep a record of releases.
echo %ARCHIVE% >> GraspMMIReleases.log
echo %ARCHIVE% >> GripGraspReleases.log

popd

