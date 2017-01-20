@echo OFF

REM 
REM ReleaseGripMMI.bat
REM

REM A batch file that creates a tar archive with the runtime environment needed for GripMMI.

setlocal
set TAR=Utils\tar.exe
set VERBOSE=

REM Point to the root level directory from which the tar archive will be created.
set ROOT=..\..

REM Copy the latest installation instructions up to the root to be included with the release.
copy /Y /V GripMMI_Installation_Instructions.txt %ROOT%

REM This gets executed inside the Visual Studio project directory. We move to the GRASPonISS root directoy.
pushd %ROOT%

REM Create a file tag with date and time, making sure that the hour has a leading zero.
if "%time:~0,1%" EQU " " (set HOUR=0%time:~1,1%) else (set HOUR=%time:~0,2%)
set TAG=(%date:~6,4%-%date:~3,2%-%date:~0,2% %HOUR%h%time:~3,2%m%time:~6,2%s)
set ARCHIVE="..\GripMMI %1 %TAG%.tar"
echo Creating GripMMI Runtime Release %ARCHIVE%

%TAR% --append %VERBOSE% --file=%ARCHIVE% GripMMIExecutables/*
%TAR% --append %VERBOSE% --file=%ARCHIVE% GripScripts/*
%TAR% --append %VERBOSE% --file=%ARCHIVE% GripDocumentation/*
%TAR% --append %VERBOSE% --file=%ARCHIVE% RunGripMMI.bat
%TAR% --append %VERBOSE% --file=%ARCHIVE% GripMMI_Installation_Instructions.txt

REM Create an empty cache directory to put in the release archive.
REM We hide the current Cache directory so as not to lose what we have there.
REM Then we create a new empty directory, with just a readme to put in the tar archive.
REM Finally, we restore the original Cache directlry.
rename Cache HIDECache
mkdir Cache
echo Cache files for GripMMI and GraspMMI go here. > Cache\readme.txt
%TAR% --append %VERBOSE% --file=%ARCHIVE% Cache/*
rmdir /S /Q Cache
rename HIDECache Cache

REM Keep a record of releases.
echo %ARCHIVE% >> GripGraspReleases.log

popd

