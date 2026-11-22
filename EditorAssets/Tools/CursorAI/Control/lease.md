# lease（ループ占有宣言）

`loopLease.ps1` が自動更新する。手で編集しない（手順: `cursorInstructions.md` §7.1.5）。
各ループは自セクションの paths にのみ書き込む。共用ファイル（progress_report.md など）は全ループが追記してよい。

## roadmap-dev
- inTick: true
- updatedAt: 2026-11-22T15:52:57.6753008+09:00
- expiresAt: 2026-11-22T18:52:57.6753008+09:00
- task: atomF XZ平面かつY座標0の床
- paths:
  - `Project/Assets/atomF/parameters.json`
  - `Project/Codes/Include/System/AtomF/AtomFParameters.h`
  - `Project/Codes/Source/System/AtomF/AtomFParameters.cpp`
  - `Project/Codes/Source/Scene/SceneBuildUpAtomFFloorSpawn.cpp`
  - `Project/CG2_BuildUp.vcxproj`
  - `Project/CG2_BuildUp.vcxproj.filters`
  - `Project/Codes/Include/System/GameAssetsPaths.h`
  - `Project/Codes/Source/Scene/SceneBuildUp.cpp`
  - `Project/Codes/Include/Scene/SceneBuildUpAtomFParametersBoot.h`
  - `Project/Codes/Source/Scene/SceneBuildUpAtomFParametersBoot.cpp`
  - `Project/Codes/Source/Scene/SceneBuildUpAtomFArenaContainmentTick.cpp`
  - `Project/Codes/Source/System/AtomF/AtomFArenaContainmentAttach.cpp`
  - `Project/Codes/Include/System/AtomF/AtomFParametersAccess.h`
  - `Project/Codes/Source/System/AtomF/AtomFParametersAccess.cpp`
