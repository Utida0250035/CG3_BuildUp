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

## 診断（DEVELOPMENT + ImGui）

- メニュー「描画診断」→「ライティング効果」パネル内 **適用状態 / 診断**
  - シェード／平面影の ON/OFF と `LightFlags::CAST_SHADOW` の可視化
  - `DrawPlanarGroundShadow` の draw／skip 回数と直近 skip 理由
- **適用状態をログへ** … `LogFile` に `[DirectionalShadow]` スナップショットを追記（`DirectionalShadowApplyDebugLog`）

実装: `DirectionalShadowApplyDiagnostics`（統計）・`DrawPlanarGroundShadow` 各 return 経路で記録。

## 今後

シャドウマップ SRV と `shadowMapIndex` による投影シャドウへ拡張可能。
