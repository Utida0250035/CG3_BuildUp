@echo off
cd /d %~dp0

start "FileAndDirNameCheck" powershell -NoProfile -ExecutionPolicy Bypass -Command "& { & '.\checkFileNames.exe' }"

exit