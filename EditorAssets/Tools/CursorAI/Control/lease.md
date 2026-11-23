# lease（ループ占有宣言）

`loopLease.ps1` が自動更新する。手で編集しない（手順: `cursorInstructions.md` §7.1.5）。
各ループは自セクションの paths にのみ書き込む。共用ファイル（progress_report.md など）は全ループが追記してよい。

## roadmap-dev
- inTick: true
- updatedAt: 2026-11-23T09:39:00.9020056+09:00
- expiresAt: 2026-11-23T10:39:00.9020056+09:00
- task: 当たり判定は半径cの球
- paths:
  - `Project/Codes/Include/System/AtomF/`
  - `Project/Codes/Source/System/AtomF/`
  - `Project/Codes/Include/System/Entity/Components/CmpHitSphere.h`
  - `EditorAssets/Tools/CursorAI/loop_state.json`
