###
### This makefile is designed to rebuild every time.
### The default target file CommonTimestampForceRebuild is never built,
### so the command is executed each time. This means that the 
### common timestamps created by CommonTimestamp.bat are update for 
### every build.
###
CommonTimestampForceRebuild: CommonTimestamp.bat
	CommonTimestamp.bat
