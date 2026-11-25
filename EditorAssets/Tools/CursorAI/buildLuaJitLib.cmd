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

call "src\msvcbuild.bat"
if errorlevel 1 exit /b 1

if exist "lua51.lib" move /Y "lua51.lib" "lib\lua51.lib"
if exist "lua51.dll" move /Y "lua51.dll" "bin\lua51.dll"

echo lua51.lib / lua51.dll を配置しました。
exit /b 0