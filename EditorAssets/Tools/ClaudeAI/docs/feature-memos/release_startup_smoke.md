# Release 起動スモーク

人手向けの全体手順は **`smoke_runbook.md`**（ルート README からリンク）。

## 用途

- **CI:** `ReleaseBuild.yml` で MSBuild 成功後、exe が数フレーム起動できるか確認
- **配布:** Title／ゲームループ初期化までのリンク・Assets 同梱の退行検知

## 手順

1. `Release|x64` で `CG2_BuildUp.slnx` をビルド
2. `EditorAssets/Tools/ClaudeAI/runReleaseStartupSmoke.ps1` を実行

- **CLI（優先）:** `CG2_BuildUp.exe --smoke-frames=90` または `--smoke-frames 90`
- **環境変数:** `ATRUM_RELEASE_SMOKE_FRAMES`（CLI 未指定時のフォールバック）

通常プレイではどちらも指定しない。

## 成果物パス

`Generated/Outputs/Release/CG2_BuildUp.exe`（PostBuild で `Assets` / `EditorAssets` を同梱）