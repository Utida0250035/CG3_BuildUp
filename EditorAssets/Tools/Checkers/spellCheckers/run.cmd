@echo off
cd /d %~dp0

call npm.cmd run lint:spell

echo 60seconds left
timeout /t 60 /nobreak >nul
exit /b