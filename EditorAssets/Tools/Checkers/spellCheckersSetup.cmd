@echo off
chcp 65001 >nul

setlocal
echo STARTING SETUP...

set "TOOLS_DIR=%~dp0"

cd ..\..\..

echo [1/5] Node.js check...

call node -v >nul 2>&1
if %errorlevel% neq 0 (
    echo node -v FAILED.
    echo Node.js is not found or not in PATH.
    pause
    exit /b 1
)

echo Node.js found.
echo [2/5] npm check...

call npm -v >nul 2>&1
if %errorlevel% neq 0 (
    echo npm -v FAILED.
    echo npm is not found.
    pause
    exit /b 1
)

echo npm found.
echo [3/5] Running npm install...

if exist "node_modules\" (
    echo [SKIP] node_modules\ is already exist.
) else (
    echo install node_modules...
    
    call npm install
    if %errorlevel% neq 0 (
        echo [ERROR] install library failed
        pause
        exit /b 1
    )
)

echo [INFO] init Husky...
call npx husky
if %errorlevel% neq 0 (
    echo [ERROR] husky install failed
    pause
    exit /b 1
)


set "FILE=.\.husky\pre-commit"

echo [INFO] setup pre-commit...

(
    echo #!/usr/bin/env sh
    echo.
    echo cmd.exe /c ".\Assets\Editor\Tools\spellCheckersRun.cmd"
) > "%FILE%"

echo [INFO] created : ProjectDir\.husky\pre-commit

cd %TOOLS_DIR%

echo SUCCESS!

pause

exit