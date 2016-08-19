@echo on
REM If GraspTrackerDaemon is already running, kill it so that we don't have multiple copies.
TaskKill /IM GraspTrackerDaemon.exe
REM Start up the CODA system
Start "GRASP Tracker" Executables\GraspTrackerDaemon.exe  /B
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

