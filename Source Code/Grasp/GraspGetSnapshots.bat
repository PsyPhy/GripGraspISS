@echo on
REM
REM		GraspGetSnapshots.bat
REM

REM
REM A batch file that gets today's pictures off of DEX.
REM

REM
REM Copy the data files to DEX.
REM
set LOGONID=speedy
set PASSWORD=dex
set HOST=10.80.12.103

REM
REM Compute the subdirectory name based on today's date.
set GRASPROOT=DATA1/DATA/gimg
set yy=%date:~6,4%
set mm=%date:~3,2%
set dd=%date:~0,2%
set DIRECTORY=%yy%%mm%%dd%

..\..\Executables\WinSCP.com /command "open ftp://%LOGONID%:%PASSWORD%@%HOST%" "cd %GRASPROOT%" "ls %DIRECTORY%_*/*" "exit" 
pause
