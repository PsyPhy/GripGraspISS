### This Makefile creates some documentation automatically.
### It uses the GraspVR package to create images that are up to date.

DOCUMENTATION_DESTINATION=..\..\Documentation
EXECUTABLES=..\..\Executables
INSTRUCTIONS=..\..\Instructions
MESSAGES=..\GraspBMPs

install: GraspScreens.pdf
	echo %date% %time% > $@


# Define the path to the pandoc.exe program that does the conversion.
PANDOC=pandoc.exe
PANDOC_OPTIONS=

# Define the preprocessor.
PREPROCESSOR=cl.exe
# The preprocessor that we are using is actually the Visual C++ compiler.
# The /EP option tells the compiler to preprocess only. 
# The /FI forces the inclusion of the preprocessor macros that we have defined. 
PREPROCESSOR_OPTIONS=/EP /nologo /FI macros.h

# The idea here is to create a single document to show all the instruction screens.
GraspInstructionScreens.pdf: $(INSTRUCTIONS)\*.html
	$(EXECUTABLES)\wkhtmltopdf.exe  --page-size A6  --default-header --header-left "Apendix I: Grasp Instruction Screens" --header-font-size 8 --header-spacing 5 --margin-bottom 0 $(**) $@


GraspScreens.pdf: GraspInstructionScreens.pdf $(MESSAGES)\GraspVRMessages.pdf
	 $(EXECUTABLES)\pdftk.exe $(**) cat output $@