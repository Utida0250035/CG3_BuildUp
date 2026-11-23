# lease（ループ占有宣言）

`loopLease.ps1` が自動更新する。手で編集しない（手順: `cursorInstructions.md` §7.1.5）。
各ループは自セクションの paths にのみ書き込む。共用ファイル（progress_report.md など）は全ループが追記してよい。

## roadmap-dev
- inTick: true
- updatedAt: 2026-11-23T08:58:51.8716999+09:00
- expiresAt: 2026-11-23T09:58:51.8716999+09:00
- task: 自機弾や別の敵の爆発に吹き飛ばされる速度は速さb
- paths:
  - `Project/Codes/Include/System/AtomF/`
  - `Project/Codes/Source/System/AtomF/`
  - `EditorAssets/Tools/CursorAI/loop_state.json`
