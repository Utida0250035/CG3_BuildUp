# 実装ギャップ一覧（用途別・正本）

人間が読む正本。機械による重複判定・マージの正本は **`implementation_gaps_index.json`**。

<!-- gaps-schema: 2 -->

## メタ

- **schemaVersion:** 2
- **lastFullScanAt:** （未実施）
- **lastIncrementalScanAt:** 2026-11-10T02:00:32.963377+09:00
- **関連:** `use_case_scenarios.md`（シナリオ正本） / `implementation_gaps_index.json`

## 運用（エージェント向け要約）

**キーワード:** `ギャップスキャンを実行して`（1 回 = 1 tick）

1. `planGapScanTick.py` で **`gap_scan_tick.json`** の範囲（シナリオ 1 件）を取得する。
2. スキャン前に **`implementation_gaps_index.json`** を Read する。
3. 計画されたシナリオについてのみ、「その用途でできないこと」だけを GAP 化する。
3. **用途が推測できない候補は GAP にしない**（下記 §用途不明（確認待ち）へ `PENDING-UC-*` のみ）。
4. マージは **`scopeKey`**（`primaryUseCase | Area | PrimarySymbol | Kind | Anchor`）で突合。一致時は **新規 ID を発行しない**（`summary` / `evidence` / `lastSeenAt` の更新のみ）。
5. 一覧本文は **index から再生成してよい**（見出し `### GAP-...` は ID ごとに 1 つのみ）。
6. セッション報告は **`progress_report.md`** に `scanId`・`useCase`・`newIds` 件数のみ追記（ギャップ本文はここが正本）。

## 並び順

1. `status` が `open` または `investigating` のみ §ギャップ一覧 に載せる。
2. **`score` 降順**（同点は `GAP-...` 辞書順）。
3. 読みやすさのため **UseCase 見出しで束ねてもよい**（同一 ID の二重掲載は禁止）。

## ID ルール

- 形式: `GAP-<Area>-<Subject>-<NNN>`
- **Area:** `Runtime` / `Editor` / `Streaming` / `Scene` / `Render` / `Resource` / `UX` / `Infra`
- **Kind:** `Stub` / `Unwired` / `Incomplete` / `MissingTest` / `MissingUI` / `MissingData` / `Contract` / `PerfObs`

## summary 必須型（1 行）

```text
【Play|Authoring|Debug|Ship】<その用途でできないこと／足りないこと>
```

## 優先度（tier / score）

| tier | score 目安 |
|------|----------------|
| P0 | ≥ 900 |
| P1 | 700–899 |
| P2 | 400–699 |
| P3 | &lt; 400 |

- **用途不明の GAP:** 作らない。調査中で人間判断待ちの GAP は tier **P2 上限**。
- `frequencyBasis` には **UseCase 視点**（誰が・どの導線で困るか）を必ず 1 行書く。

## UseCase ID（正本）

| ID | ラベル | 対応（§6.4） |
|----|--------|----------------|
| `UC-Play` | Play | ランタイムのゲームプレイ |
| `UC-Authoring` | Authoring | レベル／アセット制作 |
| `UC-Debug` | Debug | デバッグ・プロファイリング |
| `UC-Ship` | Ship | ビルド・配布 |

---

## 用途不明（確認待ち）

なし

---

## ギャップ一覧

#### UC-Authoring

### GAP-Editor-StageViewCameraTuning-001

- status: open
- tier: P1
- score: 820
- primaryUseCase: UC-Authoring
- scenarioId: AUT-VisualTune-001
- frequency: high
- frequencyBasis: カメラ調整 JSON は CLI プレビューのみでエディタ即時確認不可
- scope: UC-Authoring|Editor|StageViewCameraTuning|MissingUI|EditorAssets/Tools/CursorAI/docs/feature-memos/odyssey_style_stage_camera.md:32
- primary: `StageViewCameraTuning`
- summary: 【Authoring】カメラ tuning JSON のエディタ内プレビュー／ホットリードが未実装
- scenarioText: 制作者がライト・カメラ・ポストのパラメータを編集し、プレビューで結果を確認できる。
- evidence:
  - odyssey_style_stage_camera.md に「未実装（次）: JSON ホットリード or ImGui スライダー→即プレビュー」（32-33 行）
  - 調整は preview_poses.py の外部 CLI のみ（StageViewCameraTuning/README）
- suggestedUse: 制作: カメラパラメータを編集しながらエディタで結果を見る
- roadmapLink: feature-memos/odyssey_style_stage_camera.md
- firstSeenAt: 2026-11-10T01:11:00+09:00
- lastSeenAt: 2026-11-10T01:11:00+09:00

### GAP-Editor-EntityEditor-002

- status: open
- tier: P1
- score: 800
- primaryUseCase: UC-Authoring
- scenarioId: AUT-EditorPlace-001
- frequency: high
- frequencyBasis: 別シーンを開くたびに未保存配置が消えうる
- scope: UC-Authoring|Editor|EntityEditor|Incomplete|Project/Codes/Source/System/Editor/Entity/EntityEditor.cpp:233
- primary: `EntityEditor`
- summary: 【Authoring】シーン切替時に未保存確認・自動保存が無い
- scenarioText: 制作者がエディタ上でオブジェクトを配置・保存し、再度開いて同じ状態を得られる。
- evidence:
  - OpenSceneFromPath は LoadFromJsonPath のみで Save 確認なし（233-241 行）
  - 未保存フラグ／ダーティ追跡の参照が Editor 配下に無い（rg unsaved 0 件）
- suggestedUse: 制作: 配置後に別シーンを開いてもデータを失わない
- firstSeenAt: 2026-11-10T00:31:00+09:00
- lastSeenAt: 2026-11-10T00:31:00+09:00

### GAP-Editor-EditorSceneRuntime-002

- status: open
- tier: P1
- score: 790
- primaryUseCase: UC-Authoring
- scenarioId: AUT-Component-001
- frequency: high
- frequencyBasis: JSON 手編集や kind 誤り時にコンポーネントが黙って落ちる
- scope: UC-Authoring|Editor|EditorSceneRuntime|Incomplete|Project/Codes/Source/System/Editor/Scene/EditorSceneRuntime.cpp:291
- primary: `EditorSceneRuntime`
- summary: 【Authoring】スロット種別不一致のコンポーネントが無警告でスキップされる
- scenarioText: エンティティ／コンポント分割方針に沿ってアセットを組み立てられる（spec: component_split_guidelines.md）。
- evidence:
  - TryAddEmptyComponent は typeName と ComponentSlotKind の組み合わせが一致しないと false（67-94 行）
  - BuildEntityFromDefinition は失敗時 continue のみでログ・UI 警告なし（289-293 行）
- suggestedUse: 制作: Draw/Upd/Data 分割違反をエディタで即気づける
- roadmapLink: feature-memos/component_split_guidelines.md
- firstSeenAt: 2026-11-10T00:11:00+09:00
- lastSeenAt: 2026-11-10T00:11:00+09:00

### GAP-Editor-AtrumEngineVisual-001

- status: open
- tier: P1
- score: 780
- primaryUseCase: UC-Authoring
- scenarioId: AUT-VisualTune-001
- frequency: high
- frequencyBasis: フォグ／ゴッドレイ／シャドウは SceneGame 直書きでエディタから触れない
- scope: UC-Authoring|Editor|AtrumEngineVisual|MissingUI|Project/Codes/Include/Engine/AtrumEngine.h:535
- primary: `AtrumEngineVisual`
- summary: 【Authoring】ライト・ポスト系パラメータのエディタ用 UI が無い
- scenarioText: 制作者がライト・カメラ・ポストのパラメータを編集し、プレビューで結果を確認できる。
- evidence:
  - RefVolumetricFog / RefGodRays / RefDirectionalShadow は SceneGame::SetupGameLighting のみで設定
  - EntityEditor ImGui は Cmp パラメータのみ（isLightingEnabled 等）でエンジン post 無し
- suggestedUse: 制作: ライト・ポストをプレビューしながら調整
- roadmapLink: feature-memos/volumetric_fog.md
- firstSeenAt: 2026-11-10T01:11:00+09:00
- lastSeenAt: 2026-11-10T01:11:00+09:00

### GAP-Editor-EntityEditor-001

