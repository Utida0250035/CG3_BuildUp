---
description: 半自律モード（ゲームエンジン開発の停止条件・報告・ビルド）
---

# 半自律エージェント

## いつ有効か

ユーザーが **半自律** / **半自律稼働** / **claudeInstructions に従って** / **ロードマップを実行して。** と明示したとき、以下を適用する（ロードマップ文言のタスク選定・1 行単位の詳細は `roadmap-execute.md`）。通常の質問や単発修正では過度に停止しない。

## 参照ドキュメント（作業開始時に Read）

- 行動指針: `EditorAssets/Tools/ClaudeAI/claudeInstructions.md`
- 優先タスク: `EditorAssets/Tools/ClaudeAI/task_backlog.md`（あれば最優先）
- 機能メモ: `EditorAssets/Tools/ClaudeAI/docs/feature-memos/roadmap_rough.md` → 同フォルダの `roadmap.md`（§6.2 即 `[x]`、§6.3 保留 `[-]` は他のみ着手、§6.4 用途不明は `[ ]` のまま放置して確認。`progress_report` 必須）
- 最終目標: `EditorAssets/Tools/ClaudeAI/docs/ultimateGoal.md`（着手前に §6.7 で矛盾と有効性を査定。本文は書き換えない）
- 直前の報告: `EditorAssets/Tools/ClaudeAI/progress_report.md`

## 必須動作

1. サブタスクを小さく切り、通常開発時は **ビルド**（`EditorAssets/Tools/ClaudeAI/buildDevelopment.cmd`）を試す。ただし **`roadmap-execute.md` に従うロードマップ実行時はビルド（全構成のビルド確認を含む）を行わず、作業ブランチ作成・ファイル編集・エラーになる矛盾の静的チェック（vcxproj 登録、宣言・実装・呼び出し元・マクロ分岐の照合）のみ**を行う。
2. マイルストーン完了または中断時は `EditorAssets/Tools/ClaudeAI/progress_report.md` を **追記更新**（上書き禁止。`claudeInstructions.md` §3・§7.1）。**人間のレビュー・指示待ち**で止める。§6.2 の即時 `[x]` も各セクションの「実装済みと判断した項目」に必ず記録する。`/loop` 中は 200kB 超過でループ自動停止し、末尾に停止理由を追記する。
3. **タスク（roadmap 小見出し 1 行）ごとに** `startTaskBranchFromDevelop.ps1` で **develop から** 作業ブランチを切る（§6.5）。長期使い回しブランチで複数タスクを続けない。  
   **タスク完了時はユーザーの許可を待たず自動で** コミット・push・develop マージ（pre-commit の cspell あり。`finishTaskGit.ps1`、§8）。**`/loop` tick 成功時**は `loopMergeTickToDevelop.ps1 -NextBranchName …` でマージ・**作業ブランチ削除**・**次ブランチ作成**（§7.1）。develop 統合は §5 のリスク例外以外確認不要。マージ衝突時は abort して報告。  
   **ループ中ビルド 3 回失敗:** ロールバック → `progress_report` にファイル・行番号付き失敗ログ → 依存のない別 roadmap タスク（§7.1.2）。
4. コード命名・名前空間は既存の Atrum 規約に合わせる。ユーザー向け説明は日本語。

## 停止の目安

`claudeInstructions.md` の停止条件（設計判断、同一ビルドエラー3回連続、コア破壊的変更）に該当したら即報告して待機。
