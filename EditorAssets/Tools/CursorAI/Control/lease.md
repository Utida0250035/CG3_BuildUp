# lease（ループ占有宣言）

`loopLease.ps1` が自動更新する。手で編集しない（手順: `cursorInstructions.md` §7.1.5）。
各ループは自セクションの paths にのみ書き込む。共用ファイル（progress_report.md など）は全ループが追記してよい。

## roadmap-dev
- inTick: true
- updatedAt: 2026-11-23T11:44:59.5865037+09:00
- expiresAt: 2026-11-23T14:44:59.5865037+09:00
- task: EntityEditorUi › ヒット球は選択 1 球のみ編集
- paths:
  - `Project/Codes/Include/System/Editor/`
  - `Project/Codes/Source/System/Editor/`
  - `EditorAssets/Tools/CursorAI/loop_state.json`
  - `Project/CG2_BuildUp.vcxproj`
  - `Project/Codes/Include/System/Editor/Entity/EntityEditorUiDrawComponentParamsHitSphere.h`
  - `Project/Codes/Source/System/Editor/Entity/EntityEditorUiDrawComponentParamsHitSphere.cpp`
  - `Project/Codes/Source/System/Editor/Entity/EntityEditorUi.cpp`
