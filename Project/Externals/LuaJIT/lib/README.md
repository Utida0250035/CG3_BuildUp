# LuaJIT バイナリ（リンク用）

MSBuild は `$(ProjectDir)Externals\LuaJIT\lib\lua51.lib` と
`$(ProjectDir)Externals\LuaJIT\bin\lua51.dll` を参照します。

## 初回ビルド

リポジトリルートから:

```bat
EditorAssets\Tools\CursorAI\buildLuaJitLib.cmd
```

LuaJIT ソースを取得して `lib\lua51.lib` と `bin\lua51.dll` を生成します。