- status: open
- tier: P1
- score: 780
- primaryUseCase: UC-Authoring
- scenarioId: AUT-AssetLoop-001
- frequency: high
- frequencyBasis: シーン編集の反復で同一配置を増やす際、毎回プレハブ経由か空追加が必要
- scope: UC-Authoring|Editor|EntityEditor|MissingUI|Project/Codes/Source/System/Editor/Entity/EntityEditorUi.cpp:374
- primary: `EntityEditor`
- summary: 【Authoring】選択エンティティの複製 UI が無く複製が多段手順
- scenarioText: 反復作業（複製・一括調整）がエディタ専用 UI で過度な手順にならない。
- evidence:
  - ヒエラルキーは「オブジェクトを追加」「削除」のみ（EntityEditorUi.cpp:374-379）
  - 複製 API（DuplicateSelectedInstance 等）が EntityEditor に存在しない
  - 同等作業はプレハブ JSON 出力→シーンへ追加の 2 操作が必要（457-487 行付近）
- suggestedUse: 制作: レベル内の複製配置を 1 クリックで回す
- firstSeenAt: 2026-11-09T23:41:00+09:00
- lastSeenAt: 2026-11-09T23:41:00+09:00

### GAP-UX-OperatorGuidanceSystem-001

- status: open
- tier: P1
- score: 760
- primaryUseCase: UC-Authoring
- secondaryUseCases: UC-Play
- scenarioId: AUT-Guidance-001
- frequency: high
- frequencyBasis: JSON の message を差し替えても画面上に出ない
- scope: UC-Authoring|UX|OperatorGuidanceSystem|Incomplete|Project/Codes/Source/UX/OperatorGuidanceSystem.cpp:229
- primary: `OperatorGuidanceSystem`
- summary: 【Authoring】hints.message が保持されるが描画されず文言編集が無効
- scenarioText: 初回利用者がオペレータ案内に沿って主要操作を完了できる（spec: operator_guidance_bubble.md）。
- evidence:
  - activeMessage_ に message を代入（133, 197 行等）するが Draw は画像のみ（229-306 行）
  - feature-memo にビットマップフォント message 描画は「今後」（operator_guidance_bubble.md）
- suggestedUse: 制作: JSON の文言差し替えがプレイで確認できる
- roadmapLink: feature-memos/operator_guidance_bubble.md
- firstSeenAt: 2026-11-10T00:51:00+09:00
- lastSeenAt: 2026-11-10T00:51:00+09:00

### GAP-Editor-UiState-001

- status: open
- tier: P2
- score: 710
- primaryUseCase: UC-Authoring
- scenarioId: AUT-AssetLoop-001
- frequency: high
- frequencyBasis: インスペクタは常に 1 選択前提で一括パラメータ調整ができない
- scope: UC-Authoring|Editor|UiState|MissingUI|Project/Codes/Include/System/Editor/Entity/EntityEditor.h:25
- primary: `UiState`
- summary: 【Authoring】複数選択が無く一括調整できない
- scenarioText: 反復作業（複製・一括調整）がエディタ専用 UI で過度な手順にならない。
- evidence:
  - UiState::selectedInstanceIndex は単一 int（EntityEditor.h:25）
  - DrawInspectorPanel は SelectedInstance() のみ編集（EntityEditorUi.cpp:490 以降）
  - 複数ノード選択・共通プロパティ編集の UI/状態が無い
- suggestedUse: 制作: 同種オブジェクトのスケール・タグ等をまとめて調整
- firstSeenAt: 2026-11-09T23:41:00+09:00
- lastSeenAt: 2026-11-09T23:41:00+09:00

### GAP-Infra-stage_view_camera_tuning-001

- status: open
- tier: P2
- score: 710
- primaryUseCase: UC-Authoring
- secondaryUseCases: UC-Play
- scenarioId: AUT-VisualTune-001
- frequency: medium
- frequencyBasis: JSON 変更がランタイムに自動反映されず C++ 定数手コピー
- scope: UC-Authoring|Infra|stage_view_camera_tuning|Unwired|EditorAssets/Camera/stage_view_camera_tuning.json:1
- primary: `stage_view_camera_tuning`
- summary: 【Authoring】tuning.json と StageViewCamera 定数の自動同期が無い
- scenarioText: 制作者がライト・カメラ・ポストのパラメータを編集し、プレビューで結果を確認できる。
- evidence:
  - odyssey_style_stage_camera.md: tuning 変更後は kStageBattleView* へ手動反映（36 行）
  - Codes から stage_view_camera_tuning.json のロード参照無し（rg）
- suggestedUse: 制作: データ編集がプレイ中カメラにそのまま反映される
- firstSeenAt: 2026-11-10T01:11:00+09:00
- lastSeenAt: 2026-11-10T01:11:00+09:00

### GAP-Editor-EntityEditorDefinition-002

- status: open
- tier: P2
- score: 700
- primaryUseCase: UC-Authoring
- scenarioId: AUT-Component-001
- frequency: medium
- frequencyBasis: 方針メモの CmpMesh 等が UI 登録表とプレビュー実装で不整合
- scope: UC-Authoring|Editor|EntityEditorDefinition|Incomplete|Project/Codes/Source/System/Editor/Entity/EntityEditorDefinition.cpp:17
- primary: `EntityEditorDefinition`
- summary: 【Authoring】登録コンポーネント一覧がガイドライン／プレビュー経路と同期していない
- scenarioText: エンティティ／コンポント分割方針に沿ってアセットを組み立てられる（spec: component_split_guidelines.md）。
- evidence:
  - IsKnownEntityEditorComponent は 6 型のみ（EntityEditorDefinition.cpp:17-20）
  - 方針メモは CmpMesh を描画例に挙げるが kKnownComponents に無い
  - EditorSceneRuntime はプレビュー時に CmpMesh/CmpEditorTextureBillboard を動的追加（253-274 行）がインスペクタから選べない
- suggestedUse: 制作: 分割方針どおりのコンポーネントを UI から一貫して選べる
- roadmapLink: feature-memos/component_split_guidelines.md
- firstSeenAt: 2026-11-10T00:11:00+09:00
- lastSeenAt: 2026-11-10T00:11:00+09:00

### GAP-Editor-UXAuthoring-001

- status: open
- tier: P2
- score: 700
- primaryUseCase: UC-Authoring
- scenarioId: AUT-Guidance-001
- frequency: high
- frequencyBasis: 案内データは EditorAssets の手編集のみでエディタ UI が無い
- scope: UC-Authoring|Editor|UXAuthoring|MissingUI|EditorAssets/UX/operator_guidance.json:1
- primary: `UXAuthoring`
- summary: 【Authoring】operator_guidance.json をエディタから編集・プレビューできない
- scenarioText: 初回利用者がオペレータ案内に沿って主要操作を完了できる（spec: operator_guidance_bubble.md）。
- evidence:
  - System::LoadFromJsonFile は SceneGame::EnterScene のみ（SceneGame.cpp:111）
  - Editor 配下に operator_guidance／UX JSON 編集 UI の参照無し
  - 初回利用者向け案内の制作フローは外部エディタで JSON 直編集のみ
- suggestedUse: 制作: 案内文言・画像パスをエディタ内で反復調整
- firstSeenAt: 2026-11-10T00:51:00+09:00
- lastSeenAt: 2026-11-10T00:51:00+09:00

### GAP-Editor-EntityEditor-003

- status: open
- tier: P2
- score: 680
- primaryUseCase: UC-Authoring
- scenarioId: AUT-EditorPlace-001
- frequency: medium
- frequencyBasis: 再オープン時にパス誤りだと空シーンが黙って作られる
- scope: UC-Authoring|Editor|EntityEditor|Incomplete|Project/Codes/Source/System/Editor/Entity/EntityEditor.cpp:129
- primary: `EntityEditor`
- summary: 【Authoring】シーン JSON が無い／開けないとき空シーンへ置換し通知しない
- scenarioText: 制作者がエディタ上でオブジェクトを配置・保存し、再度開いて同じ状態を得られる。
- evidence:
  - exists しないパスでデフォルト 1 エンティティシーンを生成（129-139 行）
  - ifstream 失敗時は return のみ（143-145 行）で importStatusMessage 等に載せない
- suggestedUse: 制作: 「同じ状態を得られる」ための読込失敗を可視化
- firstSeenAt: 2026-11-10T00:31:00+09:00
- lastSeenAt: 2026-11-10T00:31:00+09:00

### GAP-UX-OperatorGuidanceSystem-002

