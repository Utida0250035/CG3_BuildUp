param(
    [int]$MaxIterations = 50,
    [int]$TimeoutSeconds = 1200,
    [int]$SleepSeconds = 900
)

$ErrorActionPreference = "Continue"

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

    # 見出し階層（#, ##, ### または [], 〇）を保持しつつ、行頭が "- [ ]" または "- [~]" の行だけを抽出
    $currentH1 = ""
    $currentH2 = ""
    $targetIndex = -1
    $targetText = ""
    $uncompletedCount = 0

    for ($i = 0; $i -lt $lines.Count; $i++) {
        $trimmed = $lines[$i].Trim()

        if ($trimmed -match "^(#{1,2}\s+.+|\[.+\])$") {
            $currentH1 = $trimmed
            $currentH2 = ""
        }
        elseif ($trimmed -match "^(#{3,4}\s+.+|[〇○].+)$") {
            $currentH2 = $trimmed
        }
        # 行頭が "- [ ]"（未着手）または "- [~]"（進行中）のタスク行のみを対象にする
        elseif ($trimmed -match "^-\s*\[[\s~]\]\s*(.+)$") {
            $uncompletedCount++
            if ($targetIndex -eq -1) {
                $targetIndex = $i
                $taskBody = $Matches[1].Trim()
                $targetText = "$currentH1 > $currentH2 > $taskBody"
            }
        }
    }

    if ($targetIndex -eq -1) {
        Write-Host "`nすべての未着手・進行中タスク（- [ ] / - [~]）が完了しました！" -ForegroundColor Green
        break
    }

    Write-Host "`n=== [$iter / $MaxIterations] 実行中（残り未完了: $uncompletedCount 件） ===" -ForegroundColor Cyan
    Write-Host "対象: $targetText" -ForegroundColor Yellow

    $prompt = @"
まず ./.cursor/rules 内のルールファイルをすべて直接読み込んで厳守してください。
その上で、以下のロードマップの1項目のみを実装してください（roadmap.md 自体の編集は不要です）。

【実装対象】
$targetText
"@

    $job = Start-Job -ScriptBlock {
        param($dir, $p)
        $ErrorActionPreference = "Continue"
        $env:CI = "true"
        Set-Location $dir
        gemini -y -p $p 2>&1
    } -ArgumentList $ProjectRoot, $prompt

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

    $outputText = ($output | ForEach-Object { $_.ToString() }) -join "`n"
    Write-Host $outputText

    # APIエラーやクォータ制限で失敗していた場合は、完了扱いにせず停止する
    if ($jobState -ne "Completed" -or $outputText -match "(429|RESOURCE_EXHAUSTED|Quota exceeded|API_KEY_INVALID)") {
        Write-Host "APIエラーまたは実行エラーのため停止しました。未完了マークは更新しません。" -ForegroundColor Red
        exit 1
    }

    # 該当行の "- [ ]" または "- [~]" を "- [x]" に書き換えて保存（インデント維持・BOMなしUTF-8）
    $lines[$targetIndex] = $lines[$targetIndex] -replace "^(\s*-\s*)\[[\s~]\]", "`$1[x]"
    [System.IO.File]::WriteAllLines((Resolve-Path -LiteralPath $RoadmapFile), $lines, (New-Object System.Text.UTF8Encoding($false)))

    # 変更があればコミット（pre-commitフックはバイパス）
    if (-not [string]::IsNullOrWhiteSpace((git status --porcelain))) {
        git add -A
        git commit --no-verify -m "auto: $targetText" | Out-Null
    }

    Start-Sleep -Seconds $SleepSeconds
}
