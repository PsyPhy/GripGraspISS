SETLOCAL ENABLEDELAYEDEXPANSION

pushd ..\.. 

set SIZE=1600x1200:0,-1440
set EXECUTABLE="Source Code\Debug\GraspScreenshots.exe"

set TARGET=25.0
set HEAD=15.0

set COUNT=100

set CONFIRM=
 
"Source Code\Debug\GraspScreenshots.exe" --shoulders --head=0.0 %CONFIRM%  --size=%SIZE% Image%COUNT%.Shoulders.bmp
set /A COUNT="COUNT+1"

"Source Code\Debug\GraspScreenshots.exe" --target=25.0	--head=0.0 %CONFIRM%  --size=%SIZE% Image%COUNT%.Target1.bmp
set /A COUNT="COUNT+1"

"Source Code\Debug\GraspScreenshots.exe" --headArrowRight --headError=-15.0 --head=0.0 %CONFIRM%  --size=%SIZE% Image%COUNT%.Tilt1.bmp
set /A COUNT="COUNT+1"
"Source Code\Debug\GraspScreenshots.exe"  --headError=-5.0 --head=10.0 %CONFIRM%  --size=%SIZE% Image%COUNT%.Tilt2.bmp
set /A COUNT="COUNT+1"
"Source Code\Debug\GraspScreenshots.exe"  --headError=-2.0 --head=13.0 %CONFIRM%  --size=%SIZE% Image%COUNT%.Tilt3.bmp
set /A COUNT="COUNT+1"
"Source Code\Debug\GraspScreenshots.exe"  --headError=-1.0 --head=14.0 %CONFIRM%  --size=%SIZE% Image%COUNT%.Tilt4.bmp
set /A COUNT="COUNT+1"
"Source Code\Debug\GraspScreenshots.exe"  --headError=0.0 --head=HEAD %CONFIRM%  --size=%SIZE% Image%COUNT%.Tilt5.bmp
set /A COUNT="COUNT+1"

set ROLL=20.0
set PITCH=-20.0
set YAW=-10
"Source Code\Debug\GraspScreenshots.exe" --head=HEAD --roll=%ROLL% --pitch=%PITCH% --yaw=%YAw% --targetSphere --vkTool --laser --handError=0.5 %CONFIRM%  --size=%SIZE% Image%COUNT%.Aim1.bmp
set /A COUNT="COUNT+1"

set ROLL=20.0
set PITCH=-10.0
set YAW=-5.0
"Source Code\Debug\GraspScreenshots.exe" --head=HEAD --roll=%ROLL% --pitch=%PITCH% --yaw=%YAw% --targetSphere --vkTool --laser --handError=0.5 %CONFIRM%  --size=%SIZE% Image%COUNT%.Aim2.bmp
set /A COUNT="COUNT+1"

set ROLL=20.0
set PITCH=-5.0
set YAW=-3.0
"Source Code\Debug\GraspScreenshots.exe" --head=HEAD --roll=%ROLL% --pitch=%PITCH% --yaw=%YAw% --targetSphere --vkTool --laser --handError=0.5 %CONFIRM%  --size=%SIZE% Image%COUNT%.Aim3.bmp
set /A COUNT="COUNT+1"

set ROLL=20.0
set PITCH=-5.0
set YAW=-3.0
"Source Code\Debug\GraspScreenshots.exe" --head=HEAD --roll=%ROLL% --pitch=%PITCH% --yaw=%YAw% --target=25.0	--vkTool --laser --handError=0.5 %CONFIRM%  --size=%SIZE% Image%COUNT%.Shot000.bmp
set /A COUNT="COUNT+1"

set SHOW_HAND=--vkTool
for /L %%i in (200,200,1000) do (
	echo iteration %%i
	"Source Code\Debug\GraspScreenshots.exe" --projectiles=%%i --head=HEAD --roll=%ROLL% --pitch=%PITCH% --yaw=%YAw% --target=25.0	!SHOW_HAND!  %CONFIRM%  --size=%SIZE% Image!COUNT!.Shot%%i.bmp
	set SHOW_HAND=
	call :up
	
)
"Source Code\Debug\GraspScreenshots.exe" --head=HEAD --roll=%ROLL% --pitch=%PITCH% --yaw=%YAw% --TrialCompleted %CONFIRM%  --size=%SIZE% Image%COUNT%.Success.bmp
set /A COUNT="COUNT+1"
"Source Code\Debug\GraspScreenshots.exe" --head=HEAD --roll=%ROLL% --pitch=%PITCH% --yaw=%YAw% --BlockCompleted %CONFIRM%  --size=%SIZE% Image%COUNT%.Finished.bmp

	
pause

goto :eof

:up
set /A COUNT="COUNT+1"
goto :eof