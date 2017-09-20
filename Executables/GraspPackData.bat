@echo off
REM
REM		GraspPackData.bat
REM

REM
REM A batch file that packages Grasp data files and places them on DEX.
REM

REM Where the utilities reside.
set UTILS=Utils

REM Now write the files to a directory on DEX, using the root filename to select which files to write.
REM Find the filename root amongst the parameters.
REM The GraspGUI shell puts the results file root in the form --output=FNROOT.
REM Note that the '=' is treated as a blank by the command processor.
REM We therefore look for '--ouput' and then take the next parameter as the rootname.
:FIND
if "%1"=="" goto NOTFOUND
if "%1"=="--output" goto FOUND
shift
goto FIND
:NOTFOUND 
echo Could not find filename root.
exit -5
:FOUND

REM
REM Strip off the filename root, leading only the path to the local results trajectory.
REM
set local_directory=%2
:shrink
if "%local_directory:~-1%" == "\" goto :next
set local_directory=%local_directory:~0,-1%
goto :shrink
:next
echo Local directory is "%local_directory%"

REM
REM Compute the archive name from the specified local directory.
REM Per CADMOS request, it should be YYDOY.tar.
REM
set yy=%local_directory:~-7,2%
set mm=%local_directory:~-5,2%
set dd=%local_directory:~-3,2%
call :DateToOrdinal %yy% %mm% %dd% year day
echo %yy% %mm% %dd% Year: %year% Day: %day%

REM 
REM Create the set of filenames that we need.
REM
set gROOT=g%year%%day%
set TARFILE=%gROOT%.tar
set TARMD5=%gROOT%.md5
set FILESMD5=%gROOT%Files.md5
echo Packing data into tar file: %TARFILE%

REM
REM Here we are supposed to insert all the pertinent files into the tar.
REM tar uses '/' instead of '\' and so does not expand 'xxx\*' correctly, so we
REM move to the directory and insert all the files using '*'. Then we move the 
REM .tar to where we really want it to be.
REM
REM We are going to move to the directory where the files are, so we keep
REM  a record of where we are now so as to be able to access the executables
REM  and utilities relative to where we are executing now.
set here=%CD%
pushd "%local_directory%"

REM
REM First create an MD5 checksum for each data file.
REM
"%here%"\%UTILS%\md5.exe * > %FILESMD5%

REM
REM Now put all the files in a tar archive, including the MD5s.
REM
"%here%"\%UTILS%\tar.exe --verbose --create --file=%TARFILE% * 

REM
REM Move it up one level to be parallel with the local directory.
REM This is done to ensure that we do not at a later time inadvertently
REM put one .tar into another.
move /Y %TARFILE% ..
cd ..

REM
REM Split it to files that can be handled by telemetry.
REM
echo Split into manageable chunks.
"%here%"\%UTILS%\split.exe --bytes=40m %TARFILE% %gROOT%.

REM
REM To be sure that we get everything correctly, compute MD5
REM checksums for each chunk and the overall .tar.
REM
echo Create MD5 checksums for archive and chunks.
"%here%"\%UTILS%\md5.exe %gRoot%.* > %TARMD5%

REM
REM Now send it to DEX using FTP.
REM
echo Transfer by FTP to GRIP.

set LOGONID=speedy
set PASSWORD=dex
set HOST=10.80.12.103

set GRASPROOT=DATA1
set DIRECTORY=GDLK

REM
REM Make the directory on the host. This might fail if the directory is already there, so ignore any errors.
REM
"%here%"\%UTILS%\WinSCP.com /command "open ftp://%LOGONID%:%PASSWORD%@%HOST%" "cd %GRASPROOT%" "mkdir %DIRECTORY%"  "exit" 
if ERRORLEVEL == 1 GOTO :IGNORE
:IGNORE
"%here%"\%UTILS%\WinSCP.com /command "open ftp://%LOGONID%:%PASSWORD%@%HOST%" "cd %GRASPROOT%" "cd %DIRECTORY%" "mput %gROOT%.??"  "put  %gROOT%.md5" "exit" 
if ERRORLEVEL == 1 GOTO :ERREXIT

REM Normal Exit
REM Clean up.
del /Q %gROOT%.?? %TARMD5% %TARFILE%
popd
exit 0

:ERREXIT
echo Error during file transfer to GRIP.
pause
exit -10

:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:DateToOrdinal %yy% %mm% %dd% year day
::
:: By: Ritchie Lawrence, updated 2002-11-22. Version 1.1
::
:: Func: Returns an ISO 8601 Ordinal date from a calendar date.
:: For NT4/2K/XP.
:: 
:: Args: %1 year component to be converted, 2 or 4 digits (by val)
::       %2 month component to be converted, leading zero ok (by val)
::       %3 day of month to be converted, leading zero ok (by val)
::       %4 var to receive year, 2 digits (by ref)
::       %5 var to receive day of year, 001 to 366 (by ref)
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
setlocal ENABLEEXTENSIONS
set yy=%1&set mm=%2&set dd=%3
if 1%yy% LSS 200 if 1%yy% LSS 170 (set yy=20%yy%) else (set yy=19%yy%)
set /a dd=100%dd%%%100,mm=100%mm%%%100
set /a z=14-mm,z/=12,y=yy+4800-z,m=mm+12*z-3,j=153*m+2
set /a j=j/5+dd+y*365+y/4-y/100+y/400-32045
set /a y=yy+4799,k=y*365+y/4-y/100+y/400-31738,o=j-k+1
set /a yy=yy-2000
if %o% LSS 100 set o=0%o%&if %o% LSS 10 set o=00%o%
endlocal&set %4=%yy%&set %5=%o%&goto :EOF
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

:EOF