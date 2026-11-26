# CG2_BuildUp（Atrum エンジン）— Claude Code 用ガイド

ルール・手順の正本は以下。**内容はここへ複製せず、正本を更新する**。

## 常時適用ルール

`.claude/rules/*.md` は Claude Code が毎セッション自動で読み込む（`paths` 指定なし＝常時適用）。

| ルール | 内容 |
|--------|------|
| `.claude/rules/japanese-language.md` | ユーザーとの会話は日本語で行う |
| `.claude/rules/ultimate-goal.md` | 着手前に最終目標との矛盾・有効性を査定 |
| `.claude/rules/game-dev-purpose.md` | ゲーム開発での用途を考慮 |
| `.claude/rules/game-logic-via-script.md` | ゲームロジックは Lua、C++ は基盤 |
| `.claude/rules/semi-autonomous-agent.md` | 半自律モード（停止条件・報告・ビルド） |
| `.claude/rules/roadmap-execute.md` | 「ロードマップを実行して。」の手順 |
| `.claude/rules/gap-scan.md` | 「ギャップスキャンを実行して」の手順 |
| `.claude/rules/loop-lease.md` | `/loop` tick の占有宣言（lease.md） |

## 参照ドキュメント（必要時に Read。大きいので自動読込しない）

| ファイル | 用途 |
|----------|------|
| `EditorAssets/Tools/ClaudeAI/claudeInstructions.md` | 行動指針の正本（§番号はここ。ルール中の「§6.7」等はこのファイルの節） |
| `EditorAssets/Tools/ClaudeAI/README.md` | 半自律ツール群の一覧 |
| `EditorAssets/Tools/ClaudeAI/branchNaming.md` | ブランチ命名 |
| `EditorAssets/Tools/ClaudeAI/task_backlog.md` | 人間が書く優先キュー |
| `EditorAssets/Tools/ClaudeAI/docs/ultimateGoal.md` | 最終目標（ユーザーのみ編集） |
| `EditorAssets/Tools/ClaudeAI/docs/feature-memos/roadmap.md` / `roadmap_rough.md` | ロードマップ正本 / 下書き |
| `EditorAssets/Tools/ClaudeAI/docs/feature-memos/*.md` | 機能ごとの設計メモ |
| `EditorAssets/Tools/ClaudeAI/progress_report.md` | 進捗報告の正本（**末尾追記のみ**。130kB 超なので必要な末尾だけ読む） |
| `EditorAssets/Tools/ClaudeAI/docs/implementation_gaps.md` | ギャップ一覧（約 300kB。全文を読まない） |
| `EditorAssets/Tools/ClaudeAI/docs/ideas/game_atomF.md` | ゲーム「atomF」企画 |

## 要点（上記ルールの抜粋）

- ユーザー向けの説明は日本語。名前空間は `Atrum`（またはその内側）。
- ゲーム用リソースは `Project/Assets/`、開発時専用は `EditorAssets/`。
- ゲームロジックは Lua（`Project/Assets/Scripts/`）、C++（`Project/Codes/`）はエンジン・エディタ・スクリプト基盤。
- 統合先は `develop`。`master` へのマージ・force push はしない。コミットはユーザーが明示したときのみ（pre-commit で cspell）。
- ビルド: `EditorAssets/Tools/ClaudeAI/buildDevelopment.cmd`（Development|x64）。ロードマップ実行時はビルドせず静的チェックのみ。

## ループと補足

- ルール中の `/loop` は Claude Code の `/loop`。tick 手順（lease の Begin/End、merge スクリプト）は `loop-lease.md`・`claudeInstructions.md` §7.1 に従う。`loopRoadmapDev.ps1` / `loopGapScanDev.ps1` は tick プロンプトを出力するだけで、`Control/claudeRoadmapLoop.ps1` は `claude -p` を 1 tick ずつ新規プロセスで呼ぶ。
- ファイルを追加・削除したら `Project/CG2_BuildUp.vcxproj` だけ更新すればよい。`vcxproj.filters` は `buildDevelopment*.cmd` 実行時と pre-commit フックで自動同期される（CI は `verify_vcxproj_filters.py` で一致を確認）。
