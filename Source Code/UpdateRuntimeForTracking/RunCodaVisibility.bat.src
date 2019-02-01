@echo off
setlocal 

REM 
REM RunCodaVisibility.bat
REM DOS script used to launch the GraspTrackerDaemon processes.
REM See GitHub for futher documentation.
REM Copyright (r) 2016 PsyPhy Consulting.
REM

REM If daemon is already running, kill it so that we don't have multiple copies.
TaskKill /IM CodaVisibility.exe

REM Clear the console window to hide any message about processes that are not present.
cls

REM Start up the application.
REM Add the --daemon flag if you want to get the data from GraspTrackerDaemon
REM instead of directly from the CODA.
Start "CODA Visibility" Executables\CodaVisibility.exe --daemon

