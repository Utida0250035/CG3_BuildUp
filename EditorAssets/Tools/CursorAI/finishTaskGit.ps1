# タスク完了時: コミット → push → develop へ merge → 作業ブランチ削除 → 次ブランチ作成
# 用法は cursorInstructions.md §8 を参照
param(
    [Parameter(Mandatory = $true)]
    [string]$CommitMessage,
    [Parameter(Mandatory = $true)]
    [string]$NextBranchName,
    [switch]$NoCommit
)

$ErrorActionPreference = "Stop"

function Invoke-Git {
    param([string[]]$Args)
    & git @Args
    if ($LASTEXITCODE -ne 0) {
        throw "git $($Args -join ' ') が失敗しました (exit $LASTEXITCODE)"
    }
}

$repoRoot = (Invoke-Git @("rev-parse", "--show-toplevel")).Trim()
Set-Location $repoRoot

$workBranch = (Invoke-Git @("branch", "--show-current")).Trim()
if ($workBranch -eq "develop" -or $workBranch -eq "master") {
    throw "作業ブランチ上で実行してください（現在: $workBranch）"
}

if ($NextBranchName -notmatch '^(feature|fix|chore)/[A-Za-z0-9][A-Za-z0-9._-]*$') {
    throw "NextBranchName は feature/ fix/ chore/ で始まる名前にしてください: $NextBranchName"
}

if (-not $NoCommit) {
    $status = (Invoke-Git @("status", "--porcelain"))
    if ([string]::IsNullOrWhiteSpace($status)) {
        throw "コミット対象の変更がありません"
    }
    Invoke-Git @("commit", "-m", $CommitMessage)
}

Invoke-Git @("push", "-u", "origin", $workBranch)

Invoke-Git @("fetch", "origin")
Invoke-Git @("checkout", "develop")
Invoke-Git @("pull", "--ff-only", "origin", "develop")

Invoke-Git @("merge", $workBranch, "--no-edit")
Invoke-Git @("push", "origin", "develop")

Invoke-Git @("branch", "-d", $workBranch)
$remoteExists = & git ls-remote --heads origin $workBranch 2>$null
if ($remoteExists) {
    Invoke-Git @("push", "origin", "--delete", $workBranch)
}

Invoke-Git @("checkout", "-b", $NextBranchName)
Invoke-Git @("push", "-u", "origin", $NextBranchName)

Write-Host "完了: develop にマージ済み / $workBranch 削除 / 現在 $NextBranchName"
