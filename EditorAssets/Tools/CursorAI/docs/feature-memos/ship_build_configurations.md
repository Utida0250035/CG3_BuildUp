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

## Release リンク除外（vcxproj）

`CG2_BuildUp.vcxproj` の `ExcludedFromBuild` で **Release|x64** から外す TU:

- `System/Editor/` 配下のエディタ UI・ツール（allowlist 以外）
- `SceneEntityEditor*` / `SceneParticleEditor*`
- `CommandSceneDevelopmentBoot*`

**Release でもリンクする正本**（`editor_sources_release_link_allowlist.txt`）: シーン JSON 読込・BuildUp 用の `EditorSceneRuntime` など。

検証: `verify_editor_sources_release_exclude.py`（CI）。vcxproj 更新: `apply_editor_release_exclusions_to_vcxproj.py`。

## ForDebug（Release リンク）

`Codes/Source/ForDebug/` の TU は **Release|x64 から除外**（ImGui デバッグ UI・プロファイラ表示・DebugCamera 等）。例外は `for_debug_sources_release_link_allowlist.txt`（現状 `Log.cpp` のみ — シェーダ／リソース初期化が `LogFile` を参照）。

検証: `verify_for_debug_sources_release_exclude.py`（CI）。vcxproj 更新: `apply_for_debug_release_exclusions_to_vcxproj.py`。