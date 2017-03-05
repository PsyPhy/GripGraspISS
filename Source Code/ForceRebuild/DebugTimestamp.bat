@echo off
setlocal

REM Create a .h file that can be used for force rebuild of other projects. It also
REM defines a preprocessor comment with the current date and time tag created above.
echo // Automatic comment generated to force rebuild of dependent projects. > CommonTimestamp.h
echo // See CommonTimestamp.bat in the CommonTimestamp project for explanation. >> CommonTimestamp.h
echo // CommonTimestamp %date% %time% >> CommonTimestamp.h
echo #define __COMMONTIMESTAMP__ "DEBUG" >> CommonTimestamp.h

REM Create a batch file that can be used to set an envirnment variable to the timestamp tag.
REM This is a crude way to pass information from this batch file to another batch file
REM that may be executed from a makefile at a higher level. It would be easier if we could
REM just set the environment variable here, but it does not seem to persist to other builds
REM called later in the build hierarchy.
echo set __COMMONTIMESTAMP__=DEBUG> %temp%\SetCommonTimestamp.bat
