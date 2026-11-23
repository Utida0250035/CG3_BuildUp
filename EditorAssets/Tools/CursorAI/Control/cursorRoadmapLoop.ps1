param(
    [int]$MaxIterations = 50,      # 最大ループ回数（暴走防止）
    [int]$TimeoutSeconds = 600,    # 1回あたりのタイムアウト（10分）
    [int]$SleepSeconds = 5         # 次の実行までの待機秒数
)

$ErrorActionPreference = "Stop"

# スクリプトの配置場所からプロジェクトルート（.git のある最上位階層）を自動検出して移動
$ProjectRoot = (git -C $PSScriptRoot rev-parse --show-toplevel 2>$null)
if (-not $ProjectRoot) {
    Write-Error "プロジェクトルートが見つかりません。"
    exit 1
}
Set-Location $ProjectRoot

for ($iter = 1; $iter -le $MaxIterations; $iter++) {
    Write-Host "`n=== [$iter / $MaxIterations] ロードマップ実行中 ===" -ForegroundColor Cyan

    # 毎回新規プロセスで実行（コンテキスト蓄積ゼロ）
    $job = Start-Job -ScriptBlock {
        param($dir)
        Set-Location $dir
        cursor-agent -p --force --model "gemini-4-hydrogen-high" "ロードマップを実行して。"
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

    Write-Host $output

    if ($jobState -ne "Completed") {
        Write-Host "エラーが発生したため停止しました。" -ForegroundColor Red
        exit 1
    }

    # リポジトリ内にファイル変更が発生しなくなったら全完了とみなして終了
    if ([string]::IsNullOrWhiteSpace((git status --porcelain))) {
        Write-Host "`n変更が発生しなくなったため終了します。" -ForegroundColor Green
        break
    }

    # 1回ごとに自動コミット
    git add -A
    git commit -m "auto: roadmap step $iter" | Out-Null

    Start-Sleep -Seconds $SleepSeconds
}
