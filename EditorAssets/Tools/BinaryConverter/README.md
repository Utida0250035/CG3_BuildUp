# BinaryConverter（エンジン外ツール）

アセット制作パイプラインで OBJ をランタイム向けバイナリ（`.amesh`）に変換する。ゲーム実行時には読み込まない（エディタ／ビルド前処理用）。

## 要件

- Python 3.8+

## 使い方

```bash
python3 obj_to_amesh.py input.obj output.amesh
```

- 対応: `v` / `vt` / `vn` / 三角形 `f`（`位置/uv/法線` 形式）
- 仕様: `EditorAssets/Tools/CursorAI/docs/feature-memos/atr_mesh_binary_format.md`

## 今後

- `ModelStorage` から `.amesh` を直接ロード
- バッチ変換用の cmd / CI フック
