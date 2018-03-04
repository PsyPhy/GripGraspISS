@echo off
REM
REM A batch file that creates the rigid body models for GRASP.
REM

REM Usually CreateRigidBodyModelFile.exe uses the CodaRTnetContinuous tracker.
REM But you can make it use either the Coda legacy tracker (--legacy) or the
REM daemon tracker (--daemon) instead. In the latter case, you must start up 
REM the daemon before running this file.
set TRACKER=

REM CreateRigidModelFile.exe uses the data from only one of the two CODA cx1 
REM  units because this tends to give better performance. Here one can specify
REM  which unit to use. For the DEX setup, UNIT=1 is the preferred unit. When
REM using only one CODA one must use UNIT=0.
set UNIT=1

set /P NAME=Directory Name: 
set DEST="%date:~-4%-%date:~-7,2%-%date:~-10,2% %NAME%"
mkdir %DEST%

set /P Description=Description: 

REM *****************************************************************************************
REM ***                                                                                   ***
REM ***                                     Chest.bdy                                     ***
REM ***                                                                                   ***
REM *****************************************************************************************

echo Get ready to acquire chest marker structure.
pause

"..\Executables\CreateRigidBodyModelFile.exe" %TRACKER% --unit=%UNIT% 0 1 2 3 4 5 6 7 > %DEST%\measuredChest.bdy
echo ;;; %NAME% - %Description% >>  %DEST%\measuredChest.bdy
"..\Executables\TweakRigidBodyModel.exe" %DEST%\measuredChest.bdy 0.0  %DEST%\Chest.bdy
"..\Executables\TweakRigidBodyModel.exe" %DEST%\measuredChest.bdy 10.0  %DEST%\ChestTilted.bdy

REM *****************************************************************************************
REM ***                                                                                   ***
REM ***                                     Hand.bdy                                      ***
REM ***                                                                                   ***
REM *****************************************************************************************
echo Get ready to acquire hand marker structure.
pause
"..\Executables\CreateRigidBodyModelFile.exe" %TRACKER% --unit=%UNIT% 8 9 10 11 12 13 14 15 > %DEST%\measuredHand.bdy
echo ;;; %NAME% - %Description% >>  %DEST%\measuredHand.bdy
"..\Executables\TweakRigidBodyModel.exe" %DEST%\measuredHand.bdy 0.0  %DEST%\Hand.bdy

REM *****************************************************************************************
REM ***                                                                                   ***
REM ***                                      HMD.bdy                                      ***
REM ***                                                                                   ***
REM *****************************************************************************************
echo Get ready to acquire HMD marker structure.
pause
"..\Executables\CreateRigidBodyModelFile.exe" %TRACKER% --unit=%UNIT% 16 17 18 19 20 21 22 23 > %DEST%\measuredHMD.bdy
echo ;;; %NAME% - %Description% >>  %DEST%\measuredHMD.bdy
"..\Executables\TweakRigidBodyModel.exe" %DEST%\measuredHMD.bdy 0.0  %DEST%\HMD.bdy

pause