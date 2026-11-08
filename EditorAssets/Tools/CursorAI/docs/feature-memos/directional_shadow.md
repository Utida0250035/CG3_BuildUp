# シェード／シャドウ（DirectionalShadow）

## 用途

- **ランタイム:** 3D メッシュの地面付近の影とシェーディング強調
- **エンティティ:** `CmpMesh` が平面影ディスクを描画（`radius_` ベース）

## 実装

| 層 | 内容 |
|----|------|
| **シェード** | `Object3d.PS` — 地面近くで背面（`-N·L`）を暗くする接触シェード |
| **平面影** | `DirectionalShadowSettings::DrawPlanarGroundShadow` — 潰した球ディスク |
| **統合** | `Lighting` / `AtrumEngine`、`LightFlags::CAST_SHADOW` |

## パラメータ

- シェード: `shadeEnabled`, `shadeDarkness`, `groundPlaneY`, `contactHeight`
- 平面影: `planarEnabled`, `planarAlpha`, `planarSquash`, `planarRadiusScale`

## 今後

シャドウマップ SRV と `shadowMapIndex` による投影シャドウへ拡張可能。
