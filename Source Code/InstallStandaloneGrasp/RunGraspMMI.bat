REM **************************************************************************

REM GraspMMI Startup Script.
REM Copyright (c) 2014, 2015, 2016 PsyPhy Consulting
REM All rights reserved.

REM Starts up the various modules that make up the GraspMMI, 
REM  including a CLSW Server emulator if needed for testing.

REM **************************************************************************

REM Edit the definitions in this section according to the installation.

REM
REM CLWS Emulator for testing
REM 

REM The batch file starts up, by default, a CLWS emulator for local testing.
REM For final installation, comment out the next line to disable the CLWS emulator.
set EMULATE=TRUE

REM The next lines determine the mode of the CLWS emulator.
REM Here it is configured to output simulated GRASP packets.
REM Possible values are -recorded and -grasp.
set EMULATE_MODE=-recorded

REM You can substitute a different file as the source of the recorded packets
REM  by changing the following definition. You can also tell it to skip a
REM  certain number of initial frames.
REM Note that this definition has no effect if you are in "-grasp" mode. 
set PACKET_SOURCE="Executables\GraspPacketsForSimulator.gpk" -skip=480

REM Host Name or Address of the CLWS Server.
REM Can be 'localhost' if running on same machine.
REM Should be 'localhost' if running the CLWSemulator on the same machine (see above).
REM Otherwise, in a typical EPM configuration the CLWS is at address 
REM 192.168.200.103 in the standard configutation and at 192.168.200.102
REM when both the SMSC simulator and the EPM Control Panel are running on
REM the GRIP EGSE laptop.
set HOST=localhost

REM
REM Directory holding cache files
REM

REM The client program will store packets received from the CLWS in this directory.
REM Note that you should include a final backslash.
set CacheDir=Cache\

REM
REM Alternate Software Unit ID and Port number.
REM

REM The CLWS can accept only one connection from a given Software Unit.
REM If you want to use two client connected to the same CLWS server,
REM  uncomment the next line to use the alternate Software Unit ID.
REM set UNIT=-alt

REM The CLWS also listens on EPMport 2345. This could be problematic if the MMI is using 
REM  the same port for other operattions. The following allows one to change to a 
REM  different port number, i.e. to 2346 when running connected to the GRIP EGSE.
REM NB Changing the parameter here changes the setting for the GripMMI (more specifically, for 
REM  GripGroundMonitorClient.exe, which is the process that actually receives packets from the CLWS)
REM  and for the CLWSemulator, so that when performing local testing, they are always in sync.
REM  But changing the port here has no effect on the actual EPM CLWS service. You must set the 
REM  port here to the port used by the EPM or equivalent. The default EPM port number is 2345.
REM  But *DO NOT* simply comment out the following line to get the default, because this will 
REM  generate an anomalous call to GripGroundMonitorClient.exe. PORT *must* be set to the correct
REM  port number.
set PORT=2345

REM 
REM UTC versus GPS time of day.
REM

REM EPM used GPS time-of-day, but CADMOS displays UTC time. There is a small shift between them,
REM  because UTC takes into account leap seconds, while GPS does not.
REM Default value is -16 which corrects for leap seconds as of Jan. 2015.
REM Modify the value if new leap seconds have been added (e.g. after July 2015 change to -17).

REM Uncomment the next line if you want to display times aligned with UTC rather than GPS
set TIMEBASE_CORRECTION=-16
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

REM Normally you will not need to edit below this line.

REM Root of the file names for the cache files
set CacheRoot=GraspPackets.%TIMESTAMP%

REM Show where we are for debugging purposes.
echo %cd%

REM Starts up the emulator according to configuration defined above.
if not defined EMULATE goto LAUNCH
REM CLWSemulator takes a mode flag, -constructed or -recorded, 
REM  a flag to set the TCP port number, -port=NNNN and
REM  an optional path to the pre-recorded EPM packets.
start Executables\CLWSemulator.exe %EMULATE_MODE% -port=%PORT% %PACKET_SOURCE%

:LAUNCH
REM Launch the TCP/IP Client.
REM This module receives packets from the CLWS server, selects out the 
REM  Grip packets and sends them to intermediate cache files.
REM First parameter is the path and root for the cache files.
REM Second is the host name or IP address in dot format of the CLWS server.
REM The process is launched with /REALTIME priority to help ensure that
REM  packets from the EPM server are not missed.
start /REALTIME Executables\DexGroundMonitorClient.exe %CacheDir%%CacheRoot% %HOST%:%PORT% %UNIT%

REM Start the actual graphical GraspMMI.
REM First parameter is the path to the packet caches that serve as inputs.
start Executables\GraspMMI.exe %CacheDir%\%CacheRoot% 
