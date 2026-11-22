# 本番相当プロファイル計測（Development_opt）

シナリオ **DBG-Profiler-001:** Development で区間コストを見つつ、Release に近いスループットも切り分けて計測する。

## 構成の違い（正本）

| 手順 | ビルド | 最適化 | ImGui / プロファイラ | 用途 |
|------|--------|--------|----------------------|------|
| 日常エディタ | `buildDevelopment.cmd` → Development\|x64 | Disabled | あり | 編集・デバッグ |
| **本番相当＋区間計測** | **`buildDevelopmentProfile.cmd`** | **MinSpace**（Release 同系） | あり | DBG-Profiler-001 |
| 出荷候補そのもの | ReleaseBuild / `Release\|x64` | MinSpace | なし（ForDebug 除外） | 配布・CI スモーク |

マクロ契約の詳細: `ship_build_configurations.md`

## 手順 A — サブシステム別フレーム（区間の当たり）

1. `EditorAssets/Tools/CursorAI/buildDevelopmentProfile.cmd` でビルド（または通常 Development でも相対比較可）
2. `Generated/Outputs/Development/CG2_BuildUp.exe` を起動
3. メニュー「描画診断」→「サブシステム フレームプロファイラ」（`frame_profiler.md`）

## 手順 B — 出荷候補に極力近いスループット

1. `Release|x64` をビルド（CI `ReleaseBuild.yml` または `msbuild` と `restoreDevelopmentNuGetPackages.cmd`）
2. `smoke_runbook.md` の自動／手動スモーク
3. 区間 UI は使えない（Release は ForDebug 未リンク）。数値は外部プロファイラまたはログで補う

## 手順 C — 切り分けの目安

- **Update / Draw が重い（本番疑い）:** 手順 B の Release で再計測
- **ImGui / エディタが重い:** 手順 A のオーバーレイで ImGuiPrep / GfxSceneDraw を確認
- **最適化差の疑似再現:** 手順 A の `buildDevelopmentProfile.cmd`（`ClCompile_Optimization=MinSpace`）

## CI

- `verify_development_opt_profile.py` … 本ドキュメントと `buildDevelopmentProfile.cmd` の存在・キーワード