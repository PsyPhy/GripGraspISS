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
set ARCHIVE="..\Grasp %1 %TAG%.tar"
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

REM Create an empty log directory to put in the release archive.
REM We hide the current LOG directory so as not to loose what we have there.
REM Then we create a new empty directory, with just a readme to put in the tar archive.
REM Finally, we restore the original LOG file.
rename LOG HIDELOG
mkdir LOG
echo Log files from GRIP and GRASP go here. > LOG\readme.txt
%TAR% --append %VERBOSE% --file=%ARCHIVE% LOG/*
rmdir /S /Q LOG
rename HIDELOG LOG

REM Create an empty Results directory to put in the release archive.
REM We hide the current Results directory so as not to loose what we have there.
REM Then we create a new empty directory, with just a readme to put in the tar archive.
REM Finally, we restore the original Results file.
rename Results HIDEResults
mkdir Results
echo Results files from GRASP go here. > Results\readme.txt
%TAR% --append %VERBOSE% --file=%ARCHIVE% Results/*
rmdir /S /Q Results
rename HIDEResults Results

REM Keep a record of releases.
echo %ARCHIVE% >> GripGraspReleases.log
echo %ARCHIVE% >> GraspReleases.log

