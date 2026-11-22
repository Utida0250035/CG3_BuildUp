# lease（ループ占有宣言）

`loopLease.ps1` が自動更新する。手で編集しない（手順: `cursorInstructions.md` §7.1.5）。
各ループは自セクションの paths にのみ書き込む。共用ファイル（progress_report.md など）は全ループが追記してよい。

## roadmap-dev
- inTick: true
- updatedAt: 2026-11-22T19:22:12.6000783+09:00
- expiresAt: 2026-11-22T22:22:12.6000783+09:00
- task: atomF XZ平面かつY座標0の床
- paths:
  - `Project/Codes/Source/Scene/SceneBuildUpAtomFEnemySpawn.cpp`
  - `Project/Codes/Source/System/AtomF/CmpAtomFEnemyWalkToPlayer.cpp`
