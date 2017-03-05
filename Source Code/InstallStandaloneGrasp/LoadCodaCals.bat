REM ****************************************************************************
REM ***                                                                      ***
REM ***                            LoadCodaCals.bat                          ***
REM ***                                                                      ***
REM ****************************************************************************

REM A batch file to load the CODA calibration files onto DEX.
REM If the model of DEX that you are using does not have the calibration
REM files that correspond to the CODA units, then this batch file must be run
REM any time DEX is rebooted.

set HOST=10.80.12.103
set ID=speedy
set PASSWORD=dex

set CODACALFILEPATH=CodaMotion\Coda
WinSCP.exe /command "open ftp://%ID%:%PASSWORD%@%HOST%" "cd %CODACALFILEPATH%" "mput CodaCalFiles\*" "exit"