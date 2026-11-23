param(
    [int]$MaxIterations = 50,
    [int]$TimeoutSeconds = 600,
    [int]$SleepSeconds = 15
)

# 外部CLIの標準エラー出力(stderr)メッセージでスクリプトが強制終了しないようにする
$ErrorActionPreference = "Continue"

# プロジェクトルートへ移動
$ProjectRoot = (git -C $PSScriptRoot rev-parse --show-toplevel 2>$null)
if (-not $ProjectRoot) {
    Write-Error "プロジェクトルートが見つかりません。"
    exit 1
}
Set-Location $ProjectRoot

for ($iter = 1; $iter -le $MaxIterations; $iter++) {
    Write-Host "`n=== [$iter / $MaxIterations] ロードマップ実行中 ===" -ForegroundColor Cyan

    $job = Start-Job -ScriptBlock {
        param($dir)
        $ErrorActionPreference = "Continue"
        Set-Location $dir
        # 2>&1 で stderr の案内文を通常出力として受け取る
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

    # 出力を表示（ErrorRecord型が混ざっていても文字列として安全に出力）
    $output | ForEach-Object { $_.ToString() }

    if ($jobState -ne "Completed") {
        Write-Host "エラーが発生したため停止しました。" -ForegroundColor Red
        exit 1
    }

    # リポジトリ内にファイル変更が発生しなくなったら全完了とみなして終了
    if ([string]::IsNullOrWhiteSpace((git status --porcelain))) {
        Write-Host "`n変更が発生しなくなったため終了します。" -ForegroundColor Green
        break
    }

    git add -A
    git commit -m "auto: roadmap step $iter" | Out-Null

    Start-Sleep -Seconds $SleepSeconds
}
