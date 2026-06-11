@echo off
setlocal

echo [1/4] Node.js check...

:: node が存在するか確認するだけの単純なコマンド
call node -v >nul 2>&1
if %errorlevel% neq 0 (
    echo node -v FAILED.
    echo Node.js is not found or not in PATH.
    pause
    exit 0
)

echo Node.js found.
echo [2/4] npm check...

call npm -v >nul 2>&1
if %errorlevel% neq 0 (
    echo npm -v FAILED.
    echo npm is not found.
    pause
    exit 0
)

echo npm found.

echo [3/4] node_modules check...

if not exist "node_modules" (
    echo node_modules\ is not exist.
    pause
    exit 0
)

echo node_modules\ found.

echo [4/4]run checkers...

:: different process
start "" cmd /k "call npm run lint:spell"

:: different process
call ".\.checkers\run.cmd"

:: 
call npm run lint:spell

pause

exit %errorlevel%