### This makefile updates a dummy .c file each time it is built.
### If you want to force a project to rebuild every time, include
### ForceRebuild.c as part of your project and make your project
### dependent on this one.

### ForceRebuild.c contains only a benign comment
### so it will not affect your executable file, but because
### it is updated each time before your project is built, it
### will force your project to recompile as well.

### Typically, I then exclude the ForceRebuild project from the Debug
### build configuration, so that the dependent projects do not 
### necessarily get built each time a build is performed.

### I use this mechanism with GripGraspVersionControl. I want 
### GripGraspVersionControl to be rebuilt whenever I create a release
### so that the shared version info is the same across executables.
### But I don't care so much when I am in debug mode.

rebuild:
	echo // ForceRebuild %date% %time% > ForceRebuild.c