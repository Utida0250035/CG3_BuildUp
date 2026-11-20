# クラウドビルド：vcxproj 穴チェック

## 用途

- **CI／リポジトリ:** 新規 `.cpp` が `CG2_BuildUp.vcxproj` 未登録のままマージされるのを防ぐ
- **エージェント開発:** Linux Cloud でも `verify_vcxproj_coverage.py` でリンク漏れを先に検出

## 実行

```bash
python3 EditorAssets/Tools/CursorAI/verify_vcxproj_coverage.py
python3 EditorAssets/Tools/CursorAI/verify_editor_component_registration.py
```

## GitHub Actions

- 独立: `.github/workflows/BuildCoverageCheck.yml`
- `Debug` / `Development` / `Release` ビルドの前段 `vcxproj-coverage` ジョブ

## ローカル Development ビルド

`EditorAssets/Tools/CursorAI/buildDevelopment.cmd` は **CI `DevelopmentBuild.yml` と同じ手順** を踏む。

1. `verify_vcxproj_coverage.py`（`vcxproj-coverage` ジョブ相当）
2. `restoreDevelopmentNuGetPackages.cmd`（ルート `packages.config` → `Project/packages`、SDL2 targets 確認）
3. `MSBuild Project/CG2_BuildUp.slnx`（`Configuration=Development` / `Platform=x64`）

CI の `DevelopmentBuild.yml` も同じ packages 復元後に slnx をビルドする。

## 許容リスト

`EditorAssets/Tools/CursorAI/vcxproj_coverage_allowlist.txt` — 意図的に vcxproj 外の `.cpp`（Lua スクリプト基盤など）
