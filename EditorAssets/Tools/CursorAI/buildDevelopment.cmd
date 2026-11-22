@echo off
setlocal
rem Development|x64 — CI DevelopmentBuild.yml と同じ前段検証 + slnx 入口

set "REPO_ROOT=%~dp0..\..\.."
cd /d "%REPO_ROOT%"
if errorlevel 1 (
  echo Failed to cd to repo root: %REPO_ROOT%
  exit /b 1
)

set "PREFLIGHT=EditorAssets\Tools\CursorAI\verify_vcxproj_coverage.py"
where python >nul 2>&1
if %errorlevel%==0 (
  python "%PREFLIGHT%"
) else (
  where py >nul 2>&1
  if %errorlevel%==0 (
    py -3 "%PREFLIGHT%"
  ) else (
    echo python not found for vcxproj pre-verify.
    exit /b 1
  )
)
if errorlevel 1 exit /b 1

call "%~dp0restoreDevelopmentNuGetPackages.cmd" "%REPO_ROOT%"
if errorlevel 1 exit /b 1

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

"%MSBUILD%" "%REPO_ROOT%\Project\CG2_BuildUp.slnx" /p:Configuration=Development /p:Platform=x64 /m /v:minimal
exit /b %ERRORLEVEL%