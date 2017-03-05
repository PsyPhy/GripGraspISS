@echo off
REM
REM A batch file that creates the rigid body models for GRASP.
REM This instance is used for testing different options.
REM

REM Usually CreateRigidBodyModelFile.exe uses the CodaRTnetContinuous tracker.
REM But you can make it use either the Coda legacy tracker (--legacy) or the
REM daemon tracker (--daemon) instead. In the latter case, you must start up 
REM the daemon before running this file.
set TRACKER=--daemon

REM CreateRigidModelFile.exe uses the data from only one of the two CODA cx1 
REM  units because this tends to give better performance. Here one can specify
REM  which unit to use. For the DEX setup, UNIT=1 is the preferred unit. When
REM using only one CODA, UNIT=0 must be used.
set UNIT=0

set /P NAME=Directory Name: 
set DEST="%date:~-4%-%date:~-7,2%-%date:~-10,2% %NAME%"
mkdir %DEST%

set /P Description=Description: 

REM *****************************************************************************************
REM ***                                                                                   ***
REM ***                                     Chest.bdy                                     ***
REM ***                                                                                   ***
REM *****************************************************************************************
"..\GraspExecutables\CreateRigidBodyModelFile.exe" %TRACKER% --unit=%UNIT% 16 17 18 19 20 21 22 23 > %DEST%\Chest.bdy
echo ;;; %NAME% - %Description% >>  %DEST%\Chest.bdy

REM *****************************************************************************************
REM ***                                                                                   ***
REM ***                                     Hand.bdy                                      ***
REM ***                                                                                   ***
REM *****************************************************************************************
"..\GraspExecutables\CreateRigidBodyModelFile.exe" %TRACKER% --unit=%UNIT% 16 17 18 19 20 21 22 23 > %DEST%\Hand.bdy
echo ;;; %NAME% - %Description% >>  %DEST%\Hand.bdy


REM *****************************************************************************************
REM ***                                                                                   ***
REM ***                                      HMD.bdy                                      ***
REM ***                                                                                   ***
REM *****************************************************************************************
"..\GraspExecutables\CreateRigidBodyModelFile.exe" %TRACKER% --unit=%UNIT% 16 17 18 19 20 21 22 23 > %DEST%\HMD.bdy
echo ;;; %NAME% - %Description% >>  %DEST%\HMD.bdy

pause