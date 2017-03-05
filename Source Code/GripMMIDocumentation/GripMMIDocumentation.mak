### Keep the documentation for the GripMMI up to date.
### For the moment, all we do is copy all the eligible files to the destination directory.

DESTINATION=..\..\GripMMIDocumentation
install: *.docx *.pdf *.doc *.rtf
	-rmdir /S /Q $(DESTINATION)
	mkdir $(DESTINATION)
	-copy /Y /V *.docx $(DESTINATION)
	-copy /Y /V *.doc  $(DESTINATION)
	-copy /Y /V *.rtf  $(DESTINATION)
	-copy /Y /V *.pdf $(DESTINATION)
	echo %date% %time% > $@