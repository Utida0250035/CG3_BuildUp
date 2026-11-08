#Requires -Version 5.1
<#
  直近のビルド成功時コミットへ作業ツリーを戻す。未記録時は git restore . と未追跡の整理に留める。
#>
param(
    [string]$StatePath = ""
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

if ([string]::IsNullOrWhiteSpace($StatePath)) {
    $StatePath = Join-Path $PSScriptRoot 'loop_state.json'
}

$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot '..\..\..')).Path
Set-Location $repoRoot

$sha = ''
if (Test-Path $StatePath) {
    $raw = Get-Content -LiteralPath $StatePath -Raw -Encoding UTF8
    if (-not [string]::IsNullOrWhiteSpace($raw)) {
        $state = $raw | ConvertFrom-Json
        if ($null -ne $state.lastGoodCommit) {
            $sha = [string]$state.lastGoodCommit
        }
    }
}

if (-not [string]::IsNullOrWhiteSpace($sha)) {
    git reset --hard $sha
    git clean -fd
    Write-Host "Rolled back to $sha"
    exit 0
}

git restore .
Write-Host "lastGoodCommit 未設定のため git restore . のみ実行"
exit 0
