### Keep the GRASP sequences up to date.
### For the moment, all we do is copy all the eligible files to the destination directory.
### What we should do is write a tool to walk the tree, starting from the subject file or files
### to copy only the files that will actually be used.

SCRIPT_DESTINATION=..\..\Sequences
install: *.seq
	-rmdir /S /Q $(SCRIPT_DESTINATION)
	mkdir $(SCRIPT_DESTINATION)
	copy /Y /V *.seq $(SCRIPT_DESTINATION)
	echo %date% %time% > $@