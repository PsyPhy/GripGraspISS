@echo off
setlocal

REM 
REM ReleaseAiming.bat
REM

REM A batch file that creates a tar archive with the GRASPonISS runtime installation.

REM Create a file tag based on the compiled build date and time.
REM The batch file  %temp%\SetCommonTimestamp.bat was created by the CommonTimestamp project.
REM We call it here to get the timestamp info that was compiled into the executables.
call %temp%\SetCommonTimestamp.bat
set TAG=(%__COMMONTIMESTAMP__%)
set ARCHIVE="..\AIMING Runtime %1 %TAG%.tar"
echo Creating Aiming Runtime Release %ARCHIVE%

set TAR=Utils\tar.exe
set VERBOSE=

REM This gets executed inside the Visual Studio project directory. We move to the GRASPonISS root directoy.

pushd ..\..

%TAR% --create %VERBOSE% --file=%ARCHIVE% RunAIMING.bat

::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

%TAR% --append %VERBOSE% --file=%ARCHIVE% Executables/*
%TAR% --append %VERBOSE% --file=%ARCHIVE% Bdy/*.bdy
%TAR% --append %VERBOSE% --file=%ARCHIVE% EpmRTnet.ini
%TAR% --append %VERBOSE% --file=%ARCHIVE% Grasp.ini
%TAR% --append %VERBOSE% --file=%ARCHIVE% GripAimingTool.ini
REM Force the system to use the polling method to avoid coda desync.
echo polled >PolledCODA.flg
%TAR% --append %VERBOSE% --file=%ARCHIVE% PolledCODA.flg


::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

:FINISHED
REM Keep a record of releases.
echo %ARCHIVE% >> GripGraspReleases.log
echo %ARCHIVE% >> AimingReleases.log

popd

