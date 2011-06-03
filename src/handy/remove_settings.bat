@echo off

:: navigate to target dir 
%systemdrive%
cd %userprofile%
cd ..

::del dediprog folders
del /S/F/Q DediPreference.xml

cd "%allusersprofile%\Application Data\dediprog"
cd ..

::del dediprog folders
rd /s/q dediprog