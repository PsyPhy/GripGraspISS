@echo OFF
REM **************************************************************************

REM GripMMI Startup Script.
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
REM Mirror of GRIP runtime scripts.
REM

REM The client program will look for the GRIP scripts here.
REM The mirror directory must contain an exact copy of the runtime scripts that are 
REM on the GRIP hardware that is generating the telemetry data. You will find one or
REM more candidates in the GripMMIMirrorEnvironments directory.
REM The directory specification can be relative to this .bat file. 

::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
REM Use this option if you want to run in simulation mode using the packets
REM  captured during EST, which is the latest set of packets that we have available.
set ScriptDir="GripMMIMirrorEnvironments\GripFlightRelease (2017.01.24 12.01.27)\Scripts"
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
REM Use the following option instead when operating with the most recent script delivery.
REM set ScriptDir="GripMMIMirrorEnvironments\GripFlightRelease (2018.02.19 04.00.30)\Scripts"
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

REM Provide a set of recorded packets corresponding to the above scripts for simulation mode.
REM Note that this definition has no effect if you are in "-constructed" mode.
REM Use PACKET_SOURCE="GripMMIExecutables\GripPacketsForSimulator.gpk" if you do not have a more specific file.
REM Note that if the scripts do not correspond the behavior is undefined.
REM Here we set a pointer to recorded packets that correspond to the scripts above.

::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
REM Use a set of packets were recorded during EST. Note that these packets were recorded
REM with scripts that are different from the most current version of the scripts. To work properly,
REM you must select the "GripFlightRelease (2017.01.24 12.01.27)" scripts above.
set PACKET_SOURCE="GripMMIMirrorEnvironments\GripFlightRelease (2017.01.24 12.01.27)\GripESTPacketsForSimulator.gpk"
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
REM We do not yet have a set of recorded packets corresponding to "GripFlightRelease (2018.02.19 04.00.30)".
REM When we do, we can set PACKET_SOURCE accordingly so that the latest scripts 
REM can be tested in simulation mode.
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

REM
REM CLWS Emulator for testing
REM 

REM The batch file starts up, by default, a CLWS emulator for local testing.
REM For final installation, comment out the next line to disable the CLWS emulator.
set EMULATE=TRUE

REM The next lines determine the mode of the CLWS emulator.
REM It is currently configured to send out pre-recorded packets.
REM set EMULATE_MODE=-constructed
set EMULATE_MODE=-recorded


REM Host Name or Address of the CLWS Server.
REM Should be 'localhost' if running the CLWSemulator on the same machine (see above).
REM Otherwise, in a typical EPM configuration the CLWS is at address 
REM 192.168.200.103 in the standard configutation and at 192.168.200.102
REM when both the SMSC simulator and the EPM Control Panel are running on
REM the GRIP EGSE laptop.
set HOST=localhost

REM The CLWS also listens on EPMport 2345. This could be problematic if the MMI is using 
REM  the same port for other operattions. The following allows one to change the default port.
REM NB Changing the parameter here changes the setting for the GripMMI (more specifically, for 
REM  GripGroundMonitorClient.exe, which is the process that actually receives packets from the CLWS)
REM  and for the CLWSemulator, so that when performing local testing, they are always in sync.
REM  But changing the port here has no effect on the actual EPM CLWS service. You must set the 
REM  port here to the port used by the EPM or equivalent. The default EPM port number is 2345.
REM  But *DO NOT* simply comment out the following line to get the default, because this will 
REM  generate an anomalous call to GripGroundMonitorClient.exe. PORT must be set to the correct
REM  port number.
set PORT=2345

REM
REM Directory holding cache files
REM

REM The client program will store packets received from the CLWS in this directory.
set CacheDir=d:\DexCache

REM 
REM Location of restart batch file.
REM

