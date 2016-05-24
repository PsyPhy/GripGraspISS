install: *.gsp
	copy /Y /V *.gsp ..\..\Scripts
	echo %date% %time% > $@