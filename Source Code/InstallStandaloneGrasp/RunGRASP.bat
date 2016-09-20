@echo off
setlocal 

REM 
REM RunGRASP.bat
REM DOS script used to launch the GRASP GUI and associated processes.
REM See GitHub for futher documentation.
REM Copyright (r) 2016 PsyPhy Consulting.
REM

REM If OculusMouse is already running, kill it so that we don't have multiple copies.
TaskKill /IM OculusMouse.exe

REM Same for the CODA daemon.
TaskKill /IM GraspTrackerDaemon.exe

REM Clear the console window to hide any message about processes that are not present.
cls

REM Now start up Oculus.
Start /MIN /WAIT Executables\StartOculus.exe 

REM Now start up OculuseMouse to be able to move the mouse pointer and click using the Oculus Remote.
REM When GRASP exits, OculusMouse will still be running so that if the exit was accidental
REM  the subject can use the Remote to navigate on the computer and restart.
Start /MIN "OculusMouse" Executables\OculusMouse.exe 

REM Start up the CODA system and wait for it to be active.
Start "GRASP Tracker" Executables\GraspTrackerDaemon.exe
Start "Wait For Tracker" /MIN /WAIT Executables\WaitForCodaDaemon.exe  

REM Here we actually start Grasp as a background process with no console window.
Start "Grasp Console Window" Executables\GraspGUI.exe



