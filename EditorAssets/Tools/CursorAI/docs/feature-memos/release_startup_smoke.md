# Release 起動スモーク

## 用途

- **CI:** `ReleaseBuild.yml` で MSBuild 成功後、exe が数フレーム起動できるか確認
- **配布:** Title／ゲームループ初期化までのリンク・Assets 同梱の退行検知

## 手順

1. `Release|x64` で `CG2_BuildUp.slnx` をビルド
2. `EditorAssets/Tools/CursorAI/runReleaseStartupSmoke.ps1` を実行

環境変数 `ATRUM_RELEASE_SMOKE_FRAMES`（既定 90）で `WinMain` が自動終了する。通常プレイでは未設定のまま。

## 成果物パス

`Generated/Outputs/Release/CG2_BuildUp.exe`（PostBuild で `Assets` / `EditorAssets` を同梱）