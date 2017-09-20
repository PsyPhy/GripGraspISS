@echo OFF
setlocal

REM 
REM ReleaseGrasp.bat
REM

REM A batch file that creates a tar archive with the GRASPonISS runtime installation.

REM Create a file tag based on the compiled build date and time.
REM The batch file  %temp%\SetCommonTimestamp.bat was created by the CommonTimestamp project.
REM We call it here to get the timestamp info that was compiled into the executables.
call %temp%\SetCommonTimestamp.bat
set TAG=(%__COMMONTIMESTAMP__%)
set ARCHIVE="..\GRASP Runtime %1 %TAG%.tar"
echo Creating Grasp Runtime Release %ARCHIVE%

set TAR=Utils\tar.exe
set VERBOSE=

REM This gets executed inside the Visual Studio project directory. We move to the GRASPonISS root directoy.
pushd ..\..

%TAR% --create %VERBOSE% --file=%ARCHIVE% Bdy/*
%TAR% --append %VERBOSE% --file=%ARCHIVE% Bmp/*
%TAR% --append %VERBOSE% --file=%ARCHIVE% Executables/*
%TAR% --append %VERBOSE% --file=%ARCHIVE% Utils/*
%TAR% --append %VERBOSE% --file=%ARCHIVE% Instructions/*
%TAR% --append %VERBOSE% --file=%ARCHIVE% Scripts/*
%TAR% --append %VERBOSE% --file=%ARCHIVE% Sequences/*
%TAR% --append %VERBOSE% --file=%ARCHIVE% InitFiles/*
%TAR% --append %VERBOSE% --file=%ARCHIVE% Documentation/*
%TAR% --append %VERBOSE% --file=%ARCHIVE% RunGRASP.bat
%TAR% --append %VERBOSE% --file=%ARCHIVE% *.ini.*

REM If we are creating a training version, then by default set for mirroring mode.
if NOT %1==Training goto :NOMIRRORING
echo Enabling mirroring.
echo "Mirroring on." > Mirroring.flg
%TAR% --append %VERBOSE% --file=%ARCHIVE% Mirroring.flg
:NOMIRRORING

REM Keep a record of releases.
echo %ARCHIVE% >> GripGraspReleases.log
echo %ARCHIVE% >> GraspReleases.log

