@echo off

chcp 65001 >nul

setlocal
echo STARTING SETUP...

set "TOOLS_DIR=%~dp0"

cd ..\..\..

goto NODE_CHECK


:END_SUCCESS

cd %TOOLS_DIR%

echo SUCCESS!

pause

exit /b 0


:WAIT

echo.
echo push key to go next...

pause


:EXIT

echo Exit.
pause
exit /b 1


:ERROR

echo Error.
pause
exit /b 1


:NODE_CHECK

echo [1/5] Node.js check...

call node -v >nul 2>&1

if %errorlevel% neq 0 (
    goto NODE_NOT_FOUND
)

echo Node.js found.

goto PYTHON_CHECK

:NODE_NOT_FOUND

echo.
echo node -v FAILED.
echo.
echo Node.js is not found or not in PATH.
ecoh.
echo Please Install.
echo.

winget --version >nul 2>&1
if %errorlevel% neq 0 (
    echo winget is not available.
    echo Please use option [2].
)

echo.
echo [1] Winget command[Auto]:
echo winget install OpenJS.NodeJS.LTS
echo.
echo [2] Official WebSite:
echo https://nodejs.org/ja
echo.
echo [0] Exit
echo.

call choice /c 120 /n /m "Select with key: "

if errorlevel 3 (
    goto EXIT
)
    
if errorlevel 2 (

   goto NODE_WEBSITE

)

if errorlevel 1 (

   goto NODE_WINGET

)

goto ERROR


:NODE_WINGET

echo NODE_WINGET

winget install OpenJS.NodeJS.LTS

goto PYTHON_CHECK


:NODE_WEBSITE

echo NODE_WEBSITE

start "" "https://nodejs.org/ja"

call :WAIT

goto PYTHON_CHECK


:PYTHON_CHECK

echo [2/5]Python check...

call py --version >nul 2>&1
if %errorlevel% neq 0 (
    call python --version >nul 2>&1
    if %errorlevel% neq 0 (
        goto PYTHON_NOT_FOUND
    )
)

echo Python found.
goto NPM_CHECK


:PYTHON_NOT_FOUND

echo py --version FAILED.
echo Python is not found or not in PATH.
echo.
echo Please install Python.
echo.
echo [1] Install with winget[Auto]:
echo     winget install --id Python.Python.3.12 -e
echo.
echo [2] Open official website:
echo     https://www.python.org/downloads/
echo.
echo [0] Exit
echo.

choice /c 120 /n /m "Select: "

if errorlevel 3 (
    goto EXIT
)

if errorlevel 2 (
    goto PYTHON_WEBSITE
)

if errorlevel 1 (
    goto PYTHON_WINGET
)

goto ERROR


:PYTHON_WINGET

winget install Python3

echo.
echo Please restart this setup after the installation.

goto NPM_CHECK


:PYTHON_WEBSITE

start "" "https://www.python.org/downloads/"

call :WAIT

goto NPM_CHECK


:NPM_CHECK

echo [3/5] npm check...

call npm -v >nul 2>&1

if %errorlevel% neq 0 (
    echo npm -v FAILED.
    echo npm is not found.
    pause
    exit /b 1
)

echo npm found.

goto NPM_INSTALL


:NPM_INSTALL

echo [4/5] Running npm install...

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

goto HUSKY_INIT


:HUSKY_INIT

echo [5/5] init Husky...
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
    echo npm.cmd run pre-commit
) > "%FILE%"

echo [INFO] created : ProjectDir\.husky\pre-commit

goto END_SUCCESS