- status: open
- tier: P2
- score: 680
- primaryUseCase: UC-Authoring
- secondaryUseCases: UC-Play
- scenarioId: AUT-Guidance-001
- frequency: medium
- frequencyBasis: JSON に actionId を足しても NotifyAction が parry のみ
- scope: UC-Authoring|UX|OperatorGuidanceSystem|Unwired|Project/Codes/Source/System/Playable/CmpMoveByInput.cpp:119
- primary: `OperatorGuidanceSystem`
- summary: 【Authoring】案内トリガー actionId のコード配線が parry 固定
- scenarioText: 初回利用者がオペレータ案内に沿って主要操作を完了できる（spec: operator_guidance_bubble.md）。
- evidence:
  - NotifyAction 呼び出しは CmpMoveByInput の parry のみ（rg 全体）
  - operator_guidance.json の actionId 追加だけでは新操作案内は発火しない
- suggestedUse: 制作: 主要操作ごとに案内ヒントをデータ駆動で増やす
- roadmapLink: feature-memos/operator_guidance_bubble.md
- firstSeenAt: 2026-11-10T00:51:00+09:00
- lastSeenAt: 2026-11-10T00:51:00+09:00

### GAP-Editor-EditorSceneRuntime-003

- status: open
- tier: P2
- score: 660
- primaryUseCase: UC-Authoring
- scenarioId: AUT-EditorPlace-001
- frequency: high
- frequencyBasis: 配置はインスペクタ数値のみでビューポート操作が無い
- scope: UC-Authoring|Editor|EditorSceneRuntime|MissingUI|Project/Codes/Source/System/Editor/Scene/EditorSceneRuntime.cpp:281
- primary: `EditorSceneRuntime`
- summary: 【Authoring】ビューポートから配置・移動して JSON に書き戻す経路が無い
- scenarioText: 制作者がエディタ上でオブジェクトを配置・保存し、再度開いて同じ状態を得られる。
- evidence:
  - initialTransform は ImGui インスペクタのみ（EntityEditorUi.cpp:617-618）
  - Editor 配下に gizmo／ドラッグ配置の実装参照無し（rg gizmo 0 件）
  - BuildEntityFromDefinition は JSON→Entity 一方向（EditorSceneRuntime.cpp:278-281）
- suggestedUse: 制作: 視覚的に配置して保存→再オープンで同じ姿勢を得る
- firstSeenAt: 2026-11-10T00:31:00+09:00
- lastSeenAt: 2026-11-10T00:31:00+09:00

### GAP-Editor-EntityEditorDefinition-001

- status: open
- tier: P2
- score: 650
- primaryUseCase: UC-Authoring
- scenarioId: AUT-AssetLoop-001
- frequency: medium
- frequencyBasis: コンポーネント構成の再利用は手作業コピー前提
- scope: UC-Authoring|Editor|EntityEditorDefinition|MissingUI|Project/Codes/Source/System/Editor/Entity/EntityEditorUi.cpp:530
- primary: `EntityEditorDefinition`
- summary: 【Authoring】コンポーネント設定のコピー／ペーストが無い
- scenarioText: 反復作業（複製・一括調整）がエディタ専用 UI で過度な手順にならない。
- evidence:
  - Editor 配下に Copy/Paste/Duplicate コンポーネント処理の参照が無い（rg 確認）
  - 類似エンティティへ Draw/Behavior スロットを揃える手段は JSON 手編集かプレハブ再投入のみ
- suggestedUse: 制作: 同型オブジェクトへコンポーネントパラメータを素早く展開
- firstSeenAt: 2026-11-09T23:41:00+09:00
- lastSeenAt: 2026-11-09T23:41:00+09:00

### GAP-Infra-component_split_verify-001

- status: open
- tier: P2
- score: 620
- primaryUseCase: UC-Authoring
- secondaryUseCases: UC-Ship
- scenarioId: AUT-Component-001
- frequency: low
- frequencyBasis: 新規 Cmp 追加時の 3 ファイル手順は手動チェックのみ
- scope: UC-Authoring|Infra|component_split_verify|MissingTest|EditorAssets/Tools/CursorAI/docs/feature-memos/component_split_guidelines.md:19
- primary: `component_split_verify`
- summary: 【Authoring】コンポーネント登録の三箇所同期を検証する自動化が無い
- scenarioText: エンティティ／コンポント分割方針に沿ってアセットを組み立てられる（spec: component_split_guidelines.md）。
- evidence:
  - ガイドラインは EntityEditorDefinition / EntityEditorUi / EditorSceneRuntime の 4 手順を規定（19-24 行）
  - verify_camera_policy のような component 用 verify スクリプト／CI 呼び出しが無い
- suggestedUse: 制作・配布: 登録漏れを PR 前に検出
- roadmapLink: feature-memos/component_split_guidelines.md
- firstSeenAt: 2026-11-10T00:11:00+09:00
- lastSeenAt: 2026-11-10T00:11:00+09:00

#### UC-Debug

### GAP-Scene-SceneBuildUpRegistry-001

- status: open
- tier: P1
- score: 820
- primaryUseCase: UC-Debug
- secondaryUseCases: UC-Authoring
- scenarioId: DBG-FeatureScene-001
- frequency: medium
- frequencyBasis: DebugPlayArea 手順は README のみでエディタ UI から到達できない
- scope: UC-Debug|Scene|SceneBuildUpRegistry|Unwired|Project/Codes/Source/Scene/SceneBuildUp.cpp:35
- primary: `SceneBuildUpRegistry`
- summary: 【Debug】最小ランタイム検証シーン起動がエディタから未配線
- scenarioText: 開発者がエディタ既定の機能検証シーンから、エディタ外ランタイム機能だけを最小構成で試せる。
- evidence:
  - SceneBuildUpRegistry::Launch は実装あるが Editor コードから参照無し（rg Editor 配下 0 件）
  - DebugPlayAreaGenerator README の手順 2 のみ（C++ 直 Launch 想定）
- suggestedUse: デバッグ: ストリーミング等を本編外の最小シーンで試す
- roadmapLink: feature-memos/debug_test_play_area.md
- firstSeenAt: 2026-11-09T23:51:00+09:00
- lastSeenAt: 2026-11-09T23:51:00+09:00

### GAP-Editor-EditorAssetsPaths-001

- status: open
- tier: P1
- score: 810
- primaryUseCase: UC-Debug
- secondaryUseCases: UC-Authoring
- scenarioId: DBG-PlayArea-001
- frequency: high
- frequencyBasis: テスト再生エリアの既定パスがエディタ定数に無い
- scope: UC-Debug|Editor|EditorAssetsPaths|MissingData|Project/Codes/Include/System/Editor/EditorAssetsPaths.h:6
- primary: `EditorAssetsPaths`
- summary: 【Debug】DebugPlayArea 用の EditorAssets パス定数が無い
- scenarioText: 開発者がエディタ内のテスト再生エリアでランタイム挙動を素早く再現できる（spec: `feature-memos/debug_test_play_area.md`）。
- evidence:
  - EditorAssetsPaths は Scenes/Entities/Particles のみ（6-8 行）
  - debug_test_play_area.md の成果物は EditorAssets/DebugPlayArea/ 配下
- suggestedUse: デバッグ: テスト用 JSON の場所をエディタコードで一意に参照
- roadmapLink: feature-memos/debug_test_play_area.md
- firstSeenAt: 2026-11-10T02:11:00+09:00
- lastSeenAt: 2026-11-10T02:11:00+09:00

### GAP-Editor-EntityEditorUi-004

