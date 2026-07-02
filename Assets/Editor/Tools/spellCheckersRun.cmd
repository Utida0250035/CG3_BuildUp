@echo off

chcp 65001 >nul

setlocal

cd /d "%~dp0"

set "TOOLS_DIR=%~dp0"

cd ../../..

echo [1/4] Node.js check...


call node -v >nul 2>&1
if %errorlevel% neq 0 (
    echo node -v FAILED.
    echo Node.js is not found or not in PATH.
    exit 0
)

echo Node.js found.
echo [2/4] npm check...

call npm -v >nul 2>&1
if %errorlevel% neq 0 (
    echo npm -v FAILED.
    echo npm is not found.
    exit 0
)

echo npm found.

echo [3/4] node_modules check...

if not exist "node_modules" (
    echo node_modules\ is not exist.
    exit 0
)

echo node_modules\ found.

echo [4/4]run checkers...

:: different process
start "" "%TOOLS_DIR%checkers\run.cmd"

:: different process
start "" cmd /k "call npm run lint:spell"

:: spell check(mainProcess)
call npm run lint:spell >&2

exit /b %errorlevel%