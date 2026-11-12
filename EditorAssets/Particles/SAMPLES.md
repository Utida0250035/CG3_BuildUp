# パーティクル表現サンプル（能力カタログ）

**自動デモ（推奨）:** Development ビルドで **F3** → `SceneParticleDemo`。  
サンプル JSON を順番に再生し、先頭に **ボール×壁＋衝撃波** の演出デモがあります（Space で次項目、ImGui で手動切替可）。

手動プレビュー: パーティクルエディタ（F2 → `SceneParticleEditor`）の **JSON path** に下記パスを指定。  
基準ディレクトリは実行ファイルから見た `../EditorAssets/Particles/` です。

## いま JSON で表現できること（一覧）

| 軸 | 選択肢 | ざっくりした見え方 |
|----|--------|-------------------|
| 発射 | `BURST` / `CONTINUOUS` | 一瞬まとめて / レートで連続 |
| 連続の時間 | `duration`, `isLooping` | 一定時間だけ噴く／止まるまで噴き続け |
| 発生形状 | `POINT`, `SPHERE`, `AABB`, `CIRCLE_XZ` / `XY` / `YZ` | 点・球・箱・各平面の円盤 |
| 運動 | `DIRECTIONAL`, `RADIAL_SPREAD`, `RADIAL_CONVERGE` | 一定方向・中心から外へ・中心へ吸い込み |
| 座標 | `WORLD` / `LOCAL`（`simulationSpaceFromEmitter`） | ワールド固定／エミッターに追従 |
| 見た目 | `BUILTIN_SPHERE`, `BILLBOARD`, `MODEL` | 内置球・カメラ向き板・OBJ |
| 色・大きさ | `startColor`→`endColor`, `startScale`→`endScale` | 寿命で補間（HDRっぽい色も可） |
| 物理っぽさ | `acceleration`, `damping`, `velocityRandom` | 重力・空気抵抗・ばらつき |
| キル領域 | `killFieldParam`（`INSIDE`/`OUTSIDE` × 球/AABB） | 領域の内外で粒子を消す |
| 構成 | **JSON 配列 = 複数レイヤー** | 1ファイルで煙＋火花など重ね合わせ |

**できない／弱いところ（参考）:** 曲線パス、ノイズテクスチャアニメ、衝突、ライト連動、エディタからの一括ブラウズは未整備。ゲーム本編へ使うにはコード側で JSON パスを `EmitterManager` に渡す必要があります。

## サンプルファイル

プレビュー用パス例: `../EditorAssets/Particles/samples/<ファイル名>`

| ファイル | 主に見るパラメータ |
|----------|-------------------|
| `01_burst_directional_fountain.json` | `BURST` + `POINT` + `DIRECTIONAL` + 重力 |
| `02_burst_sphere_radial_spread.json` | `SPHERE` + `RADIAL_SPREAD`（爆発系） |
| `03_burst_radial_converge.json` | `RADIAL_CONVERGE`（吸い込み系） |
| `04_continuous_fountain.json` | `CONTINUOUS` + 短命パーティクル |
| `05_continuous_circle_xz_loop.json` | `CIRCLE_XZ` + `isLooping` 環状噴出 |
| `06_emit_aabb_smoke_box.json` | `AABB` 箱全体から発生 |
| `07_emit_circle_xy_disk.json` | `CIRCLE_XY` 水平円盤 |
| `08_emit_circle_yz_wall.json` | `CIRCLE_YZ` 縦円盤 |
| `09_render_billboard.json` | `BILLBOARD` + テクスチャ |
| `10_render_model.json` | `MODEL` + `unitSphere` |
| `11_killfield_inside_sphere.json` | 球**内**に入ると消える |
| `12_killfield_outside_aabb.json` | 箱**外**に出ると消える |
| `13_damping_float.json` | `damping` で減速しながら浮遊 |
| `14_multilayer_flash_smoke.json` | 2レイヤー（閃光＋煙） |
| `15_simulation_local_burst.json` | `LOCAL` 座標（エミッター移動時の挙動確認用） |

既存の戦闘向け参考（実行時パス）: `./Assets/Particles/parry_reflect.json` など。
