# lease（ループ占有宣言）

`loopLease.ps1` が自動更新する。手で編集しない（手順: `cursorInstructions.md` §7.1.5）。
各ループは自セクションの paths にのみ書き込む。共用ファイル（progress_report.md など）は全ループが追記してよい。

## roadmap-dev
- inTick: true
- updatedAt: 2026-11-23T10:59:28.3304061+09:00
- expiresAt: 2026-11-23T13:59:28.3304061+09:00
- task: 自機がダメージを受けたときの演出 › 赤いビネットが画面端に 0.75 秒出る
- paths:
  - `Project/Codes/Include/System/AtomF/`
  - `Project/Codes/Source/System/AtomF/`
  - `Project/Codes/Include/System/Entity/Components/CmpHealthHit.h`
  - `Project/Codes/Source/System/Entity/Components/CmpHealthHit.cpp`
  - `Project/Codes/Source/Scene/SceneBuildUp.cpp`
  - `EditorAssets/Tools/CursorAI/loop_state.json`
  - `Project/Codes/Include/Camera/PlayerDamageScreenVignette.h`
  - `Project/Codes/Source/Camera/PlayerDamageScreenVignette.cpp`
