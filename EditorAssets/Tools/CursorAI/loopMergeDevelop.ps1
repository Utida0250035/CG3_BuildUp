#Requires -Version 5.1
<#
  /loop 成功タスク用: 作業ブランチへ commit → push → develop へ merge → develop push → 作業ブランチに戻る
  （作業ブランチは削除しない）
#>
param(
    [Parameter(Mandatory = $true)]
    [string]$CommitMessage,
    [switch]$NoCommit
)

$ErrorActionPreference = 'Stop'

function Invoke-Git {
    param([string[]]$Args)
    & git @Args
    if ($LASTEXITCODE -ne 0) {
        throw "git $($Args -join ' ') failed (exit $LASTEXITCODE)"
    }
}

$repoRoot = (Invoke-Git @('rev-parse', '--show-toplevel')).Trim()
Set-Location $repoRoot

$workBranch = (Invoke-Git @('branch', '--show-current')).Trim()
if ($workBranch -eq 'develop' -or $workBranch -eq 'master') {
    throw "作業ブランチ上で実行してください（現在: $workBranch）"
}

if (-not $NoCommit) {
    $status = (Invoke-Git @('status', '--porcelain'))
    if ([string]::IsNullOrWhiteSpace($status)) {
        throw 'コミット対象の変更がありません'
    }
    Invoke-Git @('add', '-A')
    Invoke-Git @('commit', '-m', $CommitMessage)
}

Invoke-Git @('push', '-u', 'origin', $workBranch)

Invoke-Git @('fetch', 'origin')
Invoke-Git @('checkout', 'develop')
Invoke-Git @('pull', '--ff-only', 'origin', 'develop')
Invoke-Git @('merge', $workBranch, '--no-edit')
Invoke-Git @('push', 'origin', 'develop')
Invoke-Git @('checkout', $workBranch)

Write-Host "完了: $workBranch → develop にマージ・push（作業ブランチ維持）"
