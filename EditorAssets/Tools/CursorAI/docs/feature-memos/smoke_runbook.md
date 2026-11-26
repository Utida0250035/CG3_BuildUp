# Release 向け最小起動スモーク手順書

シナリオ **SHP-Smoke-001:** 配布候補ビルド（`Release|x64`）が起動し、Title 相当の初期シーンまでゲームループが進むことを人手または自動で確認する。

## 前提

- Windows x64、Visual Studio 2026 系（CI と同じ MSBuild 18.x 帯）
- リポジトリルートで NuGet 復元済み（`restoreDevelopmentNuGetPackages.cmd` の `Project/packages` 先は Release ビルドでも共用）
- 詳細構成: `ship_build_configurations.md`

## 1. Release ビルド

PowerShell（リポジトリルート）:

```powershell
nuget restore packages.config -PackagesDirectory Project/packages -NonInteractive
msbuild Project/CG2_BuildUp.slnx /p:Configuration=Release /p:Platform=x64 /m /v:minimal
```

成果物:

- exe: `Generated/Outputs/Release/CG2_BuildUp.exe`
- 同梱: PostBuild で `Assets/`・`EditorAssets/` が出力フォルダへコピーされる

## 2. 自動スモーク（推奨・CI 同等）

```powershell
EditorAssets/Tools/CursorAI/runReleaseStartupSmoke.ps1 -Configuration Release -SmokeFrames 90
```

- 起動引数 `--smoke-frames=90`（または環境変数 `ATRUM_RELEASE_SMOKE_FRAMES`）で約 90 フレーム後に正常終了（exit 0）
- 実装メモ: `release_startup_smoke.md`、`ReleaseStartupSmokeFrames.cpp`

## 3. 手動スモーク（タイトル表示の目視）

1. `Generated/Outputs/Release/` をカレントに `CG2_BuildUp.exe` を起動（`ATRUM_RELEASE_SMOKE_FRAMES` は**未設定**）
2. ウィンドウが開き、Release 初期シーン（Title）が表示されること
3. 数秒間クラッシュ・即終了が無いこと
4. 終了はウィンドウを閉じる

Development ビルドの機能検証は `DebugPlayArea` README（FeatureTest / F2）を参照。本手順書は **Release 配布候補専用**。

## 4. 失敗時の切り分け

| 症状 | 確認 |
|------|------|
| exe 無し | ビルドログ、`Generated/Outputs/Release/` |
| SDL / DLL エラー | `Project/packages` の復元、`sdl2.redist.targets` |
| 起動直後終了 | 出力先の `Assets` 有無 |
| スモークのみ失敗 | `runReleaseStartupSmoke.ps1` の exit code、フレーム数を増やして再試行 |

## 5. CI

`ReleaseBuild.yml` の **Release startup smoke** ステップが上記 ps1 を実行する。バッジはルート `README.md` を参照。