#Requires -Version 5.1
<#
  /loop の占有宣言（Control/lease.md）を操作する。手順は cursorInstructions.md §7.1.5。

  Begin   … tick の最初。初回 tick は -Paths 必須。他ループと重なれば tick 中止扱いで progress_report に「占有衝突でスキップ」を追記
  Claim   … tick 途中で占有を追加する。重なれば Begin と同様に中止扱い
  End     … tick の最後。-Paths に次 tick で必要なパスを渡し、自セクションを置き換える（不要分は削除）。-Paths なしでセクション削除
  Release … 指定パス、または -Paths なしで自セクション全体を削除（ループ停止時）
  Test    … 重なり確認のみ（書き込みなし）
  Show    … lease.md の内容と失効状態を表示

  exit: 0 成功 / 2 引数エラー / 4 占有衝突

  例: powershell -NoProfile -ExecutionPolicy Bypass -File EditorAssets/Tools/CursorAI/loopLease.ps1 -Action Begin -LoopId roadmap-dev -Paths "Src/Engine/Foo.cpp,Src/Engine/Bar/" -Task "roadmap の行"
#>
param(
    [Parameter(Mandatory = $true)]
    [ValidateSet('Begin', 'Claim', 'End', 'Release', 'Test', 'Show')]
    [string]$Action,
    [string]$LoopId = '',
    [string[]]$Paths = @(),
    [string]$Task = '',
    [int]$IntervalMinutes = 90,
    [switch]$NoReport
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

. (Join-Path $PSScriptRoot 'loopLeaseCommon.ps1')

$exitConflict = 4
$exitUsage = 2

$repoRoot = Get-LeaseRepoRoot
$leasePath = Get-LeaseFilePath
$reportPath = Join-Path $PSScriptRoot 'progress_report.md'

if ($Action -ne 'Show' -and $LoopId -notmatch '^[A-Za-z0-9][A-Za-z0-9._-]*$') {
    Write-Host 'LoopId を英数字・. _ - で指定してください（例: roadmap-dev, gap-scan）'
    exit $exitUsage
}

$requested = ConvertTo-LeasePathList -Paths $Paths -RepoRoot $repoRoot
$shared = @($requested | Where-Object { Test-LeaseSharedPath $_ })
$requested = @($requested | Where-Object { -not (Test-LeaseSharedPath $_) })
foreach ($p in $shared) {
    Write-Host "共用ファイルのため占有対象外: $p"
}

function Get-Conflicts {
    param($Sections, [string[]]$Candidates)

    $conflicts = New-Object System.Collections.Generic.List[object]
    foreach ($s in $Sections) {
        if ($s.LoopId -eq $LoopId -or (Test-LeaseSectionExpired $s)) {
            continue
        }
        foreach ($c in $Candidates) {
            foreach ($held in $s.Paths) {
                if (Test-LeasePathOverlap $c $held) {
                    $conflicts.Add([PSCustomObject]@{ Path = $c; Held = $held; Owner = $s.LoopId; ExpiresAt = $s.ExpiresAt })
                }
            }
        }
    }
    return , $conflicts
}

function Merge-PathList {
    param([string[]]$Base, [string[]]$Extra)

    $list = New-Object System.Collections.Generic.List[string]
    foreach ($p in (@($Base) + @($Extra))) {
        if ([string]::IsNullOrWhiteSpace($p)) {
            continue
        }
        if (-not ($list | Where-Object { $_ -ieq $p })) {
            $list.Add($p)
        }
    }
    return , $list
}

function Set-SectionStamp {
    param($Section, [bool]$InTick)

    $now = [DateTimeOffset]::Now
    $Section.InTick = $InTick
    $Section.UpdatedAt = $now.ToString('o')
    $Section.ExpiresAt = $now.AddMinutes([math]::Max(60, $IntervalMinutes * 2)).ToString('o')
    if (-not [string]::IsNullOrWhiteSpace($Task)) {
        $Section.Task = $Task
    }
}

function Write-ConflictLines {
    param($Conflicts)

    foreach ($c in $Conflicts) {
        Write-Host "LEASE_CONFLICT $($c.Path) <-> $($c.Held) (占有: $($c.Owner), 期限 $($c.ExpiresAt))"
    }
}

function Add-SkipReport {
    param($Conflicts, [string]$Phase)

    if ($NoReport) {
        return
    }
    $stamp = (Get-Date).ToString('yyyy-MM-dd HH:mm')
    $taskText = $Task
    if ([string]::IsNullOrWhiteSpace($taskText)) {
        $taskText = '（未指定）'
    }
    $conflictLines = ($Conflicts | ForEach-Object { "- ``$($_.Path)`` ↔ ``$($_.Held)``（占有: $($_.Owner)、期限 $($_.ExpiresAt)）" }) -join "`n"
    $block = @"

---

## セッション $stamp（占有衝突でスキップ）

**ループ:** $LoopId
**状態:** 占有衝突でスキップ（${Phase}に tick を中止。占有外のファイルは変更していない）
**予定していたタスク:** $taskText

### 衝突したパス

$conflictLines

### 次の取り組み

- 占有が解放（または期限切れ）された後の tick で再試行する
- 衝突しない別タスクがあれば、tick の最後（``loopLease.ps1 -Action End``）でそちらを次 tick の占有として宣言する

"@
    Add-Content -LiteralPath $reportPath -Value $block -Encoding UTF8
}

switch ($Action) {
    'Show' {
        if (-not (Test-Path -LiteralPath $leasePath)) {
            Write-Host "lease なし: $leasePath"
            exit 0
        }
        foreach ($s in (Read-LeaseSections -LeasePath $leasePath)) {
            $state = 'active'
            if (Test-LeaseSectionExpired $s) {
                $state = 'expired'
            }
            Write-Host "[$($s.LoopId)] $state inTick=$($s.InTick) expiresAt=$($s.ExpiresAt) task=$($s.Task)"
            foreach ($p in $s.Paths) {
                Write-Host "  $p"
            }
        }
        exit 0
    }

    'Test' {
        $sections = Read-LeaseSections -LeasePath $leasePath
        $conflicts = Get-Conflicts -Sections $sections -Candidates $requested
        if ($conflicts.Count -gt 0) {
            Write-ConflictLines $conflicts
            exit $exitConflict
        }
        Write-Host 'LEASE_OK'
        exit 0
    }

    'Begin' {
        $result = Invoke-WithLeaseLock -LeasePath $leasePath -Body {
            $sections = Read-LeaseSections -LeasePath $leasePath
            $live = New-Object System.Collections.Generic.List[object]
            foreach ($s in $sections) {
                if (-not (Test-LeaseSectionExpired $s)) {
                    $live.Add($s)
                }
            }
            $own = @($live | Where-Object { $_.LoopId -eq $LoopId }) | Select-Object -First 1
            $ownPaths = @()
            if ($null -ne $own) {
                $ownPaths = @($own.Paths)
            }
            $candidates = Merge-PathList -Base $ownPaths -Extra $requested
            if ($candidates.Count -eq 0) {
                return [PSCustomObject]@{ Status = 'usage'; Conflicts = @() }
            }
            $conflicts = Get-Conflicts -Sections $live -Candidates $candidates
            if ($conflicts.Count -gt 0) {
                Write-LeaseSections -LeasePath $leasePath -Sections $live
                return [PSCustomObject]@{ Status = 'conflict'; Conflicts = $conflicts }
            }
            if ($null -eq $own) {
                $own = New-LeaseSection -LoopId $LoopId
                $live.Add($own)
            }
            $own.Paths = $candidates
            Set-SectionStamp -Section $own -InTick $true
            Write-LeaseSections -LeasePath $leasePath -Sections $live
            return [PSCustomObject]@{ Status = 'ok'; Conflicts = @() }
        }

        if ($result.Status -eq 'usage') {
            Write-Host "lease に $LoopId の占有がありません（期限切れを含む）。-Paths で今回 tick のパスを宣言してください"
            exit $exitUsage
        }
        if ($result.Status -eq 'conflict') {
            Write-ConflictLines $result.Conflicts
            Add-SkipReport -Conflicts $result.Conflicts -Phase 'tick 開始時'
            Write-Host 'LEASE_SKIP 占有衝突でスキップ'
            exit $exitConflict
        }
        Write-Host "LEASE_OK $LoopId"
        exit 0
    }

    'Claim' {
        if ($requested.Count -eq 0) {
            Write-Host 'Claim には -Paths が必要です'
            exit $exitUsage
        }
        $result = Invoke-WithLeaseLock -LeasePath $leasePath -Body {
            $sections = Read-LeaseSections -LeasePath $leasePath
            $live = New-Object System.Collections.Generic.List[object]
            foreach ($s in $sections) {
                if (-not (Test-LeaseSectionExpired $s)) {
                    $live.Add($s)
                }
            }
            $conflicts = Get-Conflicts -Sections $live -Candidates $requested
            if ($conflicts.Count -gt 0) {
                return [PSCustomObject]@{ Status = 'conflict'; Conflicts = $conflicts }
            }
            $own = @($live | Where-Object { $_.LoopId -eq $LoopId }) | Select-Object -First 1
            if ($null -eq $own) {
                $own = New-LeaseSection -LoopId $LoopId
                $live.Add($own)
            }
            $own.Paths = Merge-PathList -Base @($own.Paths) -Extra $requested
            Set-SectionStamp -Section $own -InTick $true
            Write-LeaseSections -LeasePath $leasePath -Sections $live
            return [PSCustomObject]@{ Status = 'ok'; Conflicts = @() }
        }

        if ($result.Status -eq 'conflict') {
            Write-ConflictLines $result.Conflicts
            Add-SkipReport -Conflicts $result.Conflicts -Phase 'tick 途中の占有追加時'
            Write-Host 'LEASE_SKIP 占有衝突でスキップ（今回 tick で占有パスに加えた変更は自分で元に戻すこと）'
            exit $exitConflict
        }
        Write-Host "LEASE_OK $LoopId"
        exit 0
    }

    'End' {
        $result = Invoke-WithLeaseLock -LeasePath $leasePath -Body {
            $sections = Read-LeaseSections -LeasePath $leasePath
            $live = New-Object System.Collections.Generic.List[object]
            $previousTask = ''
            foreach ($s in $sections) {
                if ($s.LoopId -eq $LoopId) {
                    $previousTask = $s.Task
                    continue
                }
                if (-not (Test-LeaseSectionExpired $s)) {
                    $live.Add($s)
                }
            }
            $conflicts = Get-Conflicts -Sections $live -Candidates $requested
            $kept = @($requested | Where-Object {
                    $candidate = $_
                    -not ($conflicts | Where-Object { $_.Path -ieq $candidate })
                })
            if ($kept.Count -gt 0) {
                $own = New-LeaseSection -LoopId $LoopId
                $own.Paths = Merge-PathList -Base @() -Extra $kept
                $own.Task = $previousTask
                Set-SectionStamp -Section $own -InTick $false
                $live.Add($own)
            }
            Write-LeaseSections -LeasePath $leasePath -Sections $live
            return [PSCustomObject]@{ Status = 'ok'; Conflicts = $conflicts; KeptCount = $kept.Count }
        }

        if ($result.Conflicts.Count -gt 0) {
            Write-ConflictLines $result.Conflicts
            Write-Host '上記パスは他ループが占有中のため次 tick 分として宣言していない（次 tick の Begin で衝突なら占有衝突でスキップ）'
            exit $exitConflict
        }
        if ($result.KeptCount -eq 0) {
            Write-Host "LEASE_RELEASED $LoopId（次 tick の占有なし）"
        }
        else {
            Write-Host "LEASE_OK $LoopId 次 tick 分 $($result.KeptCount) 件"
        }
        exit 0
    }

    'Release' {
        Invoke-WithLeaseLock -LeasePath $leasePath -Body {
            $sections = Read-LeaseSections -LeasePath $leasePath
            $live = New-Object System.Collections.Generic.List[object]
            foreach ($s in $sections) {
                if (Test-LeaseSectionExpired $s) {
                    continue
                }
                if ($s.LoopId -eq $LoopId) {
                    if ($requested.Count -eq 0) {
                        continue
                    }
                    $remaining = @($s.Paths | Where-Object {
                            $held = $_.ToLowerInvariant()
                            -not ($requested | Where-Object {
                                    $target = $_.ToLowerInvariant()
                                    $held -eq $target -or ($target.EndsWith('/') -and $held.StartsWith($target))
                                })
                        })
                    if ($remaining.Count -eq 0) {
                        continue
                    }
                    $s.Paths = Merge-PathList -Base @() -Extra $remaining
                }
                $live.Add($s)
            }
            Write-LeaseSections -LeasePath $leasePath -Sections $live
        }
        Write-Host "LEASE_RELEASED $LoopId"
        exit 0
    }
}
