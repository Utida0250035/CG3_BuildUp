param(
    [int]$MaxIterations = 50,
    [int]$TimeoutSeconds = 600,
    [int]$SleepSeconds = 15
)

$ErrorActionPreference = "Continue"

# プロジェクトルートへ移動
$ProjectRoot = (git -C $PSScriptRoot rev-parse --show-toplevel 2>$null)
if (-not $ProjectRoot) {
    Write-Error "プロジェクトルートが見つかりません。"
    exit 1
}
Set-Location $ProjectRoot

$RoadmapFile = "./EditorAssets/Tools/CursorAI/docs/feature-memos/roadmap.md"

for ($iter = 1; $iter -le $MaxIterations; $iter++) {
    if (-not (Test-Path -LiteralPath $RoadmapFile)) {
        Write-Error "roadmap.md が見つかりません: $RoadmapFile"
        exit 1
    }

    $lines = Get-Content -LiteralPath $RoadmapFile -Encoding UTF8

    # 行のどこかに「・」または「- [ ]」が含まれており、かつ「[x]」「[X]」が含まれていない行を未完了として抽出
    $uncompleted = @(
        $lines | Where-Object {
            ($_ -match "(・|-\s*\[\s*\])") -and ($_ -notmatch "\[[xX]\]")
        }
    )

    if ($uncompleted.Count -eq 0) {
        Write-Host "`nすべての小見出しが [x] になったため終了します。" -ForegroundColor Green
        break
    }

    Write-Host "`n=== [$iter / $MaxIterations] ロードマップ実行中（残り未完了: $($uncompleted.Count) 件） ===" -ForegroundColor Cyan
    Write-Host "次の対象候補: $($uncompleted[0].Trim())" -ForegroundColor DarkGray

    $job = Start-Job -ScriptBlock {
        param($dir)
        $ErrorActionPreference = "Continue"
        Set-Location $dir
        gemini -y -p ".cursor/rules を読み込んで厳守したうえで、ロードマップを実行して。" 2>&1
    } -ArgumentList $ProjectRoot

    $completed = Wait-Job $job -Timeout $TimeoutSeconds

    if (-not $completed) {
        Stop-Job $job
        Remove-Job $job
        Write-Host "タイムアウトのため停止しました。" -ForegroundColor Red
        exit 1
    }

    $output = Receive-Job $job
    $jobState = $job.State
    Remove-Job $job

    $output | ForEach-Object { $_.ToString() }

    if ($jobState -ne "Completed") {
        Write-Host "エラーが発生したため停止しました。" -ForegroundColor Red
        exit 1
    }

    # ファイル変更が発生していた場合のみ自動コミット
    if (-not [string]::IsNullOrWhiteSpace((git status --porcelain))) {
        git add -A
        git commit -m "auto: roadmap step $iter" | Out-Null
    }

    Start-Sleep -Seconds $SleepSeconds
}