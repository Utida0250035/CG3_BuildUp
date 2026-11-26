# デバッグ用テストプレイエリア生成

## テンプレート

- `EditorAssets/DebugPlayArea/template_test_play_area.scene.json`
- 例: `combat_bounds_sandbox.scene.json`（ステージ端マーカー）

## 生成

```bash
python3 scaffold_play_area.py MyFeatureTest
```

エディタ（Development）ではプロジェクトパネルの「テストエリア作成」でも同様に生成できます。

## 実行（Development・ランタイムプレビュー）

**C++ の `Launch` 直書きは不要です。** エディタから次のいずれかで BuildUp（本番同等）プレビューします。

1. プロジェクト一覧で `DebugPlayArea/MyFeatureTest.scene.json` を開く → **「シーンを実行」**（BuildUp）
2. プロジェクトパネル **「作成して BuildUp」** … 生成直後に BuildUp 起動
3. **「作成してエディタ内プレビュー」** … Runtime タブのみ（エディタ内）
4. 既定同梱シーン: **F2** = FeatureTest 正本、**F6** = combat_bounds_sandbox（エンティティエディタ）

手順の正本文言は `Project/Codes/Include/System/Editor/DebugPlayAreaRuntimePreviewGuidance.h`（`kStepsJa`）です。

## 用途

当たり・弾道・新コンポーネントの **本編を汚さない** サンドボックス。