# lease（ループ占有宣言）

`loopLease.ps1` が自動更新する。手で編集しない（手順: `cursorInstructions.md` §7.1.5）。
各ループは自セクションの paths にのみ書き込む。共用ファイル（progress_report.md など）は全ループが追記してよい。

## roadmap-dev
- inTick: true
- updatedAt: 2026-11-22T16:53:23.8737352+09:00
- expiresAt: 2026-11-22T19:53:23.8737352+09:00
- task: atomF XZ平面かつY座標0の床
- paths:
  - `Project/Codes/Include/System/AtomF/CmpAtomFPlayerFire.h`
  - `Project/Codes/Source/System/AtomF/CmpAtomFPlayerFire.cpp`
  - `Project/Codes/Include/System/AtomF/CmpAtomFPlayerBullet.h`
  - `Project/Codes/Source/System/AtomF/CmpAtomFPlayerBullet.cpp`
  - `Project/Codes/Source/Scene/SceneBuildUpAtomFPlayerSpawn.cpp`
  - `Project/CG2_BuildUp.vcxproj`
  - `Project/CG2_BuildUp.vcxproj.filters`
  - `Project/Codes/Include/System/AtomF/AtomFPlayerBulletSpawn.h`
