#Requires -Version 5.1
<#
  /loop 用: roadmap 開発 tick。各周期の前に progress_report サイズを確認し、200kB 超過で自動停止。
  例: powershell -NoProfile -ExecutionPolicy Bypass -File EditorAssets/Tools/CursorAI/loopRoadmapDev.ps1 -IntervalMinutes 90
#>
param(
    [int]$IntervalMinutes = 90,
    [string]$TickPrompt = 'roadmapの- [ ]小見出し1件ずつ。着手前にstartTaskBranchFromDevelop.ps1でdevelopから作業ブランチ作成。progress_reportは追記のみ。成功時はloopMergeTickToDevelop.ps1でdevelopマージ・作業ブランチ削除・-NextBranchNameで次ブランチ作成。ビルド3回失敗ならloopRegisterBuildFailure.ps1でロールバックし別- [ ]へ。'
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$guardScript = Join-Path $PSScriptRoot 'loopReportGuard.ps1'
$seconds = [math]::Max(60, $IntervalMinutes * 60)
$sentinel = 'AGENT_LOOP_TICK_roadmap-dev'

function Invoke-Guard {
    & powershell -NoProfile -ExecutionPolicy Bypass -File $guardScript -Quiet
    return $LASTEXITCODE
}

$guardExit = Invoke-Guard
if ($guardExit -eq 2) {
    & powershell -NoProfile -ExecutionPolicy Bypass -File $guardScript -AppendStopNoticeIfOversize -Quiet
    Write-Output "AGENT_LOOP_STOP_roadmap-dev oversize_report"
    exit 0
}

# JSON は tick 側でパースする想定
$escaped = $TickPrompt -replace '\\', '\\\\' -replace '"', '\"'
$jsonPrompt = "{`"prompt`":`"$escaped`"}"

while ($true) {
    Start-Sleep -Seconds $seconds

    $guardExit = Invoke-Guard
    if ($guardExit -eq 2) {
        & powershell -NoProfile -ExecutionPolicy Bypass -File $guardScript -AppendStopNoticeIfOversize -Quiet
        Write-Output "AGENT_LOOP_STOP_roadmap-dev oversize_report"
        break
    }

    Write-Output "$sentinel $jsonPrompt"
}
