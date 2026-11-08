# DdsConverter（エンジン外ツール）

PNG / JPEG などを **レガシー RGBA8 DDS**（`DXGI_FORMAT_R8G8B8A8_UNORM` 相当）へ変換する。ランタイムの `TextureStorage::LoadTexture` は現状 WIC 優先だが、配布アセットの事前 DDS 化・将来の DirectXTex `LoadFromDDSFile` 接続を想定する。

## 要件

```bash
pip install -r requirements.txt
```

## 使い方

```bash
python3 image_to_dds.py input.png output.dds
python3 image_to_dds.py photo.jpg output.dds --mipmaps 0
```

- 既定で簡易ミップチェーン（最長辺が 1 になるまで半分ずつ）を書き込む
- `--mipmaps 0` で単一ミップのみ

## Windows での代替

DirectXTex 付属 **texconv** を使う場合の例:

```bat
texconv -f R8G8B8A8_UNORM -m 0 -srgbi -o outdir texture.png
```

## 関連

- エンジン側読み込み: `TextureStorage.cpp`（WIC + `GenerateMipMaps`）
- Binary 系: `EditorAssets/Tools/BinaryConverter`
