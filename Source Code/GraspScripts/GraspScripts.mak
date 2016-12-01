### Keep the GRASP scripts up to date.
### For the moment, all we do is copy all the eligible script files to the destination directory.
### What we should do is write a tool to walk the tree, starting from the subject file or files
### to copy only the files that will actually be used.

SCRIPT_DESTINATION=..\..\GraspScripts
install: *.sbj *.sss *.pcl *.tsk
	-rmdir /S /Q $(SCRIPT_DESTINATION)
	mkdir $(SCRIPT_DESTINATION)
	copy /Y /V *.sbj $(SCRIPT_DESTINATION)
	copy /Y /V *.sss $(SCRIPT_DESTINATION)
	copy /Y /V *.pcl $(SCRIPT_DESTINATION)
	copy /Y /V *.tsk $(SCRIPT_DESTINATION)
	echo %date% %time% > $@