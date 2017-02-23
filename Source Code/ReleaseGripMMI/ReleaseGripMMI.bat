@echo OFF
setlocal

REM 
REM ReleaseGripMMI.bat
REM

REM A batch file that creates a tar archive with the runtime environment needed for GripMMI.

REM Create a file tag based on the compiled build date and time.
REM The batch file  %temp%\SetCommonTimestamp.bat was created by the CommonTimestamp project.
REM We call it here to get the timestamp info that was compiled into the executables.
call %temp%\SetCommonTimestamp.bat
set TAG=(%__COMMONTIMESTAMP__%)
set ARCHIVE="..\GripMMI %1 %TAG%.tar"
echo Creating GrippMMI Runtime Release %ARCHIVE%

set TAR=Utils\tar.exe
set VERBOSE=

REM Point to the root level directory from which the tar archive will be created.
set ROOT=..\..

REM Copy the latest installation instructions up to the root to be included with the release.
copy /Y /V GripMMI_Installation_Instructions.txt %ROOT%\GripMMIDocumentation

REM This gets executed inside the Visual Studio project directory. We move to the root directoy.
pushd %ROOT%

%TAR% --append %VERBOSE% --file=%ARCHIVE% GripMMIExecutables/*
%TAR% --append %VERBOSE% --file=%ARCHIVE% GripMMIMirrorEnvironments/*
%TAR% --append %VERBOSE% --file=%ARCHIVE% GripMMIDocumentation/*
%TAR% --append %VERBOSE% --file=%ARCHIVE% RunGripMMI.bat

REM Keep a record of releases.
echo %ARCHIVE% >> GripGraspReleases.log
echo %ARCHIVE% >> GripMMIReleases.log

popd

