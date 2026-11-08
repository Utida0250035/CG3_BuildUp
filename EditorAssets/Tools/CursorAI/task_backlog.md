# 半自律用タスクバックログ

人間が優先度を書く。エージェントは **上から順に未完了** を1サブタスクずつ実行する。

## 書き方

```markdown
- [ ] [feature] （P0）短い説明 — 完了条件（例: Development ビルド成功、○○シーン手動確認）
- [ ] [fix] ...
- [x] （完了）...
```

行頭の `[feature]` / `[fix]` / `[chore]` は §8 の次ブランチ名（`branchNaming.md`）に使う。

下書き: **`roadmap_rough.md`**（`[大]` / `〇中` / `・小`）→ 正本: **`roadmap.md`**

- 優先行の例: `- [ ] roadmap: EditorAssets/Tools/CursorAI/docs/feature-memos/roadmap.md`（特定行はチャットで指定可）
- 補足 spec があるときだけ: `spec: EditorAssets/Tools/CursorAI/docs/feature-memos/○○.md`

## キュー

- [x] [chore] SemiAutoRulesInRepo — 半自律ルール・buildDevelopment.cmd・.cursor/rules をコミット
- [x] [chore] SemiAutoFlowVerify — finishTaskGit 実行結果を progress_report に記録（完了条件: develop へマージ済み）

**メモ:** キューが空のときは、エージェントが `progress_report.md` で次候補を提案し、指示待ちとする。
