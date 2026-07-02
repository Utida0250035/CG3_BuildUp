@echo off
setlocal
echo STARTING SETUP...

set "TOOLS_DIR=%~dp0"

if exist "spellCheckers\inited" (

    echo spellCheckers are inited.
    
    pause

    exit /b 1

)

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
    exit .b 1
)

echo npm found.
echo [3/5] Running npm install...

if exist "node_modules" (
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


for /f "tokens=*" %%i in ('git config core.hooksPath') do set HOOKS_PATH=%%i

set "FILE=.\.husky\pre-commit"

if "%HOOKS_PATH%"==".husky" (
    echo [OK] Husky is ready
) else (
    echo [INFO] init Husky...
    call npx husky init
    echo [INFO] setup pre-commit...
    ::echo export PATH="./node_modules/.bin:$PATH" > .husky/pre-commit
    ::echo cmd.exe //c call "..\Assets\Editor\Tools\checkersRun.cmd" >> .husky/pre-commit

    type nul > "%FILE%"

    (
        echo #!/bin/sh
        echo export PATH="./node_modules/.bin:$PATH"
        echo cmd.exe /c call "..\Assets\Editor\Tools\checkersRun.cmd"
    ) > "%FILE%"

    :: 3. ここが重要：改行コードを強制的にLFにする
    powershell -NoProfile -Command "(Get-Content '%FILE%' -Raw) -replace \"`r`n\", \"`n\" | Set-Content -Path '%FILE%' -NoNewline -Encoding Ascii"

)

cd %TOOLS_DIR%

mkdir .\spellCheckers\inited

echo SUCCESS!

pause

exit

:: --- 内部関数 ---
:write_utf8
powershell -NoProfile -Command "$val = '%~2'; $val -replace \"`r`n\", \"`n\" | Add-Content -Path '%~1' -Encoding UTF8"
exit /b

:append_utf8
:: PowerShellのAdd-ContentはEncodingを指定できる
powershell -NoProfile -Command "Add-Content -Path '%~1' -Value '%~2' -Encoding UTF8"
exit /b