# デバッグ用テストプレイエリア

## 用途

- **デバッグ・プロファイリング:** 本編 `SceneGame` 外で Entity / コンポーネントを試す
- **制作:** テンプレからシーン JSON を量産し `SceneBuildUp` で即プレビュー

## 成果物

| パス | 内容 |
|------|------|
| `EditorAssets/DebugPlayArea/template_*.scene.json` | 床マーカー + 回転プロップ |
| `combat_bounds_sandbox.scene.json` | `StageBounds` 左右端の可視マーカー |
| `Tools/DebugPlayAreaGenerator/scaffold_play_area.py` | 名前指定でテンプレ複製 |

## フロー

1. `scaffold_play_area.py FeatureX` で JSON 生成
2. エディタで編集 → `SceneBuildUp` またはビルドアッププレビュー
3. 問題なければ本編シーンへ Entity を移植

## 推測したゲーム開発での用途

- プログラマ: 新 `Cmp*` の単体確認
- デザイナ: 弾幕パターン用ダミー配置（DEBUG タグ）
