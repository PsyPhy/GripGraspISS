@echo OFF

REM **************************************************************************

REM GraspMMI Startup Script.
REM Copyright (c) 2014, 2015, 2016 PsyPhy Consulting
REM All rights reserved.

REM Starts up the various modules that make up the GraspMMI, 
REM  including a CLSW Server emulator if needed for testing.

REM **************************************************************************
REM *                                                                        *
REM *  Edit the definitions in this section according to the installation.   *
REM *                                                                        *
REM **************************************************************************

REM
REM Mirror of GRASP runtime environment.
REM

REM The client program will look for the GRASP scripts, instructions, sequences, etc. here.
REM The mirror directory must contain an exact copy of the runtime environment that is 
REM on the Perspectives laptop that is generating the telemetry data. You will find one or
REM more candidates in the GraspMMIMirrorEnvironments directory.
REM The directory specification can be relative to this .bat file. 
REM set MirrorDir=GraspMMIMirrorEnvironments\GraspProximaFlight
set MirrorDir="GraspMMIMirrorEnvironments\GRASP Runtime Flight (2017-11-02 16h20m20s)"

REM
REM CLWS Emulator for testing
REM 

REM The batch file starts up, by default, a CLWS emulator for local testing.
REM For final installation, comment out the next line to disable the CLWS emulator.
set EMULATE=TRUE

REM The next lines determine the mode of the CLWS emulator.
REM Possible values are -recorded to play back recorded packets 
REM and -grasp to send out simulated grasp packets.
set EMULATE_MODE=-recorded

REM You can substitute a different file as the source of the recorded packets
REM  by changing the following definition. You can also tell it to skip a
REM  certain number of initial frames by including -skip=nnn in the definition.
REM Note that this definition has no effect if you are in "-grasp" mode. 

REM The mirror directory may have a set of packets that were acquired specifically
REM  from the corresponding GRASP runtime environment. If so, choose that file.
REM  You can skip some initial packets and set a speed greater than one if you want
REM  to see things happen faster than real time by setting the flag "-speed=N" 
REM  (where N = 1 - 100), but if you set it too fast you will miss some events
REM  and the stripchart data plots will be corrupted.
REM set PACKET_SOURCE=%MirrorDir%\GraspESTPacketsForSimulator.gpk -skip=5280 -speed=1
set PACKET_SOURCE=%MirrorDir%\GraspPackets.2017.11.09.any.gpk -skip=4800 -speed=1

REM Otherwise, you can uncomment the following line to use the default packets, but if the
REM  runtime environment does not match you will get unpredictable results.
REM set PACKET_SOURCE="GraspMMIExecutables\GraspPacketsForSimulator.gpk" -skip=480

REM
REM Connection to EPM
REM

REM Host Name or Address of the CLWS Server.
REM Should be 'localhost' if running the CLWSemulator on the same machine (see above).
REM Otherwise, in a typical EPM configuration the CLWS is at address 
REM 192.168.200.103 in the standard configutation and at 192.168.200.102
REM when both the SMSC simulator and the EPM Control Panel are running on
REM the GRIP EGSE laptop.
set HOST=localhost

REM The CLWS also listens on EPMport 2345. This could be problematic if the MMI is using 
REM  the same port for other operattions. The following allows one to change to a 
REM  different port number, i.e. to 2346 when running connected to the GRIP EGSE.
REM NB Changing the parameter here changes the setting for the GraspMMI (more specifically, for 
REM  DexGroundMonitorClient.exe, which is the process that actually receives packets from the CLWS)
REM  and for the CLWSemulator, so that when performing local testing, they are always in sync.
REM  But changing the port here has no effect on the actual EPM CLWS service. You must set the 
REM  port here to the port used by the EPM or equivalent. The default EPM port number is 2345.
REM  But *DO NOT* simply comment out the following line to get the default, because this will 
REM  generate an anomalous call to GripGroundMonitorClient.exe. PORT *must* be set to the correct
REM  port number.
set PORT=2345

REM The CLWS can accept only one connection from a given Software Unit.
REM If you want to use two client connected to the same CLWS server,
REM  uncomment the next line to use the alternate Software Unit ID.
REM set UNIT=-alt

REM
REM Directory holding cache files
REM