- status: open
- tier: P1
- score: 790
- primaryUseCase: UC-Debug
- scenarioId: DBG-PlayArea-001
- frequency: high
- frequencyBasis: シーン一覧 UI が Scenes ディレクトリ限定
- scope: UC-Debug|Editor|EntityEditorUi|MissingUI|Project/Codes/Source/System/Editor/Entity/EntityEditorUi.cpp:419
- primary: `EntityEditorUi`
- summary: 【Debug】テスト再生エリア JSON がエディタのシーン一覧に出ない
- scenarioText: 開発者がエディタ内のテスト再生エリアでランタイム挙動を素早く再現できる（spec: `feature-memos/debug_test_play_area.md`）。
- evidence:
  - ListSceneJsonFiles(kScenesDirectory) のみ表示（EntityEditorUi.cpp:419-420）
  - DebugPlayArea/*.scene.json は一覧対象外
- suggestedUse: デバッグ: ワンクリックでサンドボックスシーンを開く
- firstSeenAt: 2026-11-10T02:11:00+09:00
- lastSeenAt: 2026-11-10T02:11:00+09:00

### GAP-Infra-FeatureTestScene-001

- status: open
- tier: P1
- score: 760
- primaryUseCase: UC-Debug
- scenarioId: DBG-FeatureScene-001
- frequency: medium
- frequencyBasis: ユーザー合意の SceneFeatureTest 相当の既定アセット／シーンクラスが未整備
- scope: UC-Debug|Infra|FeatureTestScene|MissingData|EditorAssets/DebugPlayArea/template_test_play_area.scene.json:1
- primary: `FeatureTestScene`
- summary: 【Debug】エディタ既定の機能検証シーン（FeatureTest）が定義されていない
- scenarioText: 開発者がエディタ既定の機能検証シーンから、エディタ外ランタイム機能だけを最小構成で試せる。
- evidence:
  - コードベースに SceneFeatureTest クラス／シンボル無し（rg FeatureTest 0 件、Codes 配下）
  - DebugPlayArea テンプレは手動 scaffold のみで既定起動シーンではない（CommandScene は SceneEntityEditor）
- suggestedUse: デバッグ: 機能ごとミニシナリオを既定導線で開く
- firstSeenAt: 2026-11-09T23:51:00+09:00
- lastSeenAt: 2026-11-09T23:51:00+09:00

### GAP-Editor-SceneEntityEditor-001

- status: open
- tier: P2
- score: 740
- primaryUseCase: UC-Debug
- scenarioId: DBG-FeatureScene-001
- frequency: high
- frequencyBasis: Development 起動の既定シーンは EntityEditor で「シーンを実行」はプレビュータブ内シミュレーション
- scope: UC-Debug|Editor|SceneEntityEditor|Incomplete|Project/Codes/Source/Scene/SceneEntityEditor.cpp:129
- primary: `SceneEntityEditor`
- summary: 【Debug】シーン実行が本番相当 SceneBuildUp 経路にならない
- scenarioText: 開発者がエディタ既定の機能検証シーンから、エディタ外ランタイム機能だけを最小構成で試せる。
- evidence:
  - requestPlayRuntimeScene は Runtime タブ＋previewEntities_ の Update のみ（SceneEntityEditor.cpp:129-137）
  - CommandChangeScene による SceneBuildUp 遷移や既定 Feature シーン JSON パス無し
- suggestedUse: デバッグ: エディタ UI と切り離したランタイムパスで機能を検証
- firstSeenAt: 2026-11-09T23:51:00+09:00
- lastSeenAt: 2026-11-09T23:51:00+09:00

### GAP-Infra-scaffold_play_area-001

- status: open
- tier: P2
- score: 700
- primaryUseCase: UC-Debug
- scenarioId: DBG-PlayArea-001
- frequency: medium
- frequencyBasis: テンプレ複製は外部 Python のみでエディタから新規作成できない
- scope: UC-Debug|Infra|scaffold_play_area|Unwired|EditorAssets/Tools/DebugPlayAreaGenerator/scaffold_play_area.py:1
- primary: `scaffold_play_area`
- summary: 【Debug】テストエリア新規作成が scaffold スクリプト手動実行のみ
- scenarioText: 開発者がエディタ内のテスト再生エリアでランタイム挙動を素早く再現できる（spec: `feature-memos/debug_test_play_area.md`）。
- evidence:
  - debug_test_play_area.md フロー 1 は scaffold_play_area.py のみ（18 行）
  - Editor コードから scaffold 呼び出し／同等 UI 無し
- suggestedUse: デバッグ: エディタ内で名前入力→テスト JSON 生成まで一気通貫
- firstSeenAt: 2026-11-10T02:11:00+09:00
- lastSeenAt: 2026-11-10T02:11:00+09:00

#### UC-Play

### GAP-Scene-SceneMovieRegistry-001

- status: open
- tier: P1
- score: 850
- primaryUseCase: UC-Play
- scenarioId: PLY-Movie-001
- frequency: low
- frequencyBasis: 本編からカットシーンへ遷移する呼び出しがコード上存在しない
- scope: UC-Play|Scene|SceneMovieRegistry|Unwired|Project/Codes/Source/Scene/SceneMovie.cpp:23
- primary: `SceneMovieRegistry`
- summary: 【Play】ムービーシーン起動がゲームプレイ経路に未接続
- scenarioText: シーン内ムービー／カットシーンが再生完了まで破綻しない（spec: scene_movie.md）。
- evidence:
  - SceneMovieRegistry::Launch の呼び出しは Project/Codes 内に SceneMovie.cpp 自身以外無し
  - feature-memo の起動例はドキュメントのみ（SceneGame 等からの遷移無し）
- suggestedUse: プレイ: ボス戦前後などカットシーンを本編フローへ載せる
- roadmapLink: feature-memos/scene_movie.md
- firstSeenAt: 2026-11-10T00:21:00+09:00
- lastSeenAt: 2026-11-10T00:21:00+09:00

### GAP-Scene-ChangeSceneFade-001

- status: open
- tier: P2
- score: 720
- primaryUseCase: UC-Play
- scenarioId: PLY-Audio-001
- frequency: medium
- frequencyBasis: 全シーン遷移で ChangeSceneFade 経由のため、プレイ中の遷移 SE が鳴らないと体感が途切れる
- scope: UC-Play|Scene|ChangeSceneFade|Unwired|Project/Codes/Source/Scene/ChangeSceneFade.cpp:35
- primary: `ChangeSceneFade`
- summary: 【Play】フェードイン開始時に遷移 SE が未再生（フラグのみ立つ）
- scenarioText: プレイ中に BGM／SE が途切れず再生され、シーン遷移後も期待どおり鳴り続ける。
- evidence:
  - ChangeSceneFade::Update で isSeOpenTrigger_ を true にするが PlaySe 等の呼び出しが無い（ChangeSceneFade.cpp:35-37）
  - isSeOpenTrigger_ はヘッダにのみ定義され他参照なし
- suggestedUse: ランタイム: シーン遷移の手応えを SE で補い、無音フェードによる「音切れ」体感を減らす
- firstSeenAt: 2026-11-09T23:32:00+09:00
- lastSeenAt: 2026-11-09T23:32:00+09:00

### GAP-Scene-CommandScene-002

- status: open
- tier: P2
- score: 720
- primaryUseCase: UC-Play
- secondaryUseCases: UC-Debug
- scenarioId: PLY-SceneLoad-001
- frequency: high
- frequencyBasis: Development ビルド起動時は Title/Game ではなくエディタが最初のシーン
- scope: UC-Play|Scene|CommandScene|Incomplete|Project/Codes/Source/Scene/CommandScene.cpp:16
- primary: `CommandScene`
- summary: 【Play】Development 起動の初期シーンがプレイ用シーンロード経路と一致しない
- scenarioText: プレイヤーがシーンを開き、主要オブジェクトとカメラが安定して動作する。
- evidence:
  - DEVELOPMENT&&USE_IMGUI では sceneNow_=SceneEntityEditor（CommandScene.cpp:16-17）
  - Release 相当のみ SceneTitle からプレイシーンへ遷移
- suggestedUse: プレイ: 本編シーンを開いたときの挙動を開発ビルドでも再現したい
- firstSeenAt: 2026-11-10T00:41:00+09:00
- lastSeenAt: 2026-11-10T00:41:00+09:00

### GAP-Render-SceneGame-001

- status: open
- tier: P2
- score: 700
- primaryUseCase: UC-Play
- secondaryUseCases: UC-Authoring
- scenarioId: PLY-Visual-001
- frequency: high
- frequencyBasis: フォグ／リム／シャドウが C++ 直書きでデータ資産化されていない
- scope: UC-Play|Render|SceneGame|MissingData|Project/Codes/Source/Scene/SceneGame.cpp:35
- primary: `SceneGame`
- summary: 【Play】見た目パラメータがデータではなく SetupGameLighting 直書き
- scenarioText: ランタイムでライト・フォグ等の見た目がデータどおり反映される（spec: rim_light / volumetric_fog）。
- evidence:
  - gameFog / gameGodRays / gameShadow / rim は anonymous namespace 内リテラル（SceneGame.cpp:35-73）
  - EditorAssets 配下に VolumetricFog 用 JSON プロファイル無し（volumetric_fog.md のレベル制作共有未実装）
- suggestedUse: プレイ・制作: 調整した見た目をデータとしてシーン間で再現
- roadmapLink: feature-memos/volumetric_fog.md
- firstSeenAt: 2026-11-10T01:01:00+09:00
- lastSeenAt: 2026-11-10T01:01:00+09:00

### GAP-Scene-CommandScene-001

- status: open
- tier: P2
- score: 680
- primaryUseCase: UC-Play
- scenarioId: PLY-Audio-001
- frequency: high
- frequencyBasis: Title/Game/Result 等すべて CommandScene::ChangeScene で BGM を ExitScene 時に停止
- scope: UC-Play|Scene|CommandScene|Incomplete|Project/Codes/Include/Scene/CommandScene.h:64
- primary: `CommandScene`
- summary: 【Play】シーン切替で BGM が瞬断し、クロスフェード／継続再生ポリシーが無い
- scenarioText: プレイ中に BGM／SE が途切れず再生され、シーン遷移後も期待どおり鳴り続ける。
- evidence:
  - ChangeScene で ExitScene→EnterScene。各 Scene*::ExitScene が StopBgmLoop（例: SceneGame.cpp:121）
  - ChangeSceneFade は約0.5s暗転後に ChangeScene。暗転中も旧 BGM は停止まで再生されるが切替瞬間に無音ギャップが生じうる
  - 遷移先 EnterScene で別 BGM を即 PlaySceneBgm。音量フェードや SE 同期の共通 API なし
- suggestedUse: ランタイム: シーン遷移後に期待どおり次 BGM へ移る（必要ならクロスフェード方針を仕様化）
- firstSeenAt: 2026-11-09T23:32:00+09:00
- lastSeenAt: 2026-11-09T23:32:00+09:00

### GAP-Scene-SceneBuildUp-001

- status: open
- tier: P2
- score: 670
- primaryUseCase: UC-Play
- secondaryUseCases: UC-Debug
- scenarioId: PLY-Camera-001
- frequency: medium
- frequencyBasis: BuildUp プレイでも stage カメラはロックされるが地形シルエットが本編と不一致
- scope: UC-Play|Scene|SceneBuildUp|Incomplete|Project/Codes/Source/Scene/SceneBuildUp.cpp:155
- primary: `SceneBuildUp`
- summary: 【Play】BuildUp 描画に地形越し自機シルエットが無い
- scenarioText: プレイ中カメラが地形・オブジェクトを見失わず、操作に追従する（spec: feature-memos/player_friendly_camera_policy.md）。
- evidence:
  - SceneGame::DrawEntities は PlayerTerrainSilhouette を呼ぶ（SceneGame.cpp:457-460）
  - SceneBuildUp::DrawRuntimeEntities / DrawScene は Entity::Draw のみ（SceneBuildUp.cpp:76-84, 155-168）
  - policy doc は SceneBuildUp も locked 本番相当だがシルエット補助が未共有
- suggestedUse: プレイ: 検証シーンでも本編同様に自機位置を視認できる
- roadmapLink: feature-memos/player_terrain_silhouette.md
- firstSeenAt: 2026-11-10T00:01:00+09:00
- lastSeenAt: 2026-11-10T00:01:00+09:00

### GAP-Render-SceneBuildUp-001

- status: open
- tier: P2
- score: 660
- primaryUseCase: UC-Play
- secondaryUseCases: UC-Debug
- scenarioId: PLY-Visual-001
- frequency: medium
- frequencyBasis: JSON シーンプレビューが本編と同じ描画パイプラインを使わない
- scope: UC-Play|Render|SceneBuildUp|Incomplete|Project/Codes/Source/Scene/SceneBuildUp.cpp:64
- primary: `SceneBuildUp`
- summary: 【Play】BuildUp ランタイム描画にフォグ・リム・ゴッドレイ・シャドウが無い
- scenarioText: ランタイムでライト・フォグ等の見た目がデータどおり反映される（spec: rim_light / volumetric_fog）。
- evidence:
  - SetupRuntimeLighting は DirectionalLightData のみ（SceneBuildUp.cpp:64-73）
  - RefVolumetricFog / RefGodRays / RefDirectionalShadow 未使用（SceneGame と対照）
- suggestedUse: プレイ: 制作プレビューで本編に近い見た目を確認
- firstSeenAt: 2026-11-10T01:01:00+09:00
- lastSeenAt: 2026-11-10T01:01:00+09:00

### GAP-Scene-SceneGame-002

- status: open
- tier: P2
- score: 650
- primaryUseCase: UC-Play
- scenarioId: PLY-SceneLoad-001
- frequency: medium
- frequencyBasis: EnterScene で UX JSON の成否がシーン安定性に影響しうるが未チェック
- scope: UC-Play|Scene|SceneGame|Unwired|Project/Codes/Source/Scene/SceneGame.cpp:111
- primary: `SceneGame`
- summary: 【Play】シーン入場時の operator_guidance 読込失敗を検知・代替しない
- scenarioText: プレイヤーがシーンを開き、主要オブジェクトとカメラが安定して動作する。
- evidence:
  - LoadFromJsonFile の戻り値未使用（SceneGame.cpp:111-112）
  - 相対パス ../EditorAssets/UX/... は CWD 依存（Ship 配布物とずれうる）
- suggestedUse: プレイ: シーン開始時に UI／案内が欠けてもカメラ・戦闘が破綻しない
- firstSeenAt: 2026-11-10T00:41:00+09:00
- lastSeenAt: 2026-11-10T00:41:00+09:00

### GAP-Scene-SceneMovie-002

- status: open
- tier: P2
- score: 640
- primaryUseCase: UC-Play
- scenarioId: PLY-Movie-001
- frequency: medium
- frequencyBasis: トラック JSON 失敗時に即リターンし「再生完了」体験にならない
- scope: UC-Play|Scene|SceneMovie|Incomplete|Project/Codes/Source/Scene/SceneMovie.cpp:106
- primary: `SceneMovie`
- summary: 【Play】トラック読込失敗時に初回 Update で即戻り（エラー表示なし）
- scenarioText: シーン内ムービー／カットシーンが再生完了まで破綻しない（spec: scene_movie.md）。
- evidence:
  - EnterScene で TryBeginFromJsonFile 失敗時は SetupStageViewCamera のみ（90-92 行）
  - trackPlaybackStarted_ false のまま Update が RequestReturnToCaller（106-108 行）
  - ユーザー向けログ／UI 無しでフェード復帰のみ
- suggestedUse: プレイ: 壊れたトラックでも原因が分かる／最低限のフォールバック演出
- firstSeenAt: 2026-11-10T00:21:00+09:00
- lastSeenAt: 2026-11-10T00:21:00+09:00

### GAP-Infra-verify_camera_policy-001

- status: open
- tier: P2
- score: 630
- primaryUseCase: UC-Play
- secondaryUseCases: UC-Ship
- scenarioId: PLY-Camera-001
- frequency: low
- frequencyBasis: 新シーン追加時のカメラ方針逸脱を CI で検知できない
- scope: UC-Play|Infra|verify_camera_policy|MissingTest|EditorAssets/Tools/StageViewCameraTuning/verify_camera_policy.py:1
- primary: `verify_camera_policy`
- summary: 【Play】カメラ方針静的検証が CI／ビルド手順に未組み込み
- scenarioText: プレイ中カメラが地形・オブジェクトを見失わず、操作に追従する（spec: feature-memos/player_friendly_camera_policy.md）。
- evidence:
  - verify_camera_policy.py は README と policy doc にのみ言及
  - .github ワークフローおよび buildDevelopment.cmd から未呼び出し（rg 確認）
- suggestedUse: ビルド・配布: 本番シーンのカメラロック漏れを自動検出
- firstSeenAt: 2026-11-10T00:01:00+09:00
- lastSeenAt: 2026-11-10T00:01:00+09:00

### GAP-Scene-SceneBuildUp-002

- status: open
- tier: P2
- score: 630
- primaryUseCase: UC-Play
- secondaryUseCases: UC-Authoring
- scenarioId: PLY-SceneLoad-001
- frequency: medium
- frequencyBasis: JSON シーンオープン失敗時に別内容が載る
- scope: UC-Play|Scene|SceneBuildUp|Incomplete|Project/Codes/Source/Scene/SceneBuildUp.cpp:92
- primary: `SceneBuildUp`
- summary: 【Play】シーン JSON 読込失敗時にデフォルト 1 体へ置換しプレイヤーに通知しない
- scenarioText: プレイヤーがシーンを開き、主要オブジェクトとカメラが安定して動作する。
- evidence:
  - TryLoadSceneDocument 失敗時 MakeDefaultEntityDefinition で代替（92-98 行）
  - カメラは SetupStageViewCamera するが主要オブジェクト構成が保存内容と不一致
- suggestedUse: プレイ: 開いたシーンが意図どおりのオブジェクト構成である
- firstSeenAt: 2026-11-10T00:41:00+09:00
- lastSeenAt: 2026-11-10T00:41:00+09:00

### GAP-Scene-Scene-001

- status: open
- tier: P2
- score: 600
- primaryUseCase: UC-Play
- scenarioId: PLY-Camera-001
- frequency: medium
- frequencyBasis: Scene 基底が unlocked 既定のため新規プレイシーンでユーザー操作カメラが有効化されうる
- scope: UC-Play|Scene|Scene|Contract|Project/Codes/Include/Scene/Scene.h:65
- primary: `Scene`
- summary: 【Play】新規シーンのステージカメラロックはドキュメント依存
- scenarioText: プレイ中カメラが地形・オブジェクトを見失わず、操作に追従する（spec: feature-memos/player_friendly_camera_policy.md）。
- evidence:
  - Scene::IsStageViewCameraLocked 既定は false（Scene.h:65）
  - GameOrder は false のとき Camera::Update でユーザー操作を許す（GameOrder.h:66-67）
  - policy のチェックリストは手動で override を要求するのみ
- suggestedUse: プレイ: 意図しないフリーカメラを防ぐ
- roadmapLink: feature-memos/player_friendly_camera_policy.md
- firstSeenAt: 2026-11-10T00:01:00+09:00
- lastSeenAt: 2026-11-10T00:01:00+09:00

### GAP-Scene-SceneMovie-001

- status: investigating
- tier: P2
- score: 540
- primaryUseCase: UC-Play
- scenarioId: PLY-Audio-001
- frequency: low
- frequencyBasis: scene_movie 利用シーンが限定的だが、ムービー中の BGM/SE 継続要件は未整理
- scope: UC-Play|Scene|SceneMovie|MissingData|Project/Codes/Source/Scene/SceneMovie.cpp:77
- primary: `SceneMovie`
- summary: 【Play】ムービーシーンに BGM/トラック音声の再生・復帰ポリシーがコード上見えない
- scenarioText: プレイ中に BGM／SE が途切れず再生され、シーン遷移後も期待どおり鳴り続ける。
- evidence:
  - SceneMovie::EnterScene はカメラトラック開始のみ（SceneMovie.cpp:77-88）。BGM 制御なし
  - 遷移元 ExitScene で BGM 停止後、ムービー終了→returnScene へフェード復帰。復帰先 BGM の再開手順はシーン個別実装依存
- suggestedUse: ランタイム: カットシーン前後で音体験が破綻しないよう、ムービー用オーディオ方針を決める
- roadmapLink: feature-memos/scene_movie.md
- firstSeenAt: 2026-11-09T23:32:00+09:00
- lastSeenAt: 2026-11-09T23:32:00+09:00

### GAP-Runtime-MovieCameraTrackLoader-001

- status: open
- tier: P3
- score: 520
- primaryUseCase: UC-Play
- secondaryUseCases: UC-Ship
- scenarioId: PLY-Movie-001
- frequency: low
- frequencyBasis: 同梱 sample_movie_track.json のロード成否を CI で見ていない
- scope: UC-Play|Runtime|MovieCameraTrackLoader|MissingTest|Project/Codes/Source/Camera/MovieCameraTrackLoader.cpp:34
- primary: `MovieCameraTrackLoader`
- summary: 【Play】ムービートラック JSON のスモーク検証が自動化されていない
- scenarioText: シーン内ムービー／カットシーンが再生完了まで破綻しない（spec: scene_movie.md）。
- evidence:
  - LoadMovieCameraTrackFromJson は keyframes>=2 を要求（MovieCameraTrackLoader.cpp:69）
  - sample_movie_track.json の CI 読込チェック／ワークフロー呼び出し無し
- suggestedUse: 配布: カットシーンアセット破損をマージ前に検知
- roadmapLink: feature-memos/movie_camera_controller.md
- firstSeenAt: 2026-11-10T00:21:00+09:00
- lastSeenAt: 2026-11-10T00:21:00+09:00

### GAP-Scene-SceneMovie-003

- status: open
- tier: P3
- score: 520
- primaryUseCase: UC-Play
- scenarioId: PLY-Visual-001
- frequency: low
- frequencyBasis: カットシーンでもリム／フォグ拡張を使う方針だが未適用
- scope: UC-Play|Scene|SceneMovie|Incomplete|Project/Codes/Source/Scene/SceneMovie.cpp:43
- primary: `SceneMovie`
- summary: 【Play】SceneMovie のライティングにリム・ボリュームフォグが無い
- scenarioText: ランタイムでライト・フォグ等の見た目がデータどおり反映される（spec: rim_light / volumetric_fog）。
- evidence:
  - SetupStageLighting は平行光のみ（SceneMovie.cpp:43-52）
  - rim_light.md は SceneMovie 等でも DirectionalLightData 拡張利用可と記載
- suggestedUse: プレイ: カットシーンのシルエット可読性を本編と揃える
- roadmapLink: feature-memos/rim_light.md
- firstSeenAt: 2026-11-10T01:01:00+09:00
- lastSeenAt: 2026-11-10T01:01:00+09:00

#### UC-Ship

### GAP-Ship-CG2_BuildUp-001

- status: open
- tier: P0
- score: 900
- primaryUseCase: UC-Ship
- secondaryUseCases: UC-Play
- scenarioId: SHP-AssetPack-001
- frequency: high
- frequencyBasis: Release PostBuild が Project/Assets のみ同梱しランタイムが EditorAssets も参照
- scope: UC-Ship|Ship|CG2_BuildUp|Incomplete|Project/CG2_BuildUp.vcxproj:109
- primary: `CG2_BuildUp`
- summary: 【Ship】PostBuild が Assets のみコピーしランタイム参照の EditorAssets が同梱されない
- scenarioText: 配布に必要なアセットがパイプラインから漏れず同梱される。
- evidence:
  - Development/Release とも PostBuild は xcopy ProjectDir Assets のみ（vcxproj:109-151）
  - SceneGame は ../EditorAssets/UX/operator_guidance.json を本編で読む
  - ImGui 日本語フォントは ../EditorAssets/Fonts/... フォールバックあり（Release でも Fonts が Assets に無いと失敗しうる）
- suggestedUse: 配布: exe 単体配布でも本編 UX・フォントが欠けない
- firstSeenAt: 2026-11-10T01:21:00+09:00
- lastSeenAt: 2026-11-10T01:21:00+09:00

### GAP-Ship-CG2_BuildUp-002

- status: open
- tier: P1
- score: 800
- primaryUseCase: UC-Ship
- scenarioId: SHP-Config-001
- frequency: high
- frequencyBasis: Debug 構成が DEVELOPMENT+USE_IMGUI を本番相当 Release と同列に混在
- scope: UC-Ship|Ship|CG2_BuildUp|Contract|Project/CG2_BuildUp.vcxproj:85
- primary: `CG2_BuildUp`
- summary: 【Ship】Debug 構成も DEVELOPMENT+USE_IMGUI で本番用コンパイル差分が不明瞭
- scenarioText: 本番と開発で意図したコンパイル／リンク差分のみが有効になる。
- evidence:
  - Debug|x64: _DEBUG+DEVELOPMENT+USE_IMGUI（85 行）
  - Release|x64: NDEBUG のみ（DEVELOPMENT/USE_IMGUI 無し）
  - Development|x64: DEVELOPMENT+USE_IMGUI だが NDEBUG/_DEBUG どちらも未定義
- suggestedUse: 配布: CI／手順で検証する「出荷候補」構成が一意に定義される
- firstSeenAt: 2026-11-10T01:31:00+09:00
- lastSeenAt: 2026-11-10T01:31:00+09:00

### GAP-Infra-buildDevelopment-001

- status: open
- tier: P1
- score: 790
- primaryUseCase: UC-Ship
- secondaryUseCases: UC-Debug
- scenarioId: SHP-DevBuild-001
- frequency: high
- frequencyBasis: ローカル既定ビルドが CI と前段検証・ソリューション入口が一致しない
- scope: UC-Ship|Infra|buildDevelopment|Incomplete|EditorAssets/Tools/CursorAI/buildDevelopment.cmd:18
- primary: `buildDevelopment`
- summary: 【Ship】buildDevelopment.cmd が CI と異なる入口で前検証も無い
- scenarioText: `Development | x64` ビルドが CI／ローカル手順で再現可能に成功する（spec: `feature-memos/cloud_build_vcxproj_check.md`）。
- evidence:
  - ローカル: CG2_BuildUp.vcxproj を直接 MSBuild（buildDevelopment.cmd:18）
  - CI: verify_vcxproj_coverage の後に CG2_BuildUp.slnx を Development ビルド（DevelopmentBuild.yml:19,103-106）
  - buildDevelopment.cmd は verify 呼び出し無し
- suggestedUse: 制作: ローカル成功が CI Development ビルドと同等の手順になる
- roadmapLink: feature-memos/cloud_build_vcxproj_check.md
- firstSeenAt: 2026-11-10T01:41:00+09:00
- lastSeenAt: 2026-11-10T01:41:00+09:00

### GAP-Infra-Release_smoke-001

- status: open
- tier: P1
- score: 780
- primaryUseCase: UC-Ship
- scenarioId: SHP-Smoke-001
- frequency: high
- frequencyBasis: Release CI は MSBuild 成功のみで起動スモーク無し
- scope: UC-Ship|Infra|Release_smoke|MissingTest|.github/workflows/ReleaseBuild.yml:100
- primary: `Release_smoke`
- summary: 【Ship】Release ビルド後の exe 起動スモークが CI に無い
- scenarioText: 最小シーンのスモーク起動が配布ビルドで通る（自動化未整備でも手順が文書化されている）。
- evidence:
  - ReleaseBuild.yml は Build Release ステップで終了（100-111 行）
  - 実行ファイル起動・数フレーム継続のワークフロー／スクリプト無し
- suggestedUse: 配布: マージ前に Title シーンまでの最小起動を自動検知
- firstSeenAt: 2026-11-10T02:01:00+09:00
- lastSeenAt: 2026-11-10T02:01:00+09:00

### GAP-Ship-EditorSources-001

- status: open
- tier: P1
- score: 770
- primaryUseCase: UC-Ship
- scenarioId: SHP-Config-001
- frequency: high
- frequencyBasis: Release でも Editor 系 TU がリンク対象のまま
- scope: UC-Ship|Ship|EditorSources|Incomplete|Project/CG2_BuildUp.vcxproj:325
- primary: `EditorSources`
- summary: 【Ship】Release でも Editor 系ソースが ExcludedFromBuild されずリンクされる
- scenarioText: 本番と開発で意図したコンパイル／リンク差分のみが有効になる。
- evidence:
  - imgui 系は Release|x64 で ExcludedFromBuild=true（406 行付近）
  - EntityEditor / SceneEntityEditor 等 Editor 配下は Release 除外設定無し（325 行付近）
  - #if DEVELOPMENT ガードで実体は空でもオブジェクトファイルは Release に含まれる
- suggestedUse: 配布: 本番バイナリからエディタ用コードパスをリンク単位で除去
- firstSeenAt: 2026-11-10T01:31:00+09:00
- lastSeenAt: 2026-11-10T01:31:00+09:00

### GAP-Infra-asset_pack_verify-001

- status: open
- tier: P1
- score: 760
- primaryUseCase: UC-Ship
- scenarioId: SHP-AssetPack-001
- frequency: high
- frequencyBasis: コード内の相対パス参照と PostBuild 同梱範囲の突合が自動化されていない
- scope: UC-Ship|Infra|asset_pack_verify|MissingTest|EditorAssets/Tools/CursorAI/verify_vcxproj_coverage.py:1
- primary: `asset_pack_verify`
- summary: 【Ship】同梱アセットとランタイム参照パスの整合を検証する仕組みが無い
- scenarioText: 配布に必要なアセットがパイプラインから漏れず同梱される。
- evidence:
  - CI は verify_vcxproj_coverage（ソース列挙）のみでアセットパス検証無し
  - PlaySe 等は ./Assets と ./Resources の二重候補だが Project/Resources ディレクトリ自体が無い
- suggestedUse: 配布: ビルド成果物ディレクトリに参照パスが全て存在することを PR 前に確認
- firstSeenAt: 2026-11-10T01:21:00+09:00
- lastSeenAt: 2026-11-10T01:21:00+09:00

### GAP-Infra-buildDevelopment-002

- status: open
- tier: P1
- score: 760
- primaryUseCase: UC-Ship
- scenarioId: SHP-DevBuild-001
- frequency: high
- frequencyBasis: 初回クローンで NuGet 未復元だとローカル Development ビルドが失敗しうる
- scope: UC-Ship|Infra|buildDevelopment|Unwired|EditorAssets/Tools/CursorAI/buildDevelopment.cmd:1
- primary: `buildDevelopment`
- summary: 【Ship】buildDevelopment.cmd に NuGet 復元ステップが無い
- scenarioText: `Development | x64` ビルドが CI／ローカル手順で再現可能に成功する（spec: `feature-memos/cloud_build_vcxproj_check.md`）。
- evidence:
  - DevelopmentBuild.yml は nuget restore と SDL2 targets 存在確認を実施（44-98 行）
  - buildDevelopment.cmd は MSBuild 起動のみ
- suggestedUse: 制作: 半自律手順の buildDevelopment がクリーン環境でも通る
- firstSeenAt: 2026-11-10T01:41:00+09:00
- lastSeenAt: 2026-11-10T01:41:00+09:00

### GAP-Ship-smoke_runbook-001

- status: open
- tier: P1
- score: 750
- primaryUseCase: UC-Ship
- scenarioId: SHP-Smoke-001
- frequency: high
- frequencyBasis: 配布ビルドの手動スモーク手順がリポジトリに無い
- scope: UC-Ship|Ship|smoke_runbook|MissingData|README.md:1
- primary: `smoke_runbook`
- summary: 【Ship】Release 向け最小起動スモークの手順書が無い
- scenarioText: 最小シーンのスモーク起動が配布ビルドで通る（自動化未整備でも手順が文書化されている）。
- evidence:
  - ルート README は CI バッジのみ（起動手順無し）
  - use_case_scenarios SHP-Smoke-001 は「手順が文書化」とあるが該当 md 無し
  - DebugPlayArea README は Development 向けのみ
- suggestedUse: 配布: 人手でも Title 表示までの確認手順を共有
- firstSeenAt: 2026-11-10T02:01:00+09:00
- lastSeenAt: 2026-11-10T02:01:00+09:00

### GAP-Ship-ForDebugSources-001

- status: open
- tier: P1
- score: 740
- primaryUseCase: UC-Ship
- scenarioId: SHP-Release-001
- frequency: high
- frequencyBasis: ForDebug 系ソースが imgui と同様に Release 除外されていない
- scope: UC-Ship|Ship|ForDebugSources|Incomplete|Project/CG2_BuildUp.vcxproj:283
- primary: `ForDebugSources`
- summary: 【Ship】ForDebug 系 cpp が Release でコンパイル・リンクされる
- scenarioText: Release 相当設定で不要なデバッグ UI・開発専用コードパスが出荷物に漏れない。
- evidence:
  - ForDebug 配下 ClCompile は ExcludedFromBuild 設定無し（283-286 行付近）
  - imgui 系のみ Release|x64 で除外されている
- suggestedUse: 配布: デバッグカメラ／ストリーミングデバッグ UI 用コードを出荷物から分離
- firstSeenAt: 2026-11-10T01:51:00+09:00
- lastSeenAt: 2026-11-10T01:51:00+09:00

### GAP-Ship-CG2_BuildUp-003

- status: open
- tier: P2
- score: 720
- primaryUseCase: UC-Ship
- scenarioId: SHP-Release-001
- frequency: medium
- frequencyBasis: Release でも PDB 生成が有効で出荷物にデバッグ情報が残る
- scope: UC-Ship|Ship|CG2_BuildUp|Incomplete|Project/CG2_BuildUp.vcxproj:140
- primary: `CG2_BuildUp`
- summary: 【Ship】Release リンク設定で GenerateDebugInformation が true
- scenarioText: Release 相当設定で不要なデバッグ UI・開発専用コードパスが出荷物に漏れない。
- evidence:
  - Release|x64 Link: GenerateDebugInformation=true（vcxproj:140）
  - Development も同設定で意図差分の文書化無し
- suggestedUse: 配布: 出荷 exe に意図したデバッグシンボル方針を適用
- firstSeenAt: 2026-11-10T01:51:00+09:00
- lastSeenAt: 2026-11-10T01:51:00+09:00

### GAP-Infra-verify_config_matrix-001

- status: open
- tier: P2
- score: 700
- primaryUseCase: UC-Ship
- scenarioId: SHP-Config-001
- frequency: medium
- frequencyBasis: 構成マクロと ExcludedFromBuild の期待値が CI で検証されない
- scope: UC-Ship|Infra|verify_config_matrix|MissingTest|EditorAssets/Tools/CursorAI/verify_vcxproj_coverage.py:1
- primary: `verify_config_matrix`
- summary: 【Ship】コンパイル定義と Release 除外の期待マトリクス検証が無い
- scenarioText: 本番と開発で意図したコンパイル／リンク差分のみが有効になる。
- evidence:
  - verify_vcxproj_coverage.py は .cpp 列挙のみ（cloud_build_vcxproj_check.md）
  - PreprocessorDefinitions / ExcludedFromBuild の正本ドキュメント・自動突合無し
- suggestedUse: 配布: Release ビルドが意図外マクロや Editor TU を含んだら CI で失敗
- roadmapLink: feature-memos/cloud_build_vcxproj_check.md
- firstSeenAt: 2026-11-10T01:31:00+09:00
- lastSeenAt: 2026-11-10T01:31:00+09:00

### GAP-Ship-JsonTest-001

- status: open
- tier: P2
- score: 690
- primaryUseCase: UC-Ship
- scenarioId: SHP-Release-001
- frequency: low
- frequencyBasis: Json デバッグ用 TU が Release にも含まれる
- scope: UC-Ship|Ship|JsonTest|Incomplete|Project/Codes/Source/System/Json/JsonTest.cpp:1
- primary: `JsonTest`
- summary: 【Ship】JsonTest デバッグパネル用 TU が Release ビルドから除外されていない
- scenarioText: Release 相当設定で不要なデバッグ UI・開発専用コードパスが出荷物に漏れない。
- evidence:
  - JsonTest.cpp は DEVELOPMENT/USE_IMGUI ガードだが vcxproj に Release ExcludedFromBuild 無し（394 行）
  - ImGui 用 jsonTest ウィンドウ実装が同一 TU に存在
- suggestedUse: 配布: 本番バイナリから Json 実験用デバッグ UI コードを除去
- firstSeenAt: 2026-11-10T01:51:00+09:00
- lastSeenAt: 2026-11-10T01:51:00+09:00

### GAP-Infra-BinaryConverter-001

- status: open
- tier: P2
- score: 680
- primaryUseCase: UC-Ship
- secondaryUseCases: UC-Authoring
- scenarioId: SHP-AssetPack-001
- frequency: medium
- frequencyBasis: OBJ→amesh 変換が手動で同梱物に .amesh が無い
- scope: UC-Ship|Infra|BinaryConverter|MissingTest|EditorAssets/Tools/BinaryConverter/README.md:21
- primary: `BinaryConverter`
- summary: 【Ship】メッシュ変換パイプラインのバッチ／CI フックが未整備
- scenarioText: 配布に必要なアセットがパイプラインから漏れず同梱される。
- evidence:
  - BinaryConverter README「今後: バッチ変換用 cmd / CI フック」（21 行）
  - リポジトリに .amesh 無し、vcxproj PreBuild に変換ステップ無し
- suggestedUse: 配布: 制作 OBJ 変更がランタイム向けバイナリへ漏れなく反映される
- roadmapLink: feature-memos/atr_mesh_binary_format.md
- firstSeenAt: 2026-11-10T01:21:00+09:00
- lastSeenAt: 2026-11-10T01:21:00+09:00

### GAP-Infra-cloud_dev_build-001

- status: open
- tier: P2
- score: 680
- primaryUseCase: UC-Ship
- scenarioId: SHP-DevBuild-001
- frequency: medium
- frequencyBasis: Cloud Agent 環境は Linux で Win32 MSBuild が使えない
- scope: UC-Ship|Infra|cloud_dev_build|MissingTest|EditorAssets/Tools/CursorAI/cursorInstructions.md:31
- primary: `cloud_dev_build`
- summary: 【Ship】Cloud Linux 上で Development ビルド再現手段が未定義
- scenarioText: `Development | x64` ビルドが CI／ローカル手順で再現可能に成功する（spec: `feature-memos/cloud_build_vcxproj_check.md`）。
- evidence:
  - 半自律手順は buildDevelopment.cmd 成功を必須（cursorInstructions.md:265）
  - verify_vcxproj_coverage のみ Linux 実行可（cloud_build_vcxproj_check.md）
  - progress_report に Cloud Linux で buildDevelopment 未実施の記録あり
- suggestedUse: エージェント開発: Linux Cloud でも CI 同等の妥当性チェックを完了とみなせる
- roadmapLink: feature-memos/cloud_build_vcxproj_check.md
- firstSeenAt: 2026-11-10T01:41:00+09:00
- lastSeenAt: 2026-11-10T01:41:00+09:00

### GAP-Ship-main-001

- status: open
- tier: P2
- score: 670
- primaryUseCase: UC-Ship
- secondaryUseCases: UC-Debug
- scenarioId: SHP-Smoke-001
- frequency: medium
- frequencyBasis: 自動スモーク用のコマンドライン／フレーム上限モードが無い
- scope: UC-Ship|Ship|main|MissingTest|Project/Codes/Source/Application/main.cpp:5
- primary: `main`
- summary: 【Ship】スモーク起動用 CLI（例: フレーム数上限）が main に無い
- scenarioText: 最小シーンのスモーク起動が配布ビルドで通る（自動化未整備でも手順が文書化されている）。
- evidence:
  - WinMain は引数未使用で無限ループのみ（main.cpp:5-23）
  - CI から非対話で数フレーム実行する公式フック無し
- suggestedUse: 配布・CI: ウィンドウ表示後 N フレームで正常終了するスモークを自動化
- firstSeenAt: 2026-11-10T02:01:00+09:00
- lastSeenAt: 2026-11-10T02:01:00+09:00


---

## Archive

なし

---

## スキャン履歴（直近）

- scanId: SCAN-20261109-234011 | at: 2026-11-09T23:40:39.226713+09:00 | scenario: AUT-AssetLoop-001 | new: 3 | updated: 0
- scanId: SCAN-20261109-235016 | at: 2026-11-09T23:50:37.574616+09:00 | scenario: DBG-FeatureScene-001 | new: 3 | updated: 0
- scanId: SCAN-20261110-000016 | at: 2026-11-10T00:00:39.746375+09:00 | scenario: PLY-Camera-001 | new: 3 | updated: 0
- scanId: SCAN-20261110-001015 | at: 2026-11-10T00:10:39.912798+09:00 | scenario: AUT-Component-001 | new: 3 | updated: 0
- scanId: SCAN-20261110-002012 | at: 2026-11-10T00:20:31.379655+09:00 | scenario: PLY-Movie-001 | new: 3 | updated: 0
- scanId: SCAN-20261110-003022 | at: 2026-11-10T00:30:41.073675+09:00 | scenario: AUT-EditorPlace-001 | new: 3 | updated: 0
- scanId: SCAN-20261110-004013 | at: 2026-11-10T00:40:44.827867+09:00 | scenario: PLY-SceneLoad-001 | new: 3 | updated: 0
- scanId: SCAN-20261110-005019 | at: 2026-11-10T00:50:37.082956+09:00 | scenario: AUT-Guidance-001 | new: 3 | updated: 0
- scanId: SCAN-20261110-010019 | at: 2026-11-10T01:01:03.761970+09:00 | scenario: PLY-Visual-001 | new: 3 | updated: 0
- scanId: SCAN-20261110-011015 | at: 2026-11-10T01:10:32.331404+09:00 | scenario: AUT-VisualTune-001 | new: 3 | updated: 0
- scanId: SCAN-20261110-012008 | at: 2026-11-10T01:20:33.060096+09:00 | scenario: SHP-AssetPack-001 | new: 3 | updated: 0
- scanId: SCAN-20261110-013008 | at: 2026-11-10T01:30:31.017847+09:00 | scenario: SHP-Config-001 | new: 3 | updated: 0
- scanId: SCAN-20261110-014010 | at: 2026-11-10T01:40:32.359685+09:00 | scenario: SHP-DevBuild-001 | new: 3 | updated: 0
- scanId: SCAN-20261110-015009 | at: 2026-11-10T01:50:42.672348+09:00 | scenario: SHP-Release-001 | new: 3 | updated: 0
- scanId: SCAN-20261110-020007 | at: 2026-11-10T02:00:32.963377+09:00 | scenario: SHP-Smoke-001 | new: 3 | updated: 0

---

## エントリテンプレ（コピー用）

```markdown
### GAP-Streaming-ExampleType-001

- status: open
- tier: P2
- score: 0
- primaryUseCase: UC-Debug
- secondaryUseCases: （任意・最大2）
- scenarioId: DBG-Streaming-001
- frequency: unknown
- frequencyBasis: （UseCase 視点で 1 行）
- scope: UC-Debug | Streaming | ExampleType | Unwired | （パス:行 または Symbol:Name）
- primary: `ExampleType`
- related: （任意）
- summary: 【Debug】（1 行・必須型）
- scenarioText: （誰が・何のために・どう困るか）
- evidence:
  - （箇条書き）
- suggestedUse: （§6.4 補足）
- roadmapLink: （任意）
- firstSeenAt: （JST）
- lastSeenAt: （JST）
```
