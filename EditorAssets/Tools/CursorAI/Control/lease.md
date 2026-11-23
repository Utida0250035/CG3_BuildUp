# lease（ループ占有宣言）

`loopLease.ps1` が自動更新する。手で編集しない（手順: `cursorInstructions.md` §7.1.5）。
各ループは自セクションの paths にのみ書き込む。共用ファイル（progress_report.md など）は全ループが追記してよい。

## roadmap-dev
- inTick: true
- updatedAt: 2026-11-23T10:40:13.3515150+09:00
- expiresAt: 2026-11-23T13:40:13.3515150+09:00
- task: 敵の焦りモーション › 天辺から汗のパーティクルを流す
- paths:
  - `Project/Codes/Include/System/AtomF/`
  - `Project/Codes/Source/System/AtomF/`
  - `Project/Codes/Source/Scene/SceneBuildUpAtomFEnemySpawn.cpp`
  - `EditorAssets/Tools/CursorAI/loop_state.json`
  - `Project/Codes/Include/System/Particle/GameCombatParticleEffects.h`
  - `Project/Codes/Source/System/Particle/GameCombatParticleEffects.cpp`
  - `Project/Assets/Particles/enemy_panic_sweat.json`
