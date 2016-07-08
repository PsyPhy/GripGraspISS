REM If OculusMouse is already running, kill it so that we don't have multiple copies.03:26 08/07/2016
TaskKill /IM OculusMouse.exe

REM Now start up OculuseMouse to be able to move the mouse pointer and click 
REM using the Oculus Remote.
Start "OculusMouse" Executables\OculusMouse.exe

REM Here we actually start Grasp.
Executables\GraspGUI.exe

REM When GRASP exits, we leave OculusMouse running so that if the exit was accidental
REM  the subject can use the Remote to navigate on the computer and restart.


