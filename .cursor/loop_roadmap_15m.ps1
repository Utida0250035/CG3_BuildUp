# UTF-8: roadmap /loop 15m wake script (調査・ループ用。プロジェクト本体とは分離)
$ErrorActionPreference = 'Stop'
[Console]::OutputEncoding = New-Object System.Text.UTF8Encoding $false
$OutputEncoding = [Console]::OutputEncoding
while ($true) {
  Start-Sleep -Seconds 900
  Write-Output 'AGENT_LOOP_TICK_roadmap {"prompt":"ロードマップを実行して。","loopId":"roadmap-dev"}'
}
