# lease（ループ占有宣言）

`loopLease.ps1` が自動更新する。手で編集しない（手順: `cursorInstructions.md` §7.1.5）。
各ループは自セクションの paths にのみ書き込む。共用ファイル（progress_report.md など）は全ループが追記してよい。

## roadmap-dev
- inTick: true
- updatedAt: 2026-11-23T08:39:07.7153259+09:00
- expiresAt: 2026-11-23T11:39:07.7153259+09:00
- task: 自機弾や別の敵の爆発に吹き飛ばされたあと、壁や別の敵に当たると爆発し、パーティクルを散らして小さくなって消滅する
- paths:
  - `Project/Codes/Include/System/AtomF/`
  - `Project/Codes/Source/System/AtomF/`
  - `Project/Codes/Include/Scene/SceneBuildUpAtomFEntityDestroyPurge.h`
  - `Project/Codes/Source/Scene/SceneBuildUpAtomFEntityDestroyPurge.cpp`
  - `Project/Codes/Source/Scene/SceneBuildUp.cpp`
  - `Project/Codes/Source/Scene/SceneBuildUpAtomFEnemySpawn.cpp`
  - `Project/Codes/Include/System/Entity/Components/CmpHitSphere.h`
  - `Project/Codes/Source/System/Entity/Components/CmpHitSphere.cpp`
  - `Project/CG2_BuildUp.vcxproj`
  - `Project/CG2_BuildUp.vcxproj.filters`
  - `EditorAssets/Tools/CursorAI/buildDevelopment.cmd`
  - `EditorAssets/Tools/CursorAI/restoreDevelopmentNuGetPackages.cmd`
  - `EditorAssets/Tools/CursorAI/loop_state.json`
  - `Project/Codes/Include/System/Entity/Entity.h`
