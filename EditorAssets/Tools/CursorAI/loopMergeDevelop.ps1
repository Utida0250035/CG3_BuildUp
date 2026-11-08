#Requires -Version 5.1
<#
  /loop 成功タスク用（finishTaskGit と同じブランチライフサイクル）。
  推奨: loopMergeTickToDevelop.ps1 を使用する。
#>
param(
    [Parameter(Mandatory = $true)]
    [string]$CommitMessage,
    [Parameter(Mandatory = $true)]
    [string]$NextBranchName,
    [switch]$NoCommit
)

$ErrorActionPreference = 'Stop'

$finish = Join-Path $PSScriptRoot 'finishTaskGit.ps1'
if ($NoCommit) {
    & powershell -NoProfile -ExecutionPolicy Bypass -File $finish `
        -CommitMessage $CommitMessage -NextBranchName $NextBranchName -NoCommit
} else {
    & powershell -NoProfile -ExecutionPolicy Bypass -File $finish `
        -CommitMessage $CommitMessage -NextBranchName $NextBranchName
}
