param(
    [int]$MaxIterations = 50,      # 最大ループ回数（暴走防止）
    [int]$TimeoutSeconds = 1200,    # 1回あたりのタイムアウト（20分）
    [int]$SleepSeconds = 900,       # 次の実行までの待機秒数
    [string]$RoadmapPath = "./EditorAssets/Tools/ClaudeAI/docs/feature-memos/roadmap.md",
    # claude -p の権限モード。無人で Bash（git・ps1）まで通すには .claude/settings.json の allow を整えるか、
    # 自己責任で bypassPermissions を指定する。
    [string]$PermissionMode = "acceptEdits",
    [string]$Model = ""                 # 空なら Claude Code の既定モデル
)

[Console]::InputEncoding = [System.Text.UTF8Encoding]::new($false)
[Console]::OutputEncoding = [System.Text.UTF8Encoding]::new($false)
$OutputEncoding = [System.Text.UTF8Encoding]::new($false)

$ErrorActionPreference = "Stop"

# スクリプトの配置場所からプロジェクトルート（.git のある最上位階層）を自動検出して移動
$ProjectRoot = (git -C $PSScriptRoot rev-parse --show-toplevel 2>$null)
if (-not $ProjectRoot) {
    Write-Error "プロジェクトルートが見つかりません。"
    exit 1
}
Set-Location $ProjectRoot

for ($iter = 1; $iter -le $MaxIterations; $iter++) {
    $rem = @(Select-String -Path $RoadmapPath -Pattern '^\s*[-*]\s*\[[ ~]\]').Count # 追加
    Write-Host "`n=== [$iter / $MaxIterations] ロードマップ実行中 (残り: $rem 件) ===" -ForegroundColor Cyan

    # 毎回新規プロセスで実行（コンテキスト蓄積ゼロ）
    $job = Start-Job -ScriptBlock {
        param($dir, $permissionMode, $model)
        [Console]::OutputEncoding = [System.Text.UTF8Encoding]::new($false)
        $OutputEncoding = [System.Text.UTF8Encoding]::new($false)
        Set-Location $dir
        $claudeArgs = @('-p', '--permission-mode', $permissionMode)
        if ($model) { $claudeArgs += @('--model', $model) }
        claude @claudeArgs "ロードマップを実行して。"
    } -ArgumentList $ProjectRoot, $PermissionMode, $Model

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

    $remAfter = @(Select-String -Path $RoadmapPath -Pattern '^\s*[-*]\s*\[[ ~]\]').Count # 追加
    Write-Host "[$iter / $MaxIterations] 実行後の残り件数: $remAfter 件" -ForegroundColor Yellow # 追加

    Start-Sleep -Seconds $SleepSeconds
}