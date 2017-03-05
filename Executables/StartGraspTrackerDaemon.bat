@echo off

:::
::: Do a clean start of GraspTrackerDaemon.
::: Copyright (c) 2016 PsyPhy Consulting.
:::

REM If GraspTrackerDaemon is already running, kill it so that we don't have multiple copies.
TaskKill /IM GraspTrackerDaemon.exe
REM Start up the CODA system and wait for it to be active.
Start "GRASP Tracker" GraspTrackerDaemon.exe
Start /MIN /WAIT WaitForCodaDaemon.exe    