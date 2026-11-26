# CG2_BuildUp（Atrum エンジン）— Claude Code 用ガイド

ルール・手順の正本は Cursor 用に書かれた以下のファイル。**内容はここへ複製せず、正本を更新する**（Cursor と Claude Code の両方が同じルールを読む）。

## 常時適用ルール（Cursor の `alwaysApply` ルールをそのまま読み込む）

@.cursor/rules/ultimate-goal.mdc
@.cursor/rules/game-dev-purpose.mdc
@.cursor/rules/game-logic-via-script.mdc
@.cursor/rules/semi-autonomous-agent.mdc
@.cursor/rules/roadmap-execute.mdc
@.cursor/rules/gap-scan.mdc
@.cursor/rules/loop-lease.mdc

## 参照ドキュメント（必要時に Read。大きいので自動読込しない）

| ファイル | 用途 |
|----------|------|
| `EditorAssets/Tools/CursorAI/cursorInstructions.md` | 行動指針の正本（§番号はここ。ルール中の「§6.7」等はこのファイルの節） |
| `EditorAssets/Tools/CursorAI/README.md` | 半自律ツール群の一覧 |
| `EditorAssets/Tools/CursorAI/branchNaming.md` | ブランチ命名 |
| `EditorAssets/Tools/CursorAI/task_backlog.md` | 人間が書く優先キュー |
| `EditorAssets/Tools/CursorAI/docs/ultimateGoal.md` | 最終目標（ユーザーのみ編集） |
| `EditorAssets/Tools/CursorAI/docs/feature-memos/roadmap.md` / `roadmap_rough.md` | ロードマップ正本 / 下書き |
| `EditorAssets/Tools/CursorAI/docs/feature-memos/*.md` | 機能ごとの設計メモ |
| `EditorAssets/Tools/CursorAI/progress_report.md` | 進捗報告の正本（**末尾追記のみ**。130kB 超なので必要な末尾だけ読む） |
| `EditorAssets/Tools/CursorAI/docs/implementation_gaps.md` | ギャップ一覧（約 300kB。全文を読まない） |
| `.cursor/ideas/game_atomF.md` | ゲーム「atomF」企画 |

## 要点（上記ルールの抜粋）

- ユーザー向けの説明は日本語。名前空間は `Atrum`（またはその内側）。
- ゲーム用リソースは `Project/Assets/`、開発時専用は `EditorAssets/`。
- ゲームロジックは Lua（`Project/Assets/Scripts/`）、C++（`Project/Codes/`）はエンジン・エディタ・スクリプト基盤。
- 統合先は `develop`。`master` へのマージ・force push はしない。コミットはユーザーが明示したときのみ（pre-commit で cspell）。
- ビルド: `EditorAssets/Tools/CursorAI/buildDevelopment.cmd`（Development|x64）。ロードマップ実行時はビルドせず静的チェックのみ。

## Claude Code で使うときの読み替え

- ルール中の `/loop` は Cursor 側のループ（`loopRoadmapDev.ps1` / `loopGapScanDev.ps1` / `Control/cursorRoadmapLoop.ps1` は `cursor-agent` を呼ぶ）を指す。Claude Code の `/loop` で回す場合も tick 手順（lease の Begin/End、merge スクリプト）は同じに従う。
- ファイルを追加・削除したら `Project/CG2_BuildUp.vcxproj` だけ更新すればよい。`vcxproj.filters` は `buildDevelopment*.cmd` 実行時と pre-commit フックで自動同期される（CI は `verify_vcxproj_filters.py` で一致を確認）。
