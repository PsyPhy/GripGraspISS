@echo off
REM If OculusMouse is already running, kill it so that we don't have multiple copies.
TaskKill /IM OculusMouse.exe

REM Same for the CODA daemon.
TaskKill /IM GraspTrackerDaemon.exe

REM Clear the console screen to hide the messages about processes that don't exist.
cls

REM Now start up OculuseMouse to be able to move the mouse pointer and click 
REM using the Oculus Remote.
REM When GRASP exits, OculusMouse will still be running so that if the exit was accidental
REM  the subject can use the Remote to navigate on the computer and restart.
Start /min "OculusMouse" Executables\OculusMouse.exe
@echo **********************************************************************************************
@echo **********************************************************************************************
@echo ***                                                                                        ***
@echo ***                               Starting up Oculus system.                               ***
@echo ***                                      Please Wait                                       ***
@echo ***                                                                                        ***
@echo **********************************************************************************************
@echo **********************************************************************************************
@echo.
timeout /T 10 /NOBREAK
cls
@echo **********************************************************************************************
@echo **********************************************************************************************
@echo ***                                                                                        ***
@echo ***               Please press the center button on the VR Headset Remote                  ***
@echo ***                              and hold for 2 seconds.                                   ***
@echo ***                          Then press 'Return' to continue.                              ***
@echo ***                                                                                        ***
@echo **********************************************************************************************
@echo **********************************************************************************************
@echo.
pause
cls

REM Start up the CODA system
Start "GRASP Tracker" Executables\GraspTrackerDaemon.exe
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
timeout /T 10 /NOBREAK

REM Here we actually start Grasp as a background process with no console window.
Start "Grasp Console Window" Executables\GraspGUI.exe



