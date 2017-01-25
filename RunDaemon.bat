@echo off
setlocal 

REM 
REM RunDaemon.bat
REM DOS script used to launch the GraspTrackerDaemon processes.
REM See GitHub for futher documentation.
REM Copyright (r) 2016 PsyPhy Consulting.
REM

REM If daemon is already running, kill it so that we don't have multiple copies.
TaskKill /IM GraspTrackerDaemon.exe

REM Start up the CODA system and wait for it to be active.
Start "GRASP Tracker" GraspExecutables\GraspTrackerDaemon.exe 
Start "Wait For Tracker" /MIN /WAIT GraspExecutables\WaitForCodaDaemon.exe  

