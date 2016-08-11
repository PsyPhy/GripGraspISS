@echo off
REM
REM		Grasp.bat
REM

REM A batch file that executes Grasp.exe and then transfers the data files to DEX.

setlocal ENABLEEXTENSIONS

REM Show me all of the command line arguments for debugging purposes.
echo %*

REM Find the filename root amongst the parameters.
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
echo File root is %2
REM Execute Grasp.exe with all the arguments.
Executables\Grasp.exe %*
REM Copy the data files to DEX.
dir %2.*

set LOGONID=speedy
set PASSWORD=dex
set HOST=10.80.12.103

set GRASPROOT=glog

set yy=%date:~8,2%
set mm=%date:~3,2%
set dd=%date:~0,2%

set DIRECTORY=%yy%%mm%%dd%

echo Executables\WinSCP.com /command "open ftp://%LOGONID%:%PASSWORD%@%HOST%" "cd %GRASPROOT%" "mkdir %DIRECTORY%" "cd %DIRECTORY%" "mput %2.*"  "exit" 
Executables\WinSCP.com /command "open ftp://%LOGONID%:%PASSWORD%@%HOST%" "cd %GRASPROOT%" "mkdir %DIRECTORY%" "cd %DIRECTORY%" "mput %2.*"  "exit" 

pause
exit 0