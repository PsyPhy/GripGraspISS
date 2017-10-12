### Rebuild the timestamp files only if they have changed since the last build.
### If a build is performed in Release mode, it will update CommonTimestamp.h, which
### will then be newer than the CommonTimestamp tag file that is the default target here.
### So building in release mode should force a new CommonTimestamp.h file to be written.
CommonTimestamp: CommonTimestamp.h DebugTimestamp.bat
	DebugTimestamp.bat
	echo CommonTimestamp %date% %time% > $@

CommonTimestamp.h: DebugTimestamp.bat
	DebugTimestamp.bat
