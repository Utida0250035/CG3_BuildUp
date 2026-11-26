@echo off
setlocal EnableExtensions
chcp 65001 >nul

set "REPO_ROOT=%~dp0..\..\.."
cd /d "%REPO_ROOT%\Project\Externals\LuaJIT"
if errorlevel 1 exit /b 1

if not exist "src" (
  echo LuaJIT src がありません。公式ソースを src に展開するか、git clone してください。
  echo 例: git clone https://github.com/LuaJIT/LuaJIT.git _fetch
  echo       xcopy /E /I /Y _fetch\src src
  exit /b 1
)

if not exist "lib" mkdir lib
if not exist "bin" mkdir bin

rem msvcbuild.bat は src をカレントにして実行し、成果物も src に出力する
rem （x64 の Native Tools コマンドプロンプトで実行すること。x86 だと 32bit 版になりリンクできない）
pushd src
call msvcbuild.bat
set "BUILD_ERR=%errorlevel%"
popd
if not "%BUILD_ERR%"=="0" exit /b 1

if exist "src\lua51.lib" copy /Y "src\lua51.lib" "lib\lua51.lib" >nul
if exist "src\lua51.dll" copy /Y "src\lua51.dll" "bin\lua51.dll" >nul

echo lua51.lib / lua51.dll を配置しました。
exit /b 0