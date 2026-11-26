# FeatureTest（エディタ既定の機能検証シーン）

## 正本

- JSON: `EditorAssets/DebugPlayArea/FeatureTest.scene.json`
- コード: `EditorDebugPlayAreaFeatureVerificationCanonical::SceneJsonPath()` → `kDebugPlayAreaFeatureTestScene`
- 起動既定: `EditorAssets/launch_config.json` の `startupScenePath` / `defaultFeatureVerificationScenePath`

## 導線

- **F2**（EntityEditor）: BuildUp で FeatureTest 正本を起動
- **デバッグメニュー**: 「機能検証シーン (FeatureTest 正本)」
- **Development 起動**: 設定が空／無効なとき `CommandSceneDevelopmentBoot` が正本へフォールバック

## 関連

- `template_test_play_area.scene.json` … scaffold 専用（正本ではない）
- `LoopTick13Demo.scene.json` … 歴史的デモ用（正本ではない）

