# BinaryConverter（エンジン外ツール）

アセット制作パイプラインで OBJ をランタイム向けバイナリ（`.amesh`）に変換する。ゲーム実行時には読み込まない（エディタ／ビルド前処理用）。

## 要件

- Python 3.8+

## 使い方

```bash
python3 obj_to_amesh.py input.obj output.amesh
```

- 対応: `v` / `vt` / `vn` / 三角形 `f`（`位置/uv/法線` 形式）
- 仕様: `EditorAssets/Tools/ClaudeAI/docs/feature-memos/atr_mesh_binary_format.md`

### バッチ変換

ディレクトリ内の `.obj` を一括変換する。

```bash
python3 batch_obj_to_amesh.py path/to/obj_folder --output-root path/to/out --recursive
```

Windows では `batch_obj_to_amesh.cmd` を同じ引数で呼べる。

- `--skip-up-to-date` … 出力が入力より新しければスキップ（制作パイプラインの差分ビルド用）

## 今後

- `ModelStorage` から `.amesh` を直接ロード
