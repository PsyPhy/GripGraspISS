### Keep the GRASP scripts up to date.
### For the moment, all we do is copy all the eligible script files to the destination directory.
### What we should do is write a tool to walk the tree, starting from the subject file or files
### to copy only the files that will actually be used.

SCRIPT_DESTINATION=..\..\Scripts

_flight: _common SubjectsFlight.sbj
	-del /F /Q _training _bedrest
	echo Installing scripts for FLIGHT.
	copy /Y /V $(SCRIPT_DESTINATION)\SubjectsFlight.sbj $(SCRIPT_DESTINATION)\Subjects.sbj
	copy /Y /V $(SCRIPT_DESTINATION)\SubjectsFlight.sbj $(SCRIPT_DESTINATION)\Subjects.sbj
	copy /Y /V $(SCRIPT_DESTINATION)\V-KseatedFlight.pcl $(SCRIPT_DESTINATION)\V-Kseated.pcl
	copy /Y /V $(SCRIPT_DESTINATION)\V-VseatedFlight.pcl $(SCRIPT_DESTINATION)\V-Vseated.pcl
	copy /Y /V $(SCRIPT_DESTINATION)\K-KseatedFlight.pcl $(SCRIPT_DESTINATION)\K-Kseated.pcl
	copy /Y /V $(SCRIPT_DESTINATION)\V-KfloatingFlight.pcl $(SCRIPT_DESTINATION)\V-Kfloating.pcl
	copy /Y /V $(SCRIPT_DESTINATION)\V-VfloatingFlight.pcl $(SCRIPT_DESTINATION)\V-Vfloating.pcl
	copy /Y /V $(SCRIPT_DESTINATION)\K-KfloatingFlight.pcl $(SCRIPT_DESTINATION)\K-Kfloating.pcl
	echo %date% %time% > $@

_training: _common SubjectsTraining.sbj
	-del /F /Q _flight _bedrest
	echo Installing scripts for TRAINING.
	copy /Y /V $(SCRIPT_DESTINATION)\SubjectsTraining.sbj $(SCRIPT_DESTINATION)\Subjects.sbj
	echo %date% %time% > $@

_bedrest: _common SubjectsBedrest.sbj
	-del /F /Q _flight _training
	echo Installing scripts for BEDREST.
	copy /Y /V $(SCRIPT_DESTINATION)\SubjectsBedrest.sbj $(SCRIPT_DESTINATION)\Subjects.sbj
	copy /Y /V $(SCRIPT_DESTINATION)\V-KseatedBedrest.pcl $(SCRIPT_DESTINATION)\V-Kseated.pcl
	copy /Y /V $(SCRIPT_DESTINATION)\V-VseatedBedrest.pcl $(SCRIPT_DESTINATION)\V-Vseated.pcl
	copy /Y /V $(SCRIPT_DESTINATION)\K-KseatedBedrest.pcl $(SCRIPT_DESTINATION)\K-Kseated.pcl
	copy /Y /V $(SCRIPT_DESTINATION)\V-KfloatingBedrest.pcl $(SCRIPT_DESTINATION)\V-Kfloating.pcl
	copy /Y /V $(SCRIPT_DESTINATION)\V-VfloatingBedrest.pcl $(SCRIPT_DESTINATION)\V-Vfloating.pcl
	copy /Y /V $(SCRIPT_DESTINATION)\K-KfloatingBedrest.pcl $(SCRIPT_DESTINATION)\K-Kfloating.pcl
	echo %date% %time% > $@

_common: *.sss *.pcl *.tsk *.sbj
	echo Preparing to install scripts.
	-rmdir /S /Q $(SCRIPT_DESTINATION)
	mkdir $(SCRIPT_DESTINATION)
	copy /Y /V *.sss $(SCRIPT_DESTINATION)
	copy /Y /V *.pcl $(SCRIPT_DESTINATION)
	copy /Y /V *.tsk $(SCRIPT_DESTINATION)
	copy /Y /V *.sbj $(SCRIPT_DESTINATION)
	echo %date% %time% > $@