# ステージカメラ用イージング（カスタム）

**実装:** `Project/Codes/Include/Camera/StageViewCameraEasing.h`

汎用 `Math::Ease` の式をそのまま使わず、戦闘カメラ演出向けに曲線を分離した。

| 種別 | 関数 | 用途 |
|------|------|------|
| `PullBackReveal` | `StageViewCameraEasePullBackReveal` | `CameraPoseBlendController` 既定・ボスイントロ引き |
| `FocusPush` | `StageViewCameraEaseFocusPush` | `CameraFocusPushController` |
| `LegacyInOutQuad` | `LegacyInOutQuad` | 比較・巻き戻し用 |

## 設計意図

- **PullBackReveal:** 序盤はボスフォーカスを読ませ、終盤で戦闘画へ自然に戻す（最初 35% で移動 8% まで）。
- **FocusPush:** 指数的に寄せ、終端で急すぎない（撃破演出のプッシュイン）。

## プレビュー

```bash
python3 EditorAssets/Tools/StageViewCameraTuning/plot_easing.py
```

`tuning.json` の `easing` 節でコントローラと種別の対応を記録する。
