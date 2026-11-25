# Solution Explorer（vcxproj.filters）同期

Visual Studio のソリューションエクスプローラー表示は `Project/CG2_BuildUp.vcxproj.filters` の **Filter** で決まる。ビルドには影響しない。

## 方針 A（既定）

- `vcxproj` にあって `filters` に **無い** `ClCompile` / `ClInclude` を追記する
- `<Filter>` が **無い** 行だけを、Include パスの親ディレクトリに割り当てる
- 既に `<Filter>` がある行は **変更しない**
- 不足している `Filter Include`（仮想フォルダ定義）と GUID を末尾に追加する
- 既存行が参照するが未定義の Filter Include も `--write` 時に補う

例: `Codes\Source\System\AtomF\Foo.cpp` → `Codes\Source\System\AtomF`

## 使い方

リポジトリルートで:

```bat
EditorAssets\Tools\SolutionExplorer\sync_vcxproj_filters.cmd
EditorAssets\Tools\SolutionExplorer\sync_vcxproj_filters.cmd --write
```

または:

```bash
python3 EditorAssets/Tools/SolutionExplorer/sync_vcxproj_filters.py
python3 EditorAssets/Tools/SolutionExplorer/sync_vcxproj_filters.py --write
```

## 検証

```bat
EditorAssets\Tools\SolutionExplorer\verify_vcxproj_filters.cmd
```

- 全ソースに `<Filter>` があること
- 参照先の `Filter Include` が定義されていること
- **`vcxproj` と `filters` の Include パス集合が一致**すること（両方向）

パスと Filter 名の不一致は方針 A では **既存行は報告のみ**（自動修正しない）。

## 新規ソース追加時

`CG2_BuildUp.vcxproj` に登録したら、同じ Include で `filters` にも載せ、`--write` で Filter を付けるか、Visual Studio 上でフォルダへ移動する。