# CodeTemplateGenerator

`EditorAssets/Templates/Code/manifest.json` に従い、`%ClassName%` 等を置換して `Project/Codes/` へファイルを書き出す。

## 用途

- 新規 `ComponentDraw` / `ComponentUpd` / 行動クラスの雛形をエディタ外からも生成
- 生成後は `verify_vcxproj_coverage.py` で vcxproj 登録漏れを確認

## 例

```bash
# テンプレ一覧
python3 EditorAssets/Tools/CodeTemplateGenerator/scaffold_from_template.py list

# ComponentDraw（ドライラン）
python3 EditorAssets/Tools/CodeTemplateGenerator/scaffold_from_template.py generate \
  component_draw CmpDebugMarker --dry-run

# ComponentUpd
python3 EditorAssets/Tools/CodeTemplateGenerator/scaffold_from_template.py generate \
  component_upd CmpPatrol --upd-group MOVEMENT --author-note "stage patrol"
```

生成先が既にある場合は `--force` が必要。
