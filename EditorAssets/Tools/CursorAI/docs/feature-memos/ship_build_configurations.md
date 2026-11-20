# CG2_BuildUp ビルド構成契約

## 出荷候補（唯一）

| 構成 | 用途 | 主要マクロ |
|------|------|------------|
| **Release\|x64** | ゲーム配布・CI `ReleaseBuild` | `NDEBUG` のみ（`DEVELOPMENT` / `USE_IMGUI` / `_DEBUG` なし） |

## 開発用（非出荷）

| 構成 | 用途 | 主要マクロ |
|------|------|------------|
| **Development\|x64** | エディタ日常・`buildDevelopment.cmd`・`DevelopmentBuild` | `NDEBUG` + `DEVELOPMENT` + `USE_IMGUI` |
| **Debug\|x64** | MSVC Debug CRT・`DebugBuild`（最適化オフ・診断向け） | `_DEBUG` + `DEVELOPMENT` + `USE_IMGUI` + `MSVC_DEBUG_CONFIGURATION` |

## コード上の判定

- `Atrum::Build::IsShipCandidateBuild()` … Release のみ `true`
- `Atrum::Build::ConfigurationLabel()` … 起動時ログ用ラベル
- 未登録のマクロ組み合わせは `BuildConfigurationContract.cpp` でコンパイルエラー

## 本番相当との差分

Release では `DEVELOPMENT` / `USE_IMGUI` が無いため、エディタ UI・ForDebug 経路・多くの `#if defined(DEVELOPMENT)` ブロックが除外される。Development / Debug は同一機能フラグを共有するが、CRT（`MultiThreaded` vs `MultiThreadedDebug`）と `_DEBUG` の有無が異なる。