### Keep the GRASP top level execution environment current.

EXECUTABLES = \
	..\$(BUILDCONFIGURATION)\GraspTrackerDaemon.exe \
	..\$(BUILDCONFIGURATION)\WaitForCodaDaemon.exe \
	..\$(BUILDCONFIGURATION)\CharnwoodAlignment.exe \
	..\$(BUILDCONFIGURATION)\CodaVisibility.exe \
	RunTrackerDaemon.bat.src \
	RunCodaVisibility.bat.src \
	RunCharnwoodAlignment.bat.src 

ROOT=..\..
DESTINATION = $(ROOT)\Executables

install: $(EXECUTABLES) InstallTracking.mak 
	-rmdir /S /Q $(DESTINATION)
	mkdir $(DESTINATION)
	copy ..\$(BUILDCONFIGURATION)\GraspTrackerDaemon.exe $(DESTINATION)
	copy ..\$(BUILDCONFIGURATION)\CharnwoodAlignment.exe $(DESTINATION)
	copy ..\$(BUILDCONFIGURATION)\CodaVisibility.exe $(DESTINATION)
	copy ..\$(BUILDCONFIGURATION)\WaitForCodaDaemon.exe $(DESTINATION)
	copy RunTrackerDaemon.bat.src $(ROOT)\RunTrackerDaemon.bat
	copy RunCodaVisibility.bat.src $(ROOT)\RunCodaVisibility.bat
	copy RunCharnwoodAlignment.bat.src $(ROOT)\RunCharnwoodAlignment.bat
	-mkdir $(DESTINATION)\CodaCalFiles
	-copy ..\CodaCalFiles\EM $(DESTINATION)\CodaCalFiles
	-copy ..\CodaCalFiles\FM2 $(DESTINATION)\CodaCalFiles
	-copy ..\CodaCalFiles\GM $(DESTINATION)\CodaCalFiles
	-copy ..\CodaCalFiles\SM $(DESTINATION)\CodaCalFiles
	-copy ..\CodaCalFiles\TECNALIA $(DESTINATION)\CodaCalFiles
	-copy ..\DLLs\*.dll $(DESTINATION)
	-rmdir /S /Q $(ROOT)\InitFiles
	mkdir $(ROOT)\InitFiles
	-copy ..\InitFiles\*.* $(ROOT)\InitFiles

