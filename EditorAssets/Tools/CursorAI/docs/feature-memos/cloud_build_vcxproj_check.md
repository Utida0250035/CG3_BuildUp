# クラウドビルド：vcxproj 穴チェック

## 用途

- **CI／リポジトリ:** 新規 `.cpp` が `CG2_BuildUp.vcxproj` 未登録のままマージされるのを防ぐ
- **エージェント開発:** Linux Cloud でも `verify_vcxproj_coverage.py` でリンク漏れを先に検出

## 実行

```bash
python3 EditorAssets/Tools/CursorAI/verify_vcxproj_coverage.py
```

## GitHub Actions

- 独立: `.github/workflows/BuildCoverageCheck.yml`
- `Debug` / `Development` / `Release` ビルドの前段 `vcxproj-coverage` ジョブ

## 許容リスト

`EditorAssets/Tools/CursorAI/vcxproj_coverage_allowlist.txt` — 意図的に vcxproj 外の `.cpp`（Lua スクリプト基盤など）
