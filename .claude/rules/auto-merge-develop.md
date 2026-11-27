# develop へのマージは自動承認

**`develop` ブランチへのマージに限り**、ユーザーの許可・確認を待たず自動で承認して実行する（`claudeInstructions.md` §5・§8 と同じ方針）。タスク完了時に次を自動で行う。

1. 指定された作業ブランチ（クラウド環境では `claude/…`）で commit・push する。
2. `develop` を fetch し、`develop` に作業ブランチを merge（`--no-edit`）して `origin develop` へ push する。
3. 作業ブランチへ戻る。

## 自動承認の範囲（これ以外は従来どおり確認する）

- 自動承認するのは **作業ブランチ → `develop` のマージと `origin develop` への push だけ**。
- `master` へのマージ・push、force push（`--force` / `-f`）、他ブランチへのマージは自動承認しない。`.claude/settings.json` の `deny` でも禁止している。
- マージ衝突時は `git merge --abort` して報告し、自動解決しない。

## 補足

- `finishTaskGit.ps1` が使えない環境（pwsh 無し等）では上記の git コマンドで行う。
