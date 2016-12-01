@echo OFF

REM 
REM ReleaseGrasp.bat
REM

REM A batch file that creates a tar archive with the GRASPonISS runtime installation.

setlocal
set TAR=Utils\tar.exe
set VERBOSE=

REM This gets executed inside the Visual Studio project directory. We move to the GRASPonISS root directoy.
pushd ..\..

REM Create a file tag with date and time, making sure that the hour has a leading zero.
if "%time:~0,1%" EQU " " (set HOUR=0%time:~1,1%) else (set HOUR=%time:~0,2%)
set TAG=(%date:~6,4%-%date:~3,2%-%date:~0,2% %HOUR%h%time:~3,2%m%time:~6,2%s)
set ARCHIVE="..\Grasp %1 %TAG%.tar"
echo Creating GRASPonISS Runtime Release %ARCHIVE%

%TAR% --create %VERBOSE% --file=%ARCHIVE% Bdy/*
%TAR% --append %VERBOSE% --file=%ARCHIVE% Bmp/*
%TAR% --append %VERBOSE% --file=%ARCHIVE% GraspExecutables/*
%TAR% --append %VERBOSE% --file=%ARCHIVE% Utils/*
%TAR% --append %VERBOSE% --file=%ARCHIVE% GraspInstructions/*
%TAR% --append %VERBOSE% --file=%ARCHIVE% GraspScripts/*
%TAR% --append %VERBOSE% --file=%ARCHIVE% GraspSequences/*
%TAR% --append %VERBOSE% --file=%ARCHIVE% InitFiles/*
%TAR% --append %VERBOSE% --file=%ARCHIVE% GraspDocumentation/*
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
echo Results files from GRIP and GRASP go here. > Results\readme.txt
%TAR% --append %VERBOSE% --file=%ARCHIVE% Results/*
rmdir /S /Q Results
rename HIDEResults Results

REM Create an empty cache directory to put in the release archive.
REM We hide the current Cache directory so as not to loose what we have there.
REM Then we create a new empty directory, with just a readme to put in the tar archive.
REM Finally, we restore the original Cache directlry.
rename Cache HIDECache
mkdir Cache
echo Cache files for GripMMI and GraspMMI go here. > Cache\readme.txt
%TAR% --append %VERBOSE% --file=%ARCHIVE% Cache/*
rmdir /S /Q Cache
rename HIDECache Cache

REM Keep a record of releases.
echo %ARCHIVE% >> GraspReleases.log

