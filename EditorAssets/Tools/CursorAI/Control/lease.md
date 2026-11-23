# lease（ループ占有宣言）

`loopLease.ps1` が自動更新する。手で編集しない（手順: `cursorInstructions.md` §7.1.5）。
各ループは自セクションの paths にのみ書き込む。共用ファイル（progress_report.md など）は全ループが追記してよい。

## roadmap-dev
- inTick: true
- updatedAt: 2026-11-23T09:59:52.6701094+09:00
- expiresAt: 2026-11-23T12:59:52.6701094+09:00
- task: 敵の焦りモーション › 当たり判定は動かない
- paths:
  - `Project/Codes/Include/System/AtomF/`
  - `Project/Codes/Source/System/AtomF/`
  - `EditorAssets/Tools/CursorAI/loop_state.json`
  - `Project/Codes/Include/System/Entity/Components/CmpMeshSphere.h`
  - `Project/Codes/Source/System/Entity/Components/CmpMeshSphere.cpp`
  - `Project/Codes/Source/Scene/SceneBuildUpAtomFEnemySpawn.cpp`
