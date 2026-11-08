# 作業ブランチの命名

## 形式

```
{feature|fix|chore}/{TopicInEnglish}
```

- **feature/** … 機能追加・エディタ拡張
- **fix/** … バグ修正・挙動の切り戻し
- **chore/** … ツール、Cursor ルール、辞書のみ

`TopicInEnglish` は PascalCase または単語連結（例: `EditorGridDrawLine3d`, `SemiAutoDevelopByCursorAi`）。  
スラッシュは1つだけ（`feature/foo` のみ）。空白・日本語は使わない。

## タスクから名前を作る

**roadmap:** `- [ ]` の小見出し 1 行につき **1 ブランチ**（着手前に `startTaskBranchFromDevelop.ps1`）。  
**backlog:** 未完了行 1 件につき 1 ブランチ。

1. 今回着手する **小見出し 1 行** または `task_backlog.md` の先頭未完了行を読む
2. 行頭の `[fix]` / `[feature]` / `[chore]` があればそれをプレフィックスに使う（なければ内容から推定）
3. 内容を短い英語トピックに要約（既存ブランチ名と重ならないよう確認: `git branch -a`）
4. **同一ブランチで次の小見出しに進まない** — マージ後は削除し、次行用に新しい名前で切る（`finishTaskGit.ps1` の `NextBranchName` / `loopMergeTickToDevelop.ps1` の `-NextBranchName`）

## 例

| タスク（要約） | ブランチ名 |
|----------------|------------|
| グリッドを DrawLine3D に戻す | `fix/EditorGridDrawLine3d` |
| 半自律 Git フロー整備 | `chore/SemiAutoGitFlow` |
| エンティティエディタ | `feature/EntityEditor` |
