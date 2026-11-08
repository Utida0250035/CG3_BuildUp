#Requires -Version 5.1
<#
  progress_report 等の蓄積サイズ監視。200kB 超過時に停止記録を追記し、ループ用 exit code を返す。
  リポジトリルートから実行すること。
#>
param(
    [switch]$AppendStopNoticeIfOversize,
    [switch]$Quiet
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot '..\..\..')).Path
if (-not (Test-Path (Join-Path $repoRoot 'Project\CG2_BuildUp.vcxproj'))) {
    $repoRoot = Get-Location
}

# 報告ファイル（蓄積対象）。正本以外の類似ファイルは作らない
$reportRelative = 'EditorAssets\Tools\CursorAI\progress_report.md'
$reportPath = Join-Path $repoRoot $reportRelative
$maxBytes = 200 * 1024

function Get-ReportByteLength {
    if (-not (Test-Path $reportPath)) {
        return 0
    }
    return (Get-Item -LiteralPath $reportPath).Length
}

function Add-LoopOversizeStopNotice {
    $bytes = Get-ReportByteLength
    $kb = [math]::Round($bytes / 1024.0, 1)
    $stamp = (Get-Date).ToString('yyyy-MM-dd HH:mm')
    $block = @"

---

## ループ自動停止 $stamp

**理由:** 自動記述ファイルの肥大化（``progress_report.md`` が 200kB を超過）
**サイズ:** ${kb} kB（上限 200 kB）
**対応:** ループを停止済み。本文は削除せず蓄積のまま全量を確認できる。必要なら要約・アーカイブ後に新規セクションから再開してください。

"@
    Add-Content -LiteralPath $reportPath -Value $block -Encoding UTF8
}

$length = Get-ReportByteLength
$oversize = $length -gt $maxBytes

if ($oversize -and $AppendStopNoticeIfOversize) {
    Add-LoopOversizeStopNotice
}

if (-not $Quiet) {
    if ($oversize) {
        Write-Host "Report oversize: $length bytes (max $maxBytes)"
    }
    else {
        Write-Host "Report size ok: $length bytes"
    }
}

if ($oversize) {
    exit 2
}

exit 0
