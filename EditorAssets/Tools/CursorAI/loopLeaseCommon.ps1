#Requires -Version 5.1
<#
  Control/lease.md（ループ占有宣言）の読み書き・重なり判定の共通処理。各スクリプトから dot-source する。
  lease.md はメインワークツリー側に 1 つだけ置き、git worktree を分けた別ループとも共有する。
#>

# どのループも書き込んでよい共用ファイル（追記・該当行のチェック更新など最小編集に限る）。占有対象にしない。
$script:LeaseSharedPaths = @(
    'EditorAssets/Tools/CursorAI/progress_report.md',
    'EditorAssets/Tools/CursorAI/task_backlog.md',
    'EditorAssets/Tools/CursorAI/docs/feature-memos/roadmap.md',
    'EditorAssets/Tools/CursorAI/Control/'
)

$script:LeaseRelativePath = 'EditorAssets/Tools/CursorAI/Control/lease.md'

function Get-LeaseRepoRoot {
    $top = (& git rev-parse --show-toplevel | Out-String).Trim()
    if ($LASTEXITCODE -ne 0 -or [string]::IsNullOrWhiteSpace($top)) {
        throw 'git リポジトリ内で実行してください'
    }
    return $top
}

function Get-LeaseFilePath {
    $top = Get-LeaseRepoRoot
    $common = (& git -C $top rev-parse --git-common-dir | Out-String).Trim()
    if (-not [System.IO.Path]::IsPathRooted($common)) {
        $common = Join-Path $top $common
    }
    $mainRoot = Split-Path -Parent ([System.IO.Path]::GetFullPath($common))
    return (Join-Path $mainRoot $script:LeaseRelativePath)
}

