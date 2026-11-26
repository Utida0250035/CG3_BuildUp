#Requires -Version 5.1
<#
  roadmap.md の未完了項目（- [ ]）がなくなるまで、指定間隔（分）で自動的に実行・進行状況チェックを行うループスクリプト。
  使用例:
    powershell -ExecutionPolicy Bypass -File EditorAssets/Tools/ClaudeAI/runRoadmapAutoLoop.ps1 -IntervalMinutes 15
#>
param(
    [int]$IntervalMinutes = 15,
    [int]$MaxLoops = 100
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$roadmapPath = Join-Path $PSScriptRoot "docs/feature-memos/roadmap.md"
if (-not (Test-Path $roadmapPath)) {
    $roadmapPath = "EditorAssets/Tools/ClaudeAI/docs/feature-memos/roadmap.md"
}

Write-Host "=== Roadmap Auto Loop Started ===" -ForegroundColor Cyan
Write-Host "Target Roadmap: $roadmapPath" -ForegroundColor Gray
Write-Host "Interval: $IntervalMinutes minutes" -ForegroundColor Gray

$loopCount = 0

while ($loopCount -lt $MaxLoops) {
    $loopCount++
    Write-Host "`n--- Loop Iteration $loopCount / $MaxLoops ($(Get-Date)) ---" -ForegroundColor Yellow

    if (-not (Test-Path $roadmapPath)) {
        Write-Error "Roadmap file not found at $roadmapPath"
        break
    }

    $content = Get-Content -Path $roadmapPath -Encoding UTF8
    $unchecked = $content | Where-Object { $_ -match '^\s*-\s*\[\s*\]' }

    if (-not $unchecked) {
        Write-Host "すべてのロードマップ項目が完了しました！ループを終了します。" -ForegroundColor Green
        break
    }

    Write-Host "残り未完了項目数: $($unchecked.Count)" -ForegroundColor Magenta
    Write-Host "次の項目:" -ForegroundColor Gray
    $unchecked | Select-Object -First 3 | ForEach-Object { Write-Host "  $_" -ForegroundColor Gray }

    Write-Host "次回のチェックまで $IntervalMinutes 分待機します..." -ForegroundColor Cyan
    Start-Sleep -Seconds ($IntervalMinutes * 60)
}

Write-Host "=== Roadmap Auto Loop Finished ===" -ForegroundColor Cyan