REM The client program will store packets received from the CLWS in this directory.
REM *** The path defined here MUST be an absolute path.
set CacheDir=c:\DexCache

REM 
REM UTC versus GPS time of day.
REM

REM EPM used GPS time-of-day, but CADMOS displays UTC time. There is a small shift between them,
REM  because UTC takes into account leap seconds, while GPS does not.
REM Set to a value of -18 which corrects for leap seconds as of Jan. 2017.
REM Modify the value if new leap seconds have been added.
REM https://confluence.qps.nl/qinsy/en/utc-to-gps-time-correction-32245263.html

REM Uncomment the next line if you want to display times aligned with UTC rather than GPS
set TIMEBASE_CORRECTION=-18
REM Uncomment the next line if you want to display times aligned with GPS time.
REM set TIMEBASE_CORRECTION=0

REM **************************************************************************

REM Create a common timestamp in the form YYYY.MM.DD to be used in various file names.

REM Hopefully you will not need to edit this section, but it depends on the Windows configuration.
REM Here we extract the date from the environment variable %date%. But I have seen two
REM different formats:  "DD/MM/YYYY" and "Day MM/DD/YYYY". I try to identify which one it is
REM and extract accordingly. But you may need to further modify depending on the 
REM format of the %date% variable on your system. To find out, type "echo %date%" in a command window.

REM If the 3rd character is a '/' then we have the "DD/MM/YYYY" format.
if "/"=="%date:~2,1%" goto SHORTDATE
REM If the 4th character is a '.' then we have the "day. DD/MM/YYYY" format.
if "."=="%date:~3,1%" goto BELGIAN
REM If we are here, we presume that we have the "Day MM/DD/YYYY" format.
set TIMESTAMP=%date:~10,4%.%date:~4,2%.%date:~7,2%
goto NEXT
:BELGIAN
set TIMESTAMP=%date:~11,4%.%date:~8,2%.%date:~5,2%
goto NEXT
:SHORTDATE
set TIMESTAMP=%date:~6,4%.%date:~3,2%.%date:~0,2%
:NEXT

REM **************************************************************************
REM **************************************************************************
REM **                                                                      **
REM **          Normally you will not need to edit below this line.         **
REM **                                                                      **
REM **************************************************************************
REM **************************************************************************

REM If the MMIs were already running, kill them and start over.
TaskKill /F /IM GraspMMI*
TaskKill /F /IM GripMMI*
TaskKill /F /IM DexGroundMonitorClient*
TaskKill /F /IM CLWSemulator*

REM Root of the file names for the cache files
REM This need only be the root of the filename.
REM CacheDir defined above will be prepended to create the path
REM to the packet files.
set CacheRoot=GraspPackets.%TIMESTAMP%
set CachePath=%CacheDir%\%CacheRoot% 

REM Show where we are for debugging purposes.
echo %cd%

REM Starts up the emulator according to configuration defined above.
if not defined EMULATE goto LAUNCH

REM CLWSemulator takes a mode flag, -constructed or -recorded, 
REM  a flag to set the TCP port number, -port=NNNN and
REM  an optional path to the pre-recorded EPM packets.
start GraspMMIExecutables\CLWSemulator.exe -port=%PORT% %EMULATE_MODE% %PACKET_SOURCE%

:LAUNCH
REM Launch the TCP/IP Client.
REM This module receives packets from the CLWS server, selects out the 
REM  Grip packets and sends them to intermediate cache files.
REM First parameter is the path and root for the cache files.
REM Second is the host name or IP address in dot format of the CLWS server.
REM The process is launched with /REALTIME priority to help ensure that
REM  packets from the EPM server are not missed.
start /REALTIME GraspMMIExecutables\DexGroundMonitorClient.exe %CachePath% %HOST%:%PORT% %UNIT%

REM Start the actual GraspMMI Mirror and Data Plots.
REM First parameter is the path to the packet caches that serve as inputs.
start /D %MirrorDir% GraspMMIExecutables\GraspMMIMirror.exe %CachePath%
start /D %MirrorDir% GraspMMIExecutables\GraspMMIHistory.exe %CachePath% 
start /D %MirrorDir% GraspMMIExecutables\GraspMMIDataPlots.exe %CachePath% 
