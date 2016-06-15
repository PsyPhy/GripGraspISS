### Keep the GRASP top level execution environment curent.

install: ../Debug/Grasp.exe
	copy ../Debug/Grasp.exe ../bin
	echo %date% %time% > $@