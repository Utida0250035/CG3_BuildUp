@echo off
setlocal
rem Debug x64 MSBuild (vswhere)

set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist "%VSWHERE%" (
  echo vswhere not found.
  exit /b 1
)

for /f "usebackq delims=" %%i in (`"%VSWHERE%" -latest -requires Microsoft.Component.MSBuild -find MSBuild\**\Bin\MSBuild.exe`) do set "MSBUILD=%%i"
if not defined MSBUILD (
  echo MSBuild not found.
  exit /b 1
)

set "REPO_ROOT=%~dp0..\..\.."
"%MSBUILD%" "%REPO_ROOT%\Project\CG2_BuildUp.vcxproj" /p:Configuration=Debug /p:Platform=x64 /m /v:minimal
exit /b %ERRORLEVEL%
