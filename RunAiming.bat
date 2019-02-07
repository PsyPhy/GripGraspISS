@echo off
setlocal 

::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

REM 
REM RunAIMING.bat
REM DOS script used to launch the GripAimingTool GUI and associated processes.
REM See GitHub for futher documentation.
REM Copyright (r) 2016 - 1019 PsyPhy Consulting.
REM
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

REM If CODA Daemon is already running, kill it so that we don't have multiple copies.
TaskKill /IM GraspTrackerDaemon.exe

REM If one of the processes is not already running, TaskKill will generate an error message.
REM This could cause some consternation, even though it is a normal occurance.
REM So clear the console window to hide any message about processes that are not present.
cls

REM Start up the CODA system and wait for it to be active. The Daemon window will minimize automatically.
Start "GRASP Tracker" Executables\GraspTrackerDaemon.exe --hide

REM Don't proceed until output from the tracker daemon has been detected.
Start "Wait For Tracker" /MIN /WAIT Executables\WaitForCodaDaemon.exe  
if NOT ERRORLEVEL 0 GOTO :EOF

REM Here we actually start the Grasp user interface.
Executables\GripAimingTool.exe 

REM Kill the tracker daemon.
TaskKill /IM GraspTrackerDaemon.exe

REM Remove the flag from DEX so that it can go on with GRIP.
WinSCP.com /command "open ftp://3mbit:dex@192.168.200.133" "cd DATA1" "rm aiming.flg" exit



