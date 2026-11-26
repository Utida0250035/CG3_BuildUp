# デバッグ用テストプレイエリア

## 用途

- **デバッグ・プロファイリング:** 本編 `SceneGame` 外で Entity / コンポーネントを試す
- **制作:** テンプレからシーン JSON を量産し `SceneBuildUp` で即プレビュー

## 成果物

| パス | C++ 定数（`EditorAssetsPaths.h`） | 内容 |
|------|------|------|
| `EditorAssets/DebugPlayArea/` | `kDebugPlayAreaDirectory` | 走査ルート・scaffold 出力先 |
| `template_test_play_area.scene.json` | `kDebugPlayAreaTemplateScene` | 床マーカー + 回転プロップ |
| `FeatureTest.scene.json` | `kDebugPlayAreaFeatureTestScene` | 機能検証正本 |
| `combat_bounds_sandbox.scene.json` | `kDebugPlayAreaCombatBoundsSandboxScene` | `StageBounds` 境界マーカー |
| `Tools/DebugPlayAreaGenerator/scaffold_play_area.py` | — | 名前指定でテンプレ複製 |


## フロー

1. `scaffold_play_area.py FeatureX` で JSON 生成
2. エディタで編集 → プロジェクトパネル「シーンを実行」は `QueueFadeToBuildUp`（本番同等 BuildUp）。エディタ内だけ試すときは「エディタ内プレビュー」
3. 問題なければ本編シーンへ Entity を移植

## ホットキー（エンティティエディタ）

- **F2:** `FeatureTest.scene.json`（機能検証正本）を BuildUp で起動
- **F3:** パーティクルデモシーン BuildUp
- **F4:** パーティクルエディタへ遷移
- **F6:** `combat_bounds_sandbox.scene.json`（StageBounds 境界マーカー検証）を BuildUp で起動

## 推測したゲーム開発での用途

- プログラマ: 新 `Cmp*` の単体確認
- デザイナ: 弾幕パターン用ダミー配置（DEBUG タグ）
