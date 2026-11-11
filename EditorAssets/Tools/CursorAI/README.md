# Cursor 半自律開発ツール

## 使い方

1. チャットで **「半自律で」** / **「半自律稼働」** / **「ロードマップを実行して。」** のいずれかを送る（ロードマップ文言は `roadmap.md` 上から未完了 1 行を優先。詳細は `.cursor/rules/roadmap-execute.mdc`）。
2. 任意で `task_backlog.md` にやりたいことを書く（なければ文脈から提案）。
3. エージェントはサブタスク → `buildDevelopment.cmd` → `progress_report.md` 更新 → **停止**。
4. レビュー後に「次へ」「A を実行」「半自律終了」などと返す。
5. サブタスク完了時に **コミット・プッシュ許可** を出すと、`finishTaskGit.ps1` 経由で develop マージ・旧ブランチ削除・次ブランチ作成まで行う（詳細は `cursorInstructions.md` §8）。

## ファイル

| ファイル | 役割 |
|----------|------|
| `cursorInstructions.md` | 停止条件・報告形式の詳細 |
| `progress_report.md` | 最新の進捗（実装・起こし・**即時 [x] 判定**を記録。正本はここだけ） |
| `task_backlog.md` | 人間用優先キュー |
| `buildDevelopment.cmd` | `Development \| x64` ビルド |
| `startTaskBranchFromDevelop.ps1` | タスク着手前に develop から作業ブランチ作成 |
| `finishTaskGit.ps1` | コミット許可後の develop マージ・作業ブランチ削除・次ブランチ作成 |
| `branchNaming.md` | 次作業ブランチの命名規則（roadmap 小見出し 1 行＝1 ブランチ） |
| `docs/feature-memos/` | ロードマップ正本・下書き（`README.md` 参照） |
| `loopRoadmapDev.ps1` | `/loop` 用 tick（報告 200kB 超過で自動停止） |
| `loopGapScanDev.ps1` | `/loop` 用ギャップスキャン tick（キーワード「ギャップスキャンを実行して」） |
| `planGapScanTick.py` / `recordGapScanTick.py` / `syncImplementationGapsMd.py` | 1 tick の範囲選定・完了記録・md 同期 |
| `docs/implementation_gaps.md` 他 | 用途別実装不足の蓄積（§6.6） |
| `loopReportGuard.ps1` | `progress_report.md` サイズ監視・停止記録追記 |
| `loopMergeTickToDevelop.ps1` | ループ成功 tick: マージ・作業ブランチ削除・次ブランチ作成（`-NextBranchName` 必須） |
| `loopRegisterBuildFailure.ps1` | ビルド失敗記録（3 回でロールバック＋報告追記） |
| `loopRollbackToLastGood.ps1` | `lastGoodCommit` へ復帰 |
| `loop_state.json` | ループ用 Git 状態 |

### `/loop` と報告の蓄積

- ループ中は `progress_report.md` を **末尾追記のみ**（停止後に 1 ファイルで全セッション確認可能）。
- 起動例（90 分）: `powershell -NoProfile -ExecutionPolicy Bypass -File EditorAssets/Tools/CursorAI/loopRoadmapDev.ps1 -IntervalMinutes 90`
- `progress_report.md` が **200kB を超えるとループは自動停止**し、末尾に理由（肥大化）を記録する。
- **ビルド 3 回失敗**したサブタスクはロールバックし、ログの **ファイル名・行番号付き** で `progress_report` に中断理由を追記してから、依存のない別タスクへ。
- **tick 着手前**は `startTaskBranchFromDevelop.ps1` で **develop から** 作業ブランチを切る。
- **tick 成功時**は `loopMergeTickToDevelop.ps1` で **develop マージ・当該ブランチ削除・次ブランチ作成**（ユーザー確認不要）。

## Cursor 連携

`.cursor/rules/semi-autonomous-agent.mdc` が半自律モードのトリガーと参照先を定義する。
