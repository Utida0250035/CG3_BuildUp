# Cloud Linux 上の Development ビルド再現

## 制約

`CG2_BuildUp` は **Windows x64 + MSBuild** の Win32 プロジェクトです。Cloud Agent の **Linux VM ではリンクまでのビルドは実行できません**（`buildDevelopment.cmd` は Windows 専用）。

## Linux での再現手段（SHP-DevBuild-001）

半自律・Cloud Agent が「Development ビルド完了」を報告する前に、**次を実行**して CI の `vcxproj-coverage` 相当＋構成契約を満たすこととみなします。

```bash
bash EditorAssets/Tools/CursorAI/runCloudDevelopmentPreflight.sh
```

### プリフライトの内容

| 検査 | スクリプト |
|------|-----------|
| vcxproj／シェーダ登録 | `verify_vcxproj_coverage.py` |
| エディタコンポーネント登録 | `verify_editor_component_registration.py` |
| 構成マトリクス | `verify_config_matrix.py` |
| buildDevelopment と CI の一致 | `verify_build_development_ci_parity.py` |
| NuGet 復元手順の定義 | `verify_build_development_nuget_restore.py` |
| 同梱アセットパス | `verify_asset_pack_paths.py` |

## 完全なコンパイル

- **ローカル Windows:** `buildDevelopment.cmd` または `buildDevelopmentProfile.cmd`
- **CI:** `.github/workflows/DevelopmentBuild.yml`（Windows runner）

正本: `cloud_build_vcxproj_check.md`