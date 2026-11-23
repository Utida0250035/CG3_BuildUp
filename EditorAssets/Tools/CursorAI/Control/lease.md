# lease（ループ占有宣言）

`loopLease.ps1` が自動更新する。手で編集しない（手順: `cursorInstructions.md` §7.1.5）。
各ループは自セクションの paths にのみ書き込む。共用ファイル（progress_report.md など）は全ループが追記してよい。

## roadmap-dev
- inTick: true
- updatedAt: 2026-11-23T11:59:51.9086034+09:00
- expiresAt: 2026-11-23T14:59:51.9086034+09:00
- task: EntityEditorUi › ヒット球は選択 1 球のみ編集
- paths:
  - `Project/Codes/Source/System/Editor/Entity/EntityEditorUi.cpp`
  - `Project/Codes/Source/System/Editor/Entity/EntityEditorUiDrawComponentParamsGenericJson.cpp`
  - `Project/Codes/Include/System/Editor/Entity/EntityEditorUiDrawComponentParamsFallbackJson.h`
  - `Project/Codes/Source/System/Editor/Entity/EntityEditorUiDrawComponentParamsFallbackJson.cpp`
  - `Project/CG2_BuildUp.vcxproj`
