@echo off
setlocal
cd /d "%~dp0..\..\.."
python "%~dp0sync_vcxproj_filters.py" %*
exit /b %ERRORLEVEL%