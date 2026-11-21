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

## 構成マトリクス（コンパイル定義 × Release 除外）

一括: `python3 EditorAssets/Tools/CursorAI/verify_config_matrix.py`

| 期待 | 検査 |
|------|------|
| Debug / Development / Release の `PreprocessorDefinitions` | `verify_ship_debug_compile_diff.py` |
| エディタ専用 TU の `Release\|x64` 除外 | `verify_editor_sources_release_exclude.py` |
| ForDebug TU の `Release\|x64` 除外（`Log.cpp` は allowlist） | `verify_for_debug_sources_release_exclude.py` |
| 出荷候補の PDB 無効 | `verify_release_link_generate_debug_info.py` |
| JsonTest デバッグパネル TU 除外 | `verify_json_test_release_exclude.py` |

正本: `ship_build_configurations.md`。CI の `BuildCoverageCheck` は `verify_config_matrix` で上記をまとめて実行する。

## Cloud Linux（Development 再現）

MSBuild は実行できない。**`runCloudDevelopmentPreflight.sh`** で Linux 上の妥当性チェックを `DevelopmentBuild` の前段と同等に揃える（詳細: `cloud_dev_build.md`）。
