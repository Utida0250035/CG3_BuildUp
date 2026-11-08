# Cursor 半自律開発ツール

## 使い方

1. チャットで **「半自律で」** または **「半自律稼働」** と送る。
2. 任意で `task_backlog.md` にやりたいことを書く（なければ文脈から提案）。
3. エージェントはサブタスク → `buildDevelopment.cmd` → `progress_report.md` 更新 → **停止**。
4. レビュー後に「次へ」「A を実行」「半自律終了」などと返す。
5. サブタスク完了時に **コミット・プッシュ許可** を出すと、`finishTaskGit.ps1` 経由で develop マージ・旧ブランチ削除・次ブランチ作成まで行う（詳細は `cursorInstructions.md` §8）。

## ファイル

| ファイル | 役割 |
|----------|------|
| `cursorInstructions.md` | 停止条件・報告形式の詳細 |
| `progress_report.md` | 最新の進捗（エージェントが更新） |
| `task_backlog.md` | 人間用優先キュー |
| `buildDevelopment.cmd` | `Development \| x64` ビルド |
| `finishTaskGit.ps1` | コミット許可後の develop マージ・ブランチ切替 |
| `branchNaming.md` | 次作業ブランチの命名規則 |

## Cursor 連携

`.cursor/rules/semi-autonomous-agent.mdc` が半自律モードのトリガーと参照先を定義する。
