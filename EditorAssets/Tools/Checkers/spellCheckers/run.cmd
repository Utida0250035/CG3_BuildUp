@echo off
cd /d %~dp0

start "" cmd /k "namingCheck\namingCheck.cmd"

start "" cmd /k "ruleCheck\ruleCheck.cmd"

exit