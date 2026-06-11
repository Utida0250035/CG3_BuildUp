@echo off
setlocal
echo STARTING SETUP...

if exist ".checkers\.inited" (

    echo checkers are inited.
    
    pause

)

echo [1/5] Node.js check...

:: node が存在するか確認するだけの単純なコマンド
call node -v >nul 2>&1
if %errorlevel% neq 0 (
    echo node -v FAILED.
    echo Node.js is not found or not in PATH.
    pause
)

echo Node.js found.
echo [2/5] npm check...

call npm -v >nul 2>&1
if %errorlevel% neq 0 (
    echo npm -v FAILED.
    echo npm is not found.
    pause
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
    echo export PATH="./node_modules/.bin:$PATH"
    echo npm run lint:spell > .husky/pre-commit

)

mkdir .\.checkers\.inited

echo SUCCESS!

pause