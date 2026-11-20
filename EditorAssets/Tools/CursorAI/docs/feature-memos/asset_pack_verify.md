# 同梱アセットとランタイム参照パスの整合

## 用途

- **配布:** `GameAssetsPaths` / `EditorAssetsPaths` が指すファイル・ディレクトリがリポジトリと PostBuild コピー元に存在するか PR 前に確認
- **開発:** cwd = exe（`Project` または `Generated/Outputs/*`）前提の `./Assets`・`../EditorAssets` 解決

## レイアウト

| ランタイム表記 | リポジトリ正本（ビルド前） | PostBuild 先（exe 隣） |
|----------------|----------------------------|-------------------------|
| `./Assets/...` | `Project/Assets/...` | `$(TargetDir)Assets/...` |
| `../EditorAssets/...` | `EditorAssets/...`（リポジトリルート） | `$(TargetDir)../EditorAssets/...` |

## 検査

```bash
python3 EditorAssets/Tools/CursorAI/verify_asset_pack_paths.py
```

- `GameAssetsPaths.h`・`EditorAssetsPaths.h`・`main.cpp` の文字列リテラルを収集
- `PlaySe` の `./Resources/...` は任意フォールバック（存在しなくても可）
- `CG2_BuildUp.vcxproj` の PostBuild `xcopy`（Assets / EditorAssets）を確認

## 手動（配布成果物）

Release ビルド後、`Generated/Outputs/Release/` で `Assets` と `../EditorAssets` に、上記パスが解決できることを確認する（`release_startup_smoke.md` と併用）。