#Requires -Version 5.1
<#
  /loop 成功 tick 用: 作業ブランチにコミット → develop へマージ → develop を push（§7.1）。
  finishTaskGit.ps1 と違い作業ブランチは削除しない（連続ループ向け）。
#>
param(
    [Parameter(Mandatory = $true)]
    [string]$CommitMessage
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot '..\..\..')).Path
Set-Location $repoRoot

function Invoke-Git {
    param([string[]]$GitArgs)
    & git @GitArgs
    if ($LASTEXITCODE -ne 0) {
        throw "git $($GitArgs -join ' ') failed ($LASTEXITCODE)"
    }
}

$workBranch = (git branch --show-current).Trim()
if ($workBranch -eq 'develop' -or $workBranch -eq 'master') {
    throw "作業ブランチ上で実行してください: $workBranch"
}

$status = (git status --porcelain)
if ([string]::IsNullOrWhiteSpace($status)) {
    throw 'コミット対象の変更がありません'
}

Invoke-Git @('add', '-A')
Invoke-Git @('commit', '-m', $CommitMessage)
Invoke-Git @('push', '-u', 'origin', $workBranch)

Invoke-Git @('fetch', 'origin')
Invoke-Git @('checkout', 'develop')
Invoke-Git @('pull', '--ff-only', 'origin', 'develop')
Invoke-Git @('merge', $workBranch, '--no-edit')
Invoke-Git @('push', 'origin', 'develop')
Invoke-Git @('checkout', $workBranch)
Invoke-Git @('merge', 'develop', '--no-edit')

& powershell -NoProfile -ExecutionPolicy Bypass -File (Join-Path $PSScriptRoot 'loopRecordLastGood.ps1')
Write-Host 'loop: merged to develop and pushed'
