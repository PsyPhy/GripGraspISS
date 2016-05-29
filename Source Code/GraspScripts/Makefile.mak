### Keep the GRASP scripts up to date.
##1 For the moment, all we do is copy all the eligible script files to the destination directory.
### What we should do is write a tool to walk the tree, starting from the subject file or files
### to copy only the files that will actually be used.

install: *.sbj *.sss *.pcl *.tsk
	copy /Y /V *.sbj ..\..\Scripts
	copy /Y /V *.sss ..\..\Scripts
	copy /Y /V *.pcl ..\..\Scripts
	copy /Y /V *.tsk ..\..\Scripts
	echo %date% %time% > $@