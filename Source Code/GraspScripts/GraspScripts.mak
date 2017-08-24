### Keep the GRASP scripts up to date.
### For the moment, all we do is copy all the eligible script files to the destination directory.
### What we should do is write a tool to walk the tree, starting from the subject file or files
### to copy only the files that will actually be used.

SCRIPT_DESTINATION=..\..\Scripts

_flight: _common SubjectsFlight.sbj
	-del /F /Q _training
	echo Installing scripts for FLIGHT.
	copy /Y /V $(SCRIPT_DESTINATION)\SubjectsFlight.sbj $(SCRIPT_DESTINATION)\Subjects.sbj
	echo %date% %time% > $@

_training: _common SubjectsTraining.sbj
	-del /F /Q _flight
	echo Installing scripts for TRAINING.
	copy /Y /V $(SCRIPT_DESTINATION)\SubjectsTraining.sbj $(SCRIPT_DESTINATION)\Subjects.sbj
	echo %date% %time% > $@

_common: *.sss *.pcl *.tsk
	-rmdir /S /Q $(SCRIPT_DESTINATION)
	mkdir $(SCRIPT_DESTINATION)
	copy /Y /V *.sss $(SCRIPT_DESTINATION)
	copy /Y /V *.pcl $(SCRIPT_DESTINATION)
	copy /Y /V *.tsk $(SCRIPT_DESTINATION)
	copy /Y /V *.sbj $(SCRIPT_DESTINATION)
	echo %date% %time% > $@