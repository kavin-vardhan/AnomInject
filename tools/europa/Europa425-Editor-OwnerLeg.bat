@echo off
setlocal
"C:\Python313\python.exe" -B "D:\IA_Europa\StylizedParisStreet\Plugins\AnomalyInjector\tools\europa\owner_leg.py" --config "D:\IA_Europa\Launch\Europa425-OwnerLeg.json" --owner-started %*
set "EUROPA_EXIT=%ERRORLEVEL%"
if not "%~1"=="--dry-run" pause
exit /b %EUROPA_EXIT%
