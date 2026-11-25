@echo off
setlocal
cd /d "%~dp0..\..\.."
python "%~dp0verify_vcxproj_filters.py" %*
pause
exit /b %ERRORLEVEL%