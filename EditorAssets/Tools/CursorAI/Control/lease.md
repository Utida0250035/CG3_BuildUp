# lease（ループ占有宣言）

`loopLease.ps1` が自動更新する。手で編集しない（手順: `cursorInstructions.md` §7.1.5）。
各ループは自セクションの paths にのみ書き込む。共用ファイル（progress_report.md など）は全ループが追記してよい。

## roadmap-dev
- inTick: true
- updatedAt: 2026-11-23T11:19:08.5432649+09:00
- expiresAt: 2026-11-23T12:19:08.5432649+09:00
- task: 自機がダメージを受けたときの演出 › 効果音が 1 回鳴る
- paths:
  - `Project/Codes/Include/System/AtomF/`
  - `Project/Codes/Source/System/AtomF/`
  - `Project/Codes/Source/System/Entity/Components/CmpHealthHit.cpp`
  - `Project/Codes/Include/System/PlaySe.h`
  - `EditorAssets/Tools/CursorAI/loop_state.json`
