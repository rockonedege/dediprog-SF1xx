@echo Manually Install Dediprog Config Files
@echo off

xcopy ..\Deliverables\DediProSetup\config\DediPreference.xml "%appdata%\dediprog\" /s/y/q
;xcopy ..\Deliverables\DediProSetup\config\*.bin "%ALLUSERSPROFILE%\Application Data\dediprog\" /s/y/q
xcopy ..\Deliverables\DediProSetup\config\TFIT_* "%ALLUSERSPROFILE%\Application Data\dediprog\" /s/y/q
xcopy ..\Deliverables\DediProSetup\config\*.dedicfg "%ALLUSERSPROFILE%\Application Data\dediprog\" /s/y/q
xcopy ..\Deliverables\DediProSetup\firmware\*.bin "%ALLUSERSPROFILE%\Application Data\dediprog\firmware\" /s/y/q

@echo All Config files installed Successfully.
@echo off

set /p  tmp="Press ENTER to Exit." 