@echo off
setlocal
echo STARTING SCRIPT...
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

echo SUCCESS!

pause