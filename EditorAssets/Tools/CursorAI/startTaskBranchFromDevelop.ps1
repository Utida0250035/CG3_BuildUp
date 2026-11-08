#Requires -Version 5.1
<#
  タスク着手前: develop を最新化し、そこから作業ブランチを新規作成して push する。
  用法は cursorInstructions.md §6.5 / §7.1.0 を参照。
#>
param(
    [Parameter(Mandatory = $true)]
    [string]$BranchName
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

function Invoke-Git {
    param([string[]]$GitArgs)
    & git @GitArgs
    if ($LASTEXITCODE -ne 0) {
        throw "git $($GitArgs -join ' ') failed ($LASTEXITCODE)"
    }
}

$repoRoot = (Invoke-Git @('rev-parse', '--show-toplevel')).Trim()
Set-Location $repoRoot

if ($BranchName -notmatch '^(feature|fix|chore)/[A-Za-z0-9][A-Za-z0-9._-]*$') {
    throw "BranchName は feature/ fix/ chore/ で始まる名前にしてください: $BranchName"
}

$status = (Invoke-Git @('status', '--porcelain')).Trim()
if (-not [string]::IsNullOrWhiteSpace($status)) {
    throw '未コミットの変更があります。先にコミット・stash・破棄してください。'
}

$current = (Invoke-Git @('branch', '--show-current')).Trim()
if ($current -eq $BranchName) {
    Write-Host "既に $BranchName 上です。"
    exit 0
}

$localExists = & git show-ref --verify --quiet "refs/heads/$BranchName"
if ($LASTEXITCODE -eq 0) {
    throw "ローカルに既に存在するブランチ名です: $BranchName"
}

Invoke-Git @('fetch', 'origin')
Invoke-Git @('checkout', 'develop')
Invoke-Git @('pull', '--ff-only', 'origin', 'develop')

Invoke-Git @('checkout', '-b', $BranchName)
Invoke-Git @('push', '-u', 'origin', $BranchName)

Write-Host "着手: develop から $BranchName を作成・push 済み"
