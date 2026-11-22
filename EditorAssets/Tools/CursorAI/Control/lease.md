# lease（ループ占有宣言）

`loopLease.ps1` が自動更新する。手で編集しない（手順: `cursorInstructions.md` §7.1.5）。
各ループは自セクションの paths にのみ書き込む。共用ファイル（progress_report.md など）は全ループが追記してよい。

## roadmap-dev
- inTick: true
- updatedAt: 2026-11-22T16:22:14.9884182+09:00
- expiresAt: 2026-11-22T17:22:14.9884182+09:00
- task: atomF XZ平面かつY座標0の床
- paths:
  - `Project/Codes/Include/System/AtomF/CmpAtomFPlayerShot.h`
  - `Project/Codes/Source/System/AtomF/CmpAtomFPlayerShot.cpp`
  - `Project/Codes/Source/Scene/SceneBuildUpAtomFPlayerSpawn.cpp`
  - `Project/CG2_BuildUp.vcxproj`
  - `Project/CG2_BuildUp.vcxproj.filters`
