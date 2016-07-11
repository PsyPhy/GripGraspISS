REM If OculusMouse is already running, kill it so that we don't have multiple copies.
REM We should probably repeat this in a loop until it is sure that there are no
REM instances running at this time.
TaskKill /IM OculusMouse.exe

REM Now start up OculuseMouse to be able to move the mouse pointer and click 
REM using the Oculus Remote.
REM When GRASP exits, OculusMouse will still be running so that if the exit was accidental
REM  the subject can use the Remote to navigate on the computer and restart.
Start /min "OculusMouse" Executables\OculusMouse.exe

REM Here we actually start Grasp as a background process with no console window.
Start "Grasp Console Window" Executables\GraspGUI.exe



