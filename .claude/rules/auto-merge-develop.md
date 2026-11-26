# タスク完了時の develop 自動マージ

ユーザーの許可を待たず、タスク完了時に次を自動で行う（`claudeInstructions.md` §5・§8 と同じ方針）。

1. 指定された作業ブランチ（クラウド環境では `claude/…`）で commit・push する。
2. `develop` を fetch し、`develop` に作業ブランチを merge（`--no-edit`）して `origin develop` へ push する。
3. 作業ブランチへ戻る。

- `finishTaskGit.ps1` が使えない環境（pwsh 無し等）では上記の git コマンドで行う。
- `master` へのマージ・force push はしない。
- マージ衝突時は `git merge --abort` して報告し、自動解決しない。
