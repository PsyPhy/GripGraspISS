@echo off
setlocal 

REM 
REM RunCharnwoodAlignment.bat
REM DOS script used to launch the GraspTrackerDaemon processes.
REM See GitHub for futher documentation.
REM Copyright (r) 2016 PsyPhy Consulting.
REM

REM If daemon is already running, kill it so that we don't have multiple copies.
TaskKill /IM CharnwoodAlignment.exe

REM If daemon is  running, kill because CharnwoodAlignment must access
REM the CodaRTnet server directly.
TaskKill /IM GraspTrackerDaemon.exe


REM Clear the console window to hide any message about processes that are not present.
cls

REM Start up the CODA system and wait for it to be active.
REM Command line arguments say which markers to use to define the 
REM common laboratory reference frame.
REM Note that it is possible to use the same marker for 2 purposes,
REM e.g. marker 0 could be the origin, x_minus and y_minus all at once.
Start "Charnwood Alignment" Executables\CharnwoodAlignment.exe ^
	--origin=0  --x_minus=1  --x_plus=2  --y_minus=3  --y_plus=4