function ConvertTo-LeasePath {
    param([string]$Path, [string]$RepoRoot)

    $p = $Path.Trim().Trim('`').Replace('\', '/')
    if ([string]::IsNullOrWhiteSpace($p)) {
        return $null
    }
    if ($p -match '^[A-Za-z]:' -or $p.StartsWith('/')) {
        throw "lease のパスはリポジトリ相対で指定してください: $Path"
    }
    while ($p.StartsWith('./')) {
        $p = $p.Substring(2)
    }
    if ($p -match '(^|/)\.\.(/|$)') {
        throw "lease のパスに .. は使えません: $Path"
    }
    if ($p.EndsWith('/**') -or $p.EndsWith('/*')) {
        $p = $p.Substring(0, $p.LastIndexOf('/') + 1)
    }
    if ($p.Contains('*')) {
        throw "ワイルドカードはディレクトリ末尾の /** のみ対応: $Path"
    }
    if (-not $p.EndsWith('/') -and -not [string]::IsNullOrWhiteSpace($RepoRoot)) {
        if (Test-Path -LiteralPath (Join-Path $RepoRoot $p) -PathType Container) {
            $p = $p + '/'
        }
    }
    return $p
}

# PowerShell -File 経由では配列引数が 1 文字列になるため、カンマ／セミコロン区切りも受け付ける
function ConvertTo-LeasePathList {
    param([string[]]$Paths, [string]$RepoRoot)

    $result = New-Object System.Collections.Generic.List[string]
    foreach ($raw in $Paths) {
        if ($null -eq $raw) {
            continue
        }
        foreach ($piece in ($raw -split '[,;]')) {
            $p = ConvertTo-LeasePath -Path $piece -RepoRoot $RepoRoot
            if ($null -eq $p) {
                continue
            }
            if (-not ($result | Where-Object { $_ -ieq $p })) {
                $result.Add($p)
            }
        }
    }
    return , $result
}

function Test-LeasePathOverlap {
    param([string]$A, [string]$B)

    $a = $A.ToLowerInvariant()
    $b = $B.ToLowerInvariant()
    if ($a -eq $b) {
        return $true
    }
    if ($a.EndsWith('/') -and $b.StartsWith($a)) {
        return $true
    }
    if ($b.EndsWith('/') -and $a.StartsWith($b)) {
        return $true
    }
    return $false
}

function Test-LeaseSharedPath {
    param([string]$Path)

    $p = $Path.ToLowerInvariant()
    foreach ($shared in $script:LeaseSharedPaths) {
        $s = $shared.ToLowerInvariant()
        if ($p -eq $s -or ($s.EndsWith('/') -and $p.StartsWith($s))) {
            return $true
        }
    }
    return $false
}

function New-LeaseSection {
    param([string]$LoopId)

    return [PSCustomObject]@{
        LoopId    = $LoopId
        InTick    = $false
        UpdatedAt = ''
        ExpiresAt = ''
        Task      = ''
        Paths     = (New-Object System.Collections.Generic.List[string])
    }
}

function Test-LeaseSectionExpired {
    param($Section)

    $expires = [DateTimeOffset]::MinValue
    if (-not [DateTimeOffset]::TryParse($Section.ExpiresAt, [ref]$expires)) {
        # 手編集などで読めない期限は失効扱いにせず、占有を尊重する
        return $false
    }
    return $expires -lt [DateTimeOffset]::Now
}

function Read-LeaseSections {
    param([string]$LeasePath)

    $sections = New-Object System.Collections.Generic.List[object]
    if (-not (Test-Path -LiteralPath $LeasePath)) {
        return , $sections
    }

    $current = $null
    $inPaths = $false
    foreach ($line in (Get-Content -LiteralPath $LeasePath -Encoding UTF8)) {
        if ($line -match '^##\s+(\S+)\s*$') {
            $current = New-LeaseSection -LoopId $Matches[1]
            $sections.Add($current)
            $inPaths = $false
            continue
        }
        if ($null -eq $current) {
            continue
        }
        if ($line -match '^- paths:\s*$') {
            $inPaths = $true
            continue
        }
        if ($inPaths -and $line -match '^\s+-\s+`?([^`]+?)`?\s*$') {
            $current.Paths.Add($Matches[1].Trim())
            continue
        }
        if ($line -match '^- (\w+):\s*(.*)$') {
            $inPaths = $false
            $key = $Matches[1]
            $value = $Matches[2].Trim()
            switch ($key) {
                'inTick' { $current.InTick = ($value -eq 'true') }
                'updatedAt' { $current.UpdatedAt = $value }
                'expiresAt' { $current.ExpiresAt = $value }
                'task' { $current.Task = $value }
            }
        }
    }
    return , $sections
}

function Write-LeaseSections {
    param([string]$LeasePath, $Sections)

    $lines = New-Object System.Collections.Generic.List[string]
    $lines.Add('# lease（ループ占有宣言）')
    $lines.Add('')
    $lines.Add('`loopLease.ps1` が自動更新する。手で編集しない（手順: `cursorInstructions.md` §7.1.5）。')
    $lines.Add('各ループは自セクションの paths にのみ書き込む。共用ファイル（progress_report.md など）は全ループが追記してよい。')
    foreach ($s in $Sections) {
        $lines.Add('')
        $lines.Add("## $($s.LoopId)")
        $lines.Add('- inTick: ' + $s.InTick.ToString().ToLowerInvariant())
        $lines.Add("- updatedAt: $($s.UpdatedAt)")
        $lines.Add("- expiresAt: $($s.ExpiresAt)")
        $lines.Add('- task: ' + (($s.Task -replace '[\r\n]+', ' ').Trim()))
        $lines.Add('- paths:')
        foreach ($p in $s.Paths) {
            $lines.Add("  - ``$p``")
        }
    }

    $dir = Split-Path -Parent $LeasePath
    if (-not (Test-Path -LiteralPath $dir)) {
        New-Item -ItemType Directory -Path $dir -Force | Out-Null
    }
    $tmp = "$LeasePath.tmp"
    [System.IO.File]::WriteAllLines($tmp, $lines, (New-Object System.Text.UTF8Encoding($false)))
    Move-Item -LiteralPath $tmp -Destination $LeasePath -Force
}

# lease.md の読み書きを複数エージェント間で直列化する（Body の出力をそのまま返す）
function Invoke-WithLeaseLock {
    param([string]$LeasePath, [scriptblock]$Body)

    $dir = Split-Path -Parent $LeasePath
    if (-not (Test-Path -LiteralPath $dir)) {
        New-Item -ItemType Directory -Path $dir -Force | Out-Null
    }
    $lockPath = "$LeasePath.lock"
    $deadline = (Get-Date).AddSeconds(30)
    $stream = $null
    while ($null -eq $stream) {
        try {
            $stream = [System.IO.File]::Open($lockPath, [System.IO.FileMode]::CreateNew, [System.IO.FileAccess]::Write, [System.IO.FileShare]::None)
        }
        catch {
            if (-not (Test-Path -LiteralPath $lockPath)) {
                throw
            }
            # 異常終了で残ったロックは 2 分で破棄する
            $age = (Get-Date) - (Get-Item -LiteralPath $lockPath).LastWriteTime
            if ($age.TotalSeconds -gt 120) {
                Remove-Item -LiteralPath $lockPath -Force -ErrorAction SilentlyContinue
                continue
            }
            if ((Get-Date) -gt $deadline) {
                throw "lease のロックを取得できません: $lockPath"
            }
            Start-Sleep -Milliseconds 200
        }
    }
    try {
        & $Body
    }
    finally {
        $stream.Dispose()
        Remove-Item -LiteralPath $lockPath -Force -ErrorAction SilentlyContinue
    }
}

function Get-LeaseActiveSections {
    $sections = Read-LeaseSections -LeasePath (Get-LeaseFilePath)
    return , @($sections | Where-Object { -not (Test-LeaseSectionExpired $_) })
}

function Get-LeaseOwnPaths {
    param([string]$LoopId)

    $own = @((Get-LeaseActiveSections) | Where-Object { $_.LoopId -eq $LoopId })
    if ($own.Count -eq 0) {
        return , @()
    }
    return , @($own[0].Paths)
}

# tick 中の他ループが占有しているパス（コミット対象から外す）
function Get-LeaseForeignInTickPaths {
    param([string]$LoopId)

    $paths = New-Object System.Collections.Generic.List[string]
    foreach ($s in (Get-LeaseActiveSections)) {
        if ($s.LoopId -eq $LoopId -or -not $s.InTick) {
            continue
        }
        foreach ($p in $s.Paths) {
            $paths.Add($p)
        }
    }
    return , @($paths)
}

# 自ループの占有パスだけを指定コミットの状態へ戻す（他ループの未コミット変更は保持する）
function Invoke-LeaseScopedRestore {
    param([string]$Sha, [string[]]$Paths)

    $head = (& git rev-parse HEAD | Out-String).Trim()
    if ($head -ne $Sha) {
        & git reset --mixed -q $Sha
        if ($LASTEXITCODE -ne 0) {
            throw "git reset --mixed $Sha failed ($LASTEXITCODE)"
        }
    }

    foreach ($p in $Paths) {
        $spec = $p.TrimEnd('/')
        $known = (& git ls-tree -r --name-only $Sha -- $spec | Out-String).Trim()
        if (-not [string]::IsNullOrWhiteSpace($known)) {
            & git restore "--source=$Sha" --staged --worktree -- $spec
            if ($LASTEXITCODE -ne 0) {
                throw "git restore $spec failed ($LASTEXITCODE)"
            }
        }
        else {
            & git rm -r --cached -q --ignore-unmatch -- $spec
        }
        & git clean -fdq -- $spec
    }
}