REM When the current script is run, it automatically generates a batch file
REM that makes it easy to relaunch the graphical interface without restarting
REM the client process. This batch file must be written in a write-enabled directory.
REM Logically, I would put it in the current directory, because that is where 
REM RunGripMMI.bat (this file) has been found and executed. The corresponging assign would be:
REM
REM set RestartDir=.\
REM
REM But this directory is probably write protected at CADMOS. 
REM So I write it to the cache directory instead:
set RestartDir=%CacheDir%\

REM
REM Alternate Software Unit ID.
REM

REM The CLWS can accept only one connection from a given Software Unit.
REM If you want to use two client connected to the same CLWS server,
REM  uncomment the next line to use the alternate Software Unit ID.
REM set UNIT=-alt

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
REM different formats:  "DD/MM/YYYY" and "Day MM/DD/YYYY". Here I try to identify which one it is
REM and extract accordingly. But you may need to further modify depending on the 
REM format of the %date% variable on your system. To find out, type "echo %date%" in a command window.

REM If the 3rd character is a '/' then we have the "DD/MM/YYYY" format.
if "/"=="%date:~2,1%" goto SHORTDATE
REM If we are here, we presume that we have the "Day MM/DD/YYYY" format.
set TIMESTAMP=%date:~10,4%.%date:~4,2%.%date:~7,2%
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
set CacheRoot=GripPackets.%TIMESTAMP%
set CachePath=%CacheDir%\%CacheRoot%

REM Show where we are for debugging purposes.
echo %cd%

REM Starts up the emulator according to configuration defined above.
if not defined EMULATE goto LAUNCH
REM CLWSemulator takes a mode flag, -constructed or -recorded, 
REM  a flag to set the TCP port number, -port=NNNN and
REM  an optional path to the pre-recorded EPM packets.
start GripMMIExecutables\CLWSemulator.exe %EMULATE_MODE% -port=%PORT% %PACKET_SOURCE%
:LAUNCH
REM Launch the TCP/IP Client.
REM This module receives packets from the CLWS server, selects out the 
REM  Grip packets and sends them to intermediate cache files.
REM First parameter is the path and root for the cache files.
REM Second is the host name or IP address in dot format of the CLWS server.
REM The process is launched with /REALTIME priority to help ensure that
REM  packets from the EPM server are not missed.
start /REALTIME GripMMIExecutables\DexGroundMonitorClient.exe %CachePath% %HOST%:%PORT% %UNIT%

REM Launch a 'lite', text-only version of the MMI, just to see if 
REM  the servers are running and the cache files are filling up.
REM Usually this is disables by commenting the next line.
REM start GripMMIExecutables\GripMMIlite.exe %CachePath% 

REM Start the actual graphical GripMMI.
REM First parameter is the path to the packet caches that serve as inputs.
REM Second paramter is the path to the scripts that are installed on board.
start GripMMIExecutables\GripMMI.exe %CachePath%  %ScriptDir%\ %TIMEBASE_CORRECTION%

REM Now create a batch file that will make it easier to restart the graphical
REM GripMMI.exe interface without restarting GripGroundMonitorClient.exe

set restart_file=%RestartDir%\RestartGripMMI.%TIMESTAMP%.bat
echo REM > %restart_file%
echo REM %restart_file% >> %restart_file%
echo REM >> %restart_file%
echo REM A batch file to restart the GripMMI graphical interface. >> %restart_file%
echo REM This will restart GripMMI.exe with the most recently defined parameters. >> %restart_file%
echo REM It assumes that GripGoundMonitorClient.exe is still running. >> %restart_file%
echo REM If GripGoundMonitorClient.exe is not running, use RunGripMMI.bat instead. >> %restart_file%
echo REM >> %restart_file%
echo REM This batch file can also be used to review previously recorded data >> %restart_file%
echo REM using the GripMMI graphical interface. >> %restart_file%
echo REM >> %restart_file%
echo CD %CD% >> %restart_file%
echo start GripMMIExecutables\GripMMI.exe %CachePath%  %ScriptDir%\ %TIMEBASE_CORRECTION% >> %restart_file%
