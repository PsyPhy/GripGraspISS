:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:::                                                                                     :::
:::                                      Hook2.bat                                      :::
:::                                                                                     :::
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
echo off

REM Batch file to intercept DEX startup in GRIP mode to allow use of the GripAimingTool.
REM DEX will be started temporarily in GRASP mode to allow communication with the EMP laptop.
REM Then thanks to a simple handshake protocol based on FTP of a cookie file, the 
REM GRASP ASW will be stopped and normal boot in GRIP mode will proceed.

REM ****************************************************************************************
REM                       This must be installed in c:\ASW on DEX.
REM ****************************************************************************************

set COOKIEDIR=c:\
set AIMINGCOOKIE=%COOKIEDIR%\AIMING.FLG
set READYCOOKIE=%COOKIEDIR%\READY.FLG

echo Aiming cookie is %AIMINGCOOKIE%
echo Ready cookie is %READYCOOKIE%

REM Remove the old ready flag, if it exists.
if exist %READYCOOKIE% del %READYCOOKIE%

if exist %AIMINGCOOKIE% (

	echo Killing codaRTServer
	c:\utils\pskill codaRTServer
	c:\utils\sleep 5000

	echo Launching GRASPASW to allow use of aiming tool on EPM laptop.

	;;; Use pushd here, not cd, so that we can get back to where we started.
	pushd \GRASP
	if exist Hook1.bat call Hook1.bat
	cd \Codamotion\RTNet\Binaries
	start /b codaRTServer.exe
	rem Wait for 3 seconds - ensure server has started
	c:\utils\sleep 3000
	cd \GRASP
	if exist Hook2.bat call Hook2.bat
	start /b GRASPASW.exe
	c:\utils\sleep 3000
	echo I am ready. >%READYCOOKIE%

)

:LOOP

if exist %AIMINGCOOKIE% (  

	c:\utils\sleep 2000
	echo Looping.
	goto LOOP 

)

REM Remove the ready flag in preparation for next time.
echo Deleting %READYCOOKIE%
del /Q /F %READYCOOKIE%

REM Stop the GRASP ASW.
echo Killing GRASPASW
c:\utils\pskill GRASPASW
c:\utils\sleep 3000

REM Get back to the directory that we came from.
popd

echo Go on your way.
