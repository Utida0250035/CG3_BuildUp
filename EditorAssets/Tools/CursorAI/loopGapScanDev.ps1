#Requires -Version 5.1
<#
  /loop 用: 用途別ギャップスキャン tick（1 回 = シナリオ 1 件）。報告 200kB 超過で自動停止。
  各 tick のプロンプト先頭キーワード: ギャップスキャンを実行して

  例: powershell -NoProfile -ExecutionPolicy Bypass -File EditorAssets/Tools/CursorAI/loopGapScanDev.ps1 -IntervalMinutes 90
#>
param(
    [int]$IntervalMinutes = 90,
    [string]$TickPrompt = 'ギャップスキャンを実行して。cursorInstructions.md §6.6・§7.1.4。着手時 python3 EditorAssets/Tools/CursorAI/planGapScanTick.py を実行し gap_scan_tick.json の範囲のみ 1 tick スキャン。index と implementation_gaps.md を更新後 recordGapScanTick.py と syncImplementationGapsMd.py。ビルド・作業ブランチ不要。progress_report は追記のみ。'
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$guardScript = Join-Path $PSScriptRoot 'loopReportGuard.ps1'
$seconds = [math]::Max(60, $IntervalMinutes * 60)
$sentinel = 'AGENT_LOOP_TICK_gap-scan'

function Invoke-Guard {
    & powershell -NoProfile -ExecutionPolicy Bypass -File $guardScript -Quiet
    return $LASTEXITCODE
}

$guardExit = Invoke-Guard
if ($guardExit -eq 2) {
    & powershell -NoProfile -ExecutionPolicy Bypass -File $guardScript -AppendStopNoticeIfOversize -Quiet
    Write-Output "AGENT_LOOP_STOP_gap-scan oversize_report"
    exit 0
}

$escaped = $TickPrompt -replace '\\', '\\\\' -replace '"', '\"'
$jsonPrompt = "{`"prompt`":`"$escaped`"}"

while ($true) {
    Start-Sleep -Seconds $seconds

    $guardExit = Invoke-Guard
    if ($guardExit -eq 2) {
        & powershell -NoProfile -ExecutionPolicy Bypass -File $guardScript -AppendStopNoticeIfOversize -Quiet
        Write-Output "AGENT_LOOP_STOP_gap-scan oversize_report"
        break
    }

    Write-Output "$sentinel $jsonPrompt"
}
