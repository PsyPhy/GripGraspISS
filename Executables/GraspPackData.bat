@echo off
REM
REM		GraspPackData.bat
REM

REM
REM A batch file that packages Grasp data files and places them on DEX.
REM

REM Now write the files to a directory on DEX, using the root filename to select which files to write.
REM Find the filename root amongst the parameters.
REM The GraspGUI shell puts the results file root in the form --output=FNROOT.
REM Note that the '=' is treated as a blank by the command processor.
REM We therefore look for '--ouput' and then take the next parameter as the rootname.
:FIND
if "%1"=="" goto NOTFOUND
echo Leading argument is %1
if "%1"=="--output" goto FOUND
shift
goto FIND

:NOTFOUND 
echo Could not find filename root.
exit -1

:FOUND
set root=%2
echo File root is %root%

set local_directory=%root:~0,14%
echo Local directory is %local_directory%
dir %local_directory%

set local_directory=Results

REM
REM Copy the data files to DEX.
REM
set LOGONID=speedy
set PASSWORD=dex
set HOST=10.80.12.103

set GRASPROOT=DATA1
set DIRECTORY=GDLK

REM
REM Compute the archive name based on today's date.
set yy=%date:~6,4%
set mm=%date:~3,2%
set dd=%date:~0,2%
set TARFILE=g%yy%%mm%%dd%.tar
echo Tar filename is %TARFILE%
Executables\tar.exe --verbose --create --file=%TARFILE% *.dat *.ini *.bat

REM
REM Make the directory on the host. This might fail if the directory is already there, so ignore any errors.
REM
Executables\WinSCP.com /command "open ftp://%LOGONID%:%PASSWORD%@%HOST%" "cd %GRASPROOT%" "mkdir %DIRECTORY%"  "exit" 
if ERRORLEVEL == 1 GOTO IGNORE
:IGNORE
Executables\WinSCP.com /command "open ftp://%LOGONID%:%PASSWORD%@%HOST%" "cd %GRASPROOT%" "cd %DIRECTORY%" "mput %TARFILE%"  "exit" 


exit 0