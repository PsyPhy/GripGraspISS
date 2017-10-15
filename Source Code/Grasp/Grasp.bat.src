@echo off
REM
REM		Grasp.bat
REM

REM
REM A batch file that executes Grasp.exe and then transfers the data files to DEX.
REM

REM
REM Execute Grasp.exe with all the arguments.
REM
Executables\Grasp.exe %*
set GRASPERRORLEVEL=%ERRORLEVEL%

REM Now write the files to a directory on DEX, using the root filename to select which files to write.
REM Do not attempt to send if the NoFTP.flg cookie is present.
if exist NoFTP.flg goto NOFTP

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
exit -10

:FOUND
set root=%2
echo File root is %root%

REM
REM Copy the data files to DEX.
REM
set LOGONID=speedy
set PASSWORD=dex
set HOST=10.80.12.103

REM
REM Compute the subdirectory name based on today's date.
set GRASPROOT=DATA1/DATA/glog
set yy=%date:~6,4%
set mm=%date:~3,2%
set dd=%date:~0,2%
set DIRECTORY=%yy%%mm%%dd%

REM
REM Make the directory on the host. This might fail if the directory is already there, so ignore any errors.
REM
Utils\WinSCP.com /command "open ftp://%LOGONID%:%PASSWORD%@%HOST%" "cd %GRASPROOT%" "mkdir %DIRECTORY%"  "exit" 
if ERRORLEVEL == 1 GOTO IGNORE
:IGNORE
Utils\WinSCP.com /command "open ftp://%LOGONID%:%PASSWORD%@%HOST%" "cd %GRASPROOT%" "cd %DIRECTORY%" "mput %2.*"  "exit" 
set FTPERRORLEVEL=%ERRORLEVEL%
if %FTPERRORLEVEL% LSS 0 set /a FTPERRORLEVEL=%FTPERRORLEVEL% + 1 

:NOFTP
if "%GRASPERRORLEVEL%"=="0" exit -%FTPERRORLEVEL%
exit %GRASPERRORLEVEL%