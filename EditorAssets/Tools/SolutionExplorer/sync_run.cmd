@echo off
setlocal
call "%~dp0sync_vcxproj_filters.cmd" --write
pause
exit /b