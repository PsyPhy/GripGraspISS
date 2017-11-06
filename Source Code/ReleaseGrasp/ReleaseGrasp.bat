@echo on
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

%TAR% --create %VERBOSE% --file=%ARCHIVE% RunGRASP.bat

::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

if NOT %1==Training goto :NOTTRAINING

REM If we are creating a training version, then by default set for mirroring mode.
echo Enabling mirroring.
echo "Mirroring on." > Mirroring.flg
%TAR% --append %VERBOSE% --file=%ARCHIVE% Mirroring.flg

REM Configure for EAC Codas and Marker Structures
copy /Y InitFiles\CodaRTnet.ini.TMa CodaRTnet.ini 
copy /Y Bdy\TrM\*.bdy Bdy

goto :COMMON

::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

:NOTTRAINING

if %1==Flight goto :FLIGHT
if %1==Draft goto :FLIGHT

REM Not Training and not Flight-like
REM Releasing local configuration (%1)
goto :COMMON

:FLIGHT

REM Configure for Flight-like (%1)
REM Configure for FM2 Codas and FM Marker Structures
copy /Y InitFiles\CodaRTnet.ini.FM2 CodaRTnet.ini 
copy /Y Bdy\FM\*.bdy Bdy
echo chest > ChestAlignGaze.flg
%TAR% --append %VERBOSE% --file=%ARCHIVE% ChestAlignGaze.flg
echo Enabling mirroring.
echo "Mirroring on." > Mirroring.flg
%TAR% --append %VERBOSE% --file=%ARCHIVE% Mirroring.flg

goto :COMMON

::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

:COMMON

%TAR% --append %VERBOSE% --file=%ARCHIVE% Bdy/*
%TAR% --append %VERBOSE% --file=%ARCHIVE% Bmp/*
%TAR% --append %VERBOSE% --file=%ARCHIVE% Executables/*
%TAR% --append %VERBOSE% --file=%ARCHIVE% Utils/*
%TAR% --append %VERBOSE% --file=%ARCHIVE% Instructions/*
%TAR% --append %VERBOSE% --file=%ARCHIVE% Scripts/*
%TAR% --append %VERBOSE% --file=%ARCHIVE% Sequences/*
%TAR% --append %VERBOSE% --file=%ARCHIVE% InitFiles/*
%TAR% --append %VERBOSE% --file=%ARCHIVE% Documentation/*
%TAR% --append %VERBOSE% --file=%ARCHIVE% CodaRTnet.ini
%TAR% --append %VERBOSE% --file=%ARCHIVE% Grasp.ini

::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

:FINISHED
REM Keep a record of releases.
echo %ARCHIVE% >> GripGraspReleases.log
echo %ARCHIVE% >> GraspReleases.log

