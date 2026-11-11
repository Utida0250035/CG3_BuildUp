#Requires -Version 5.1
<#
  ビルド失敗 1 回分を記録。3 回目でロールバックし progress_report に追記する。
  -ErrorLines に MSBuild の error 行（ファイルパスと行番号を含むもの）を渡す。
  -LoopId 指定時のロールバックは lease の自ループ占有パスのみ（§7.1.5）。
#>
param(
    [Parameter(Mandatory = $true)]
    [string]$RoadmapTask,
    [string[]]$ErrorLines = @(),
    [switch]$RollbackNow,
    [string]$StatePath = "",
    [string]$ReportPath = "",
    [string]$LoopId = ""
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

if ([string]::IsNullOrWhiteSpace($StatePath)) {
    $StatePath = Join-Path $PSScriptRoot 'loop_state.json'
}
$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot '..\..\..')).Path
if ([string]::IsNullOrWhiteSpace($ReportPath)) {
    $ReportPath = Join-Path $PSScriptRoot 'progress_report.md'
}

$state = [ordered]@{
    lastGoodCommit = ''
    activeRoadmapTask = ''
    consecutiveBuildFailures = 0
    lastFailureLogExcerpt = ''
}

if (Test-Path $StatePath) {
    $existing = Get-Content -LiteralPath $StatePath -Raw -Encoding UTF8
    if (-not [string]::IsNullOrWhiteSpace($existing)) {
        $parsed = $existing | ConvertFrom-Json
        $state.lastGoodCommit = [string]$parsed.lastGoodCommit
        $state.activeRoadmapTask = [string]$parsed.activeRoadmapTask
        $state.consecutiveBuildFailures = [int]$parsed.consecutiveBuildFailures
    }
}

$state.activeRoadmapTask = $RoadmapTask
$state.consecutiveBuildFailures = [int]$state.consecutiveBuildFailures + 1

$excerpt = ($ErrorLines | Where-Object { $_ -match 'error ' }) -join "`n"
if ([string]::IsNullOrWhiteSpace($excerpt)) {
    $excerpt = ($ErrorLines -join "`n").Trim()
}
if ($excerpt.Length -gt 4000) {
    $excerpt = $excerpt.Substring(0, 4000) + "`n...(truncated)"
}
$state.lastFailureLogExcerpt = $excerpt

$shouldRollback = $RollbackNow -or ([int]$state.consecutiveBuildFailures -ge 3)

if ($shouldRollback) {
    $rollbackArgs = @('-NoProfile', '-ExecutionPolicy', 'Bypass', '-File', (Join-Path $PSScriptRoot 'loopRollbackToLastGood.ps1'), '-StatePath', $StatePath)
    $rollbackScope = ''
    if (-not [string]::IsNullOrWhiteSpace($LoopId)) {
        $rollbackArgs += @('-LoopId', $LoopId)
        $rollbackScope = "（lease の $LoopId 占有パスのみ）"
    }
    & powershell @rollbackArgs
    if ($LASTEXITCODE -ne 0) {
        $rollbackScope += "`n- **注意:** ロールバックが完了していない（exit $LASTEXITCODE）。作業ツリーを手動で確認すること"
    }
    $stamp = (Get-Date).ToString('yyyy-MM-dd HH:mm')
    $failCount = [int]$state.consecutiveBuildFailures
    $block = @"

---

## セッション $stamp（ループ・ビルド 3 回失敗でタスク中断）

**中断した roadmap タスク:** $RoadmapTask

### ビルド失敗による中断

- **連続失敗回数:** $failCount（上限 3 到達）
- **措置:** ``loopRollbackToLastGood.ps1`` で最後に成功していたコミット（``lastGoodCommit``）へロールバック$rollbackScope
- **推測される原因（ビルドログ抜粋・ファイル／行番号必須）:**

``````
$excerpt
``````

### 次の取り組み

- 失敗タスクに**依存しない**別の ``- [ ]`` を選んで継続（§6.3 の ``[-]`` はスキップ）
- 中断タスクの roadmap 行は ``[ ]`` のまま（``[x]`` にしない）

**状態:** ロールバック済み・別タスクへ切替

"@
    Add-Content -LiteralPath $ReportPath -Value $block -Encoding UTF8
    $state.consecutiveBuildFailures = 0
    $state.lastFailureLogExcerpt = ''
}

$state | ConvertTo-Json -Depth 3 | Set-Content -LiteralPath $StatePath -Encoding UTF8

if ($shouldRollback) {
    Write-Host 'BUILD_FAIL_ROLLBACK=1'
    exit 3
}

Write-Host "BUILD_FAIL_COUNT=$($state.consecutiveBuildFailures)"
exit 1
