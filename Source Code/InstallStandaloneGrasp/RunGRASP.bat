@echo off
setlocal 

::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

REM 
REM RunGRASP.bat
REM DOS script used to launch the GRASP GUI and associated processes.
REM See GitHub for futher documentation.
REM Copyright (r) 2016 PsyPhy Consulting.
REM

::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

REM If OculusMouse is already running, kill it so that we don't have multiple copies.
TaskKill /IM OculusMouse.exe

REM Same for the CODA daemon.
TaskKill /IM GraspTrackerDaemon.exe

REM If one of the processes is not already running, TaskKill will generate an error message.
REM This could cause some consternation, even though it is a normal occurance.
REM So clear the console window to hide any message about processes that are not present.
cls

::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

REM The Oculus runtime process may not start up until we try to access the Oculus from within a program.
REM That could be annoyingly too late. So we launch a small process just to get the Oculus runtime to 
REM start up and we take the same opportunity to minimize the window so that it is not a distraction.
Start "Initializing Oculus" /MIN /WAIT GraspExecutables\StartOculus.exe 

REM StartOculus can take some time. Furthermore, it can stall with the Health & Safety warning.
REM OculusStartupCheck.exe is designed to check that the Oculus will indeed run and to prompt the 
REM subject on what to do in case it does to start up correctly.
Start "Wait for Oculus" /WAIT GraspExecutables\OculusStartupCheck.exe 
if NOT ERRORLEVEL 0 GOTO :EOF

:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

REM Now start up OculuseMouse to be able to move the mouse pointer and click using the Oculus Remote.
REM When GRASP exits, OculusMouse will still be running so that if the exit was accidental
REM  the subject can use the Remote to navigate on the computer and restart.
Start "OculusMouse" /MIN GraspExecutables\OculusMouse.exe 

:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

REM Start up the CODA system and wait for it to be active. The Daemon window will minimize automatically.
Start "GRASP Tracker" GraspExecutables\GraspTrackerDaemon.exe --hide

REM Don't proceed until output from the tracker daemon has been detected.
Start "Wait For Tracker" /MIN /WAIT GraspExecutables\WaitForCodaDaemon.exe  
if NOT ERRORLEVEL 0 GOTO :EOF

:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

REM Here we actually start Grasp as a background process with no console window.
Start "Grasp Console Window" GraspExecutables\GraspGUI.exe



