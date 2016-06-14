REM 
REM  GraspHelper.bat
REM

REM A batch file to facilitate the execution of Grasp.exe from within GraspGUI.


REM First argument is the paradigm (-V-V -V-K -K-K, etc.)
set PARADIGM=%1

REM Second is the sequence file
set SEQUENCE=%2

REM Third is a tag to identify the subject.
set SUBJECT=%3

REM Create a directory to hold today's data.
set DIRECTORY=%date:~10,4%%date:~4,2%%date:~7,2%
mkdir %DIRECTORY%
if ERRORLEVEL = 1 goto :IGNORE

:IGNORE

set TIMESTAMP=%ROOT% %QUALIFIER% (%date:~10,4%.%date:~4,2%.%date:~7,2% %hour:~1,2%.%time:~3,2%.%time:~6,2%)
