@echo off
cd /d %~dp0

::start "" cmd /k "namingCheck\namingCheck.cmd"

::start "" cmd /k "ruleCheck\ruleCheck.cmd"

call npm.cmd run lint:spell

echo 60•bŒã‚ÉŽ©“®‚Å•Â‚¶‚Ü‚·...
timeout /t 60 /nobreak >nul
exit /b
