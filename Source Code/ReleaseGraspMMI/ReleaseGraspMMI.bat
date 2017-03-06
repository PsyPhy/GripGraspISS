@echo OFF
setlocal

REM 
REM ReleaseGraspMMI.bat
REM

REM A batch file that creates a tar archive with the GRASPonISS runtime installation.

REM Create a file tag based on the compiled build date and time.
REM The batch file  %temp%\SetCommonTimestamp.bat was created by the CommonTimestamp project.
REM We call it here to get the timestamp info that was compiled into the executables.
call %temp%\SetCommonTimestamp.bat
set TAG=(%__COMMONTIMESTAMP__%)
set ARCHIVE="..\GraspMMI Runtime %1 %TAG%.tar"
echo Creating GraspMMI Runtime Release %ARCHIVE%

set TAR=Utils\tar.exe
set VERBOSE=

REM Point to the root level directory from which the tar archive will be created.
set ROOT=..\..

REM This gets executed inside the Visual Studio project directory. We move to the GRASPonISS root directory.
pushd %ROOT%

%TAR% --append %VERBOSE% --file=%ARCHIVE% GraspMMIExecutables/*
%TAR% --append %VERBOSE% --file=%ARCHIVE% GraspMMIMirrorEnvironments/*
%TAR% --append %VERBOSE% --file=%ARCHIVE% "GraspMMIDocumentation\GraspMMI Runtime Installation Instructions.txt"
%TAR% --append %VERBOSE% --file=%ARCHIVE% "GraspMMIDocumentation\GraspMMI Release Notes.rtf"
%TAR% --append %VERBOSE% --file=%ARCHIVE% RunGraspMMI.bat

REM Keep a record of releases.
echo %ARCHIVE% >> GraspMMIReleases.log
echo %ARCHIVE% >> GripGraspReleases.log

popd

