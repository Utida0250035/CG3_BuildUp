# デバッグ用テストプレイエリア生成

## テンプレート

- `EditorAssets/DebugPlayArea/template_test_play_area.scene.json`
- 例: `combat_bounds_sandbox.scene.json`（ステージ端マーカー）

## 生成

```bash
python3 scaffold_play_area.py MyFeatureTest
```

## 実行（Development）

1. エンティティエディタで JSON を開く、または
2. `SceneBuildUpRegistry::Launch("../EditorAssets/DebugPlayArea/MyFeatureTest.scene.json", returnScene)` でランタイムプレビュー

## 用途

当たり・弾道・新コンポーネントの **本編を汚さない** サンドボックス。
