@echo off
setlocal

REM This batch updates a .h file each time it is built and it is always built.
REM If you want to force a project to rebuild every time, include
REM CommonTimestamp.h in the files that you want to recompile and make your project
REM dependent on this one.

REM CommonTimestamp.h defines a preprocessor variable, __COMMONTIMESTAMP__, that 
REM you can use in the programs to imbed the build time into your code.
REM For Grip/Grasp I compile it into a small library, GripGraspVersionControl, that is common to
REM all the executables in a given set. That way they all report the same build info.

REM It also creates a batch file in the %temp% directory that sets a global environment
REM variable to the common timestamp. You can use this later in build commands
REM to, for instance, include the common timestamp in your filenames created by the make.
REM I use this to allow my distribution TAR archives have the same timestamp as the
REM build timestamp reported by the executables.

REM Typically, I exclude the CommonTimestamp project from the Debug
REM build configuration, so that the dependent projects do not 
REM necessarily get built each time a build is performed.
REM I use this mechanism with GripGraspVersionControl. I want 
REM GripGraspVersionControl to be rebuilt whenever I create a release
REM so that the shared version info is the same across executables,
REM but I don't care so much when I am in debug mode.

REM Create a file tag with date and time, making sure that the hour has a leading zero.
if "%time:~0,1%" EQU " " (set HOUR=0%time:~1,1%) else (set HOUR=%time:~0,2%)
REM Create a date string in the form YYYY-MM-DD, trying to take into account the data format on the machine.
REM If the first field is the day of the week, then the 4th character will be a blank and we know that 
REM we are in US format MM-DD-YYYY. Otherwise we assume European format DD-MM-YYYY. We transform either
REM of them into YYYY-MM-DD.

if "%date:~3,1%" NEQ " " goto :NOTUS
set DTT=%date:~10,4%-%date:~4,2%-%date:~7,2%
goto :NEXT

:NOTUS
if "%date:~3,1%" EQU "." (set DTT=%date:~11,4%-%date:~8,2%-%date:~5,2%) else (set DTT=%date:~6,4%-%date:~3,2%-%date:~0,2%)

:NEXT
set TAG=%DTT% %HOUR%h%time:~3,2%m%time:~6,2%s

REM Create a .h file that can be used for force rebuild of other projects. It also
REM defines a preprocessor comment with the current date and time tag created above.
echo // Automatic comment generated to force rebuild of dependent projects. > CommonTimestamp.h
echo // See CommonTimestamp.mak in the CommonTimestamp project for explanation. >> CommonTimestamp.h
echo // CommonTimestamp %date% %time% >> CommonTimestamp.h
echo #define __COMMONTIMESTAMP__ "%TAG%" >> CommonTimestamp.h

REM Create a batch file that can be used to set an envirnment variable to the timestamp tag.
REM This is a crude way to pass information from this batch file to another batch file
REM that may be executed from a makefile at a higher level. It would be easier if we could
REM just set the environment variable here, but it does not seem to persist to other builds
REM called later in the build hierarchy.
echo set __COMMONTIMESTAMP__=%TAG%> %temp%\SetCommonTimestamp.bat
