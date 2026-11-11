#Requires -Version 5.1
<#
  /loop 成功 tick 用: 作業ブランチにコミット → develop へマージ → push → 作業ブランチ削除 → 次タスク用ブランチ作成（§7.1.1）。
  -LoopId 指定時は、tick 中の他ループが lease で占有しているパスをコミット対象から外す（§7.1.5）。
#>
param(
    [Parameter(Mandatory = $true)]
    [string]$CommitMessage,
    [Parameter(Mandatory = $true)]
    [string]$NextBranchName,
    [string]$LoopId = ''
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

if ($NextBranchName -notmatch '^(feature|fix|chore)/[A-Za-z0-9][A-Za-z0-9._-]*$') {
    throw "NextBranchName は feature/ fix/ chore/ で始まる名前にしてください: $NextBranchName"
}

$workBranch = (git branch --show-current).Trim()
if ($workBranch -eq 'develop' -or $workBranch -eq 'master') {
    throw "作業ブランチ上で実行してください: $workBranch"
}

$status = (git status --porcelain)
if ([string]::IsNullOrWhiteSpace($status)) {
    throw 'コミット対象の変更がありません'
}

if ([string]::IsNullOrWhiteSpace($LoopId)) {
    Invoke-Git @('add', '-A')
}
else {
    . (Join-Path $PSScriptRoot 'loopLeaseCommon.ps1')
    $foreignPaths = Get-LeaseForeignInTickPaths -LoopId $LoopId
    $addArgs = @('add', '-A', '--', '.')
    foreach ($p in $foreignPaths) {
        $addArgs += ":(exclude)$($p.TrimEnd('/'))"
        Write-Host "他ループ tick 中の占有のためコミット対象外: $p"
    }
    Invoke-Git $addArgs
    & git diff --cached --quiet
    if ($LASTEXITCODE -eq 0) {
        throw 'コミット対象の変更がありません（変更は他ループ tick 中の占有パスのみ）'
    }
}
Invoke-Git @('commit', '-m', $CommitMessage)
Invoke-Git @('push', '-u', 'origin', $workBranch)

Invoke-Git @('fetch', 'origin')
Invoke-Git @('checkout', 'develop')
Invoke-Git @('pull', '--ff-only', 'origin', 'develop')
Invoke-Git @('merge', $workBranch, '--no-edit')
Invoke-Git @('push', 'origin', 'develop')

Invoke-Git @('branch', '-d', $workBranch)
$remoteExists = & git ls-remote --heads origin $workBranch 2>$null
if ($remoteExists) {
    Invoke-Git @('push', 'origin', '--delete', $workBranch)
}

Invoke-Git @('checkout', '-b', $NextBranchName)
Invoke-Git @('push', '-u', 'origin', $NextBranchName)

& powershell -NoProfile -ExecutionPolicy Bypass -File (Join-Path $PSScriptRoot 'loopRecordLastGood.ps1')
Write-Host "loop: $workBranch → develop（削除済み）/ 現在 $NextBranchName"
