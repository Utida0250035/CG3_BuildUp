# Atrum メッシュバイナリ（`.amesh`）v1

`VertexData`（`Project/Codes/Include/Engine/Resource/VertexData.h`）と同じレイアウトの頂点列をファイルに格納する。ランタイム読み込みは別タスク（`ModelStorage` 拡張）。

## ファイル構造（リトルエンディアン）

| オフセット | 型 | 内容 |
|-----------|-----|------|
| 0 | `char[4]` | マジック `AMSH` |
| 4 | `uint32` | バージョン（現状 `1`） |
| 8 | `uint32` | 頂点数 `N` |
| 12 | `N × 36` バイト | 頂点配列（下記） |

1 頂点 = 36 バイト（`sizeof(VertexData)` に相当）:

- `float` position.x, position.y, position.z, position.w（w は常に `1.0`）
- `float` texCoord.u, texCoord.v
- `float` normal.x, normal.y, normal.z

## OBJ 変換時の座標

`ModelStorage::LoadObjFile` と揃える:

- 位置・法線の **Z を反転**
- UV の **V を `1 - v`**
- 三角形の頂点順は **2, 1, 0**（OBJ の `f` から展開した順を反転）

## ツール

`EditorAssets/Tools/BinaryConverter/obj_to_amesh.py` … Wavefront OBJ（三角形 `f` のみ）→ `.amesh`
