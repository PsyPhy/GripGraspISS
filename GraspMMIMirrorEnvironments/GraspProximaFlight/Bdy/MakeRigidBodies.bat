@echo off
REM
REM A batch file that creates the rigid body models for GRASP.
REM

REM CreateRigidModelFile.exe uses the data from only one of the two CODA cx1 
REM  units because this tends to give better performance. Here one can specify
REM  which unit to use. 
set UNIT=1


REM *****************************************************************************************
REM ***                                                                                   ***
REM ***                                     Chest.bdy                                     ***
REM ***                                                                                   ***
REM *****************************************************************************************
"..\Executables\CreateRigidBodyModelFile.exe" --unit=%UNIT% 0 1 2 3 4 5 6 7 > Chest.bdy

REM *****************************************************************************************
REM ***                                                                                   ***
REM ***                                     Hand.bdy                                      ***
REM ***                                                                                   ***
REM *****************************************************************************************
"..\Executables\CreateRigidBodyModelFile.exe" --unit=%UNIT% 8 9 10 11 12 13 14 15 > Hand.bdy


REM *****************************************************************************************
REM ***                                                                                   ***
REM ***                                      HMD.bdy                                      ***
REM ***                                                                                   ***
REM *****************************************************************************************
"..\Executables\CreateRigidBodyModelFile.exe" --unit=%UNIT% 16 17 18 19 20 21 22 23 > HMD.bdy

