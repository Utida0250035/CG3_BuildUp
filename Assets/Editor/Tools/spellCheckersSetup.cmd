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

:: node が存在するか確認するだけの単純なコマンド
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

:: node_modules フォルダがあればインストールをスキップするロジック
if exist "node_modules" (
    echo [SKIP] node_modules\ is already exist.
) else (
    echo install node_modules...
    
    call npm install
    if %errorlevel% neq 0 (
        echo [ERROR] ライブラリのインストールに失敗しました。
        pause
        exit /b 1
    )
)

:: Huskyの初期化判定
for /f "tokens=*" %%i in ('git config core.hooksPath') do set HOOKS_PATH=%%i

if "%HOOKS_PATH%"==".husky" (
    echo [OK] Husky は正しく設定されています。
) else (
    echo [INFO] Husky を初期化します...
    call npx husky init
    echo [INFO] setup pre-commit...
    echo export PATH="./node_modules/.bin:$PATH" > .husky/pre-commit
    echo cmd.exe //c call "..\Assets\Editor\Tools\checkersRun.cmd" >> .husky/pre-commit

)

cd %TOOLS_DIR%

mkdir .\spellCheckers\inited

echo SUCCESS!

pause