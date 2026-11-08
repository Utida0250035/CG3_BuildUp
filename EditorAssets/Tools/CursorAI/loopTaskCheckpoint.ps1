#Requires -Version 5.1
<#
  /loop タスク用チェックポイント。着手直前の HEAD を保存し、3 回ビルド失敗で hard reset。
  Begin はタスク用の編集開始「前」に必ず呼ぶこと（未コミットの当該タスク変更は reset で破棄）。
#>
param(
    [Parameter(Mandatory = $true)]
    [ValidateSet('Begin', 'Rollback', 'RecordBuildFailure', 'Clear', 'GetStatus')]
    [string]$Action,
    [string]$TaskLabel = ''
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$statePath = Join-Path $PSScriptRoot '.loop-task-state.json'

function Read-State {
    if (-not (Test-Path $statePath)) {
        return $null
    }
    return Get-Content -LiteralPath $statePath -Raw -Encoding UTF8 | ConvertFrom-Json
}

function Write-State($obj) {
    $obj | ConvertTo-Json -Depth 5 | Set-Content -LiteralPath $statePath -Encoding UTF8
}

function Invoke-Git {
    param([string[]]$Args)
    & git @Args
    if ($LASTEXITCODE -ne 0) {
        throw "git $($Args -join ' ') failed (exit $LASTEXITCODE)"
    }
}

$repoRoot = (Invoke-Git @('rev-parse', '--show-toplevel')).Trim()
Set-Location $repoRoot

switch ($Action) {
    'Begin' {
        $dirty = (Invoke-Git @('status', '--porcelain'))
        if (-not [string]::IsNullOrWhiteSpace($dirty)) {
            Write-Warning '作業ツリーに未コミット変更があります。Begin はタスク着手前（編集前）に呼んでください。'
        }

        $head = (Invoke-Git @('rev-parse', 'HEAD')).Trim()
        $state = [PSCustomObject]@{
            taskLabel      = $TaskLabel
            headSha        = $head
            buildFailCount = 0
            beganAt        = (Get-Date).ToString('o')
        }
        Write-State $state
        Write-Host "Checkpoint: HEAD=$head task=$TaskLabel"
        exit 0
    }

    'RecordBuildFailure' {
        $state = Read-State
        if ($null -eq $state) {
            Write-Host 'No checkpoint; fail count not tracked'
            exit 1
        }
        $state.buildFailCount = [int]$state.buildFailCount + 1
        Write-State $state
        Write-Host "Build failures for task: $($state.buildFailCount)"
        if ([int]$state.buildFailCount -ge 3) {
            exit 3
        }
        exit 0
    }

    'Rollback' {
        $state = Read-State
        if ($null -eq $state) {
            throw 'ロールバック用チェックポイントがありません'
        }

        Invoke-Git @('reset', '--hard', $state.headSha)
        Invoke-Git @('clean', '-fd')

        Write-Host "Rolled back to $($state.headSha)"
        Remove-Item -LiteralPath $statePath -ErrorAction SilentlyContinue
        exit 0
    }

    'Clear' {
        if (Test-Path $statePath) {
            Remove-Item -LiteralPath $statePath -Force
        }
        exit 0
    }

    'GetStatus' {
        $state = Read-State
        if ($null -eq $state) {
            exit 1
        }
        $state | ConvertTo-Json -Depth 5
        exit 0
    }
}
