@echo off
REM If OculusMouse is already running, kill it so that we don't have multiple copies.
TaskKill /IM OculusMouse.exe

REM Same for the CODA daemon.
TaskKill /IM GraspTrackerDaemon.exe

REM Output some blank lines to hide any errors due to the fact that the processes do not exist;
echo.
echo.
echo.
echo.
echo.
echo.

REM Now start up OculuseMouse to be able to move the mouse pointer and click 
REM using the Oculus Remote.
REM When GRASP exits, OculusMouse will still be running so that if the exit was accidental
REM  the subject can use the Remote to navigate on the computer and restart.
Start /min "OculusMouse" Executables\OculusMouse.exe

REM Start up the CODA system
Start "GRASP Tracker" Executables\GraspTrackerDaemon.exe --nocoda --confirm
REM Wait for it to complete initialization.
@echo **********************************************************************************************
@echo **********************************************************************************************
@echo ***                                                                                        ***
@echo ***                             Starting up 3D Tracking system.                            ***
@echo ***                                      Please Wait                                       ***
@echo ***                                                                                        ***
@echo **********************************************************************************************
@echo **********************************************************************************************
@echo.
timeout /T 8 /NOBREAK

REM Here we actually start Grasp as a background process with no console window.
Start "Grasp Console Window" Executables\GraspGUI.exe



