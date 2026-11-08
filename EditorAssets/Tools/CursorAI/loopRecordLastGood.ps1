#Requires -Version 5.1
<#
  ビルド成功・develop 統合後に呼ぶ。loop_state.json の lastGoodCommit を更新し失敗カウンタを 0 にする。
#>
param(
    [string]$RoadmapTask = "",
    [string]$StatePath = ""
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

if ([string]::IsNullOrWhiteSpace($StatePath)) {
    $StatePath = Join-Path $PSScriptRoot 'loop_state.json'
}

$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot '..\..\..')).Path
Set-Location $repoRoot

$sha = (git rev-parse HEAD).Trim()

$payload = [ordered]@{
    lastGoodCommit           = $sha
    activeRoadmapTask        = $RoadmapTask
    consecutiveBuildFailures = 0
    lastFailureLogExcerpt    = ''
}
$payload | ConvertTo-Json -Depth 3 | Set-Content -LiteralPath $StatePath -Encoding UTF8
Write-Host "loop_state: lastGoodCommit=$sha"
