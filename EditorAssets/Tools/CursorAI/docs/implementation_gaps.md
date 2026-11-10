# 実装ギャップ一覧（用途別・正本）

人間が読む正本。機械による重複判定・マージの正本は **`implementation_gaps_index.json`**。

<!-- gaps-schema: 2 -->

## メタ

- **schemaVersion:** 2
- **lastFullScanAt:** （未実施）
- **lastIncrementalScanAt:** 2026-11-11T04:30:30.823712+09:00
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

### GAP-Editor-SceneEntityEditor-002

- status: open
- tier: P1
- score: 790
- primaryUseCase: UC-Authoring
- scenarioId: AUT-EditorPlace-001
- frequency: high
- frequencyBasis: 3D ビューで配置操作ができず数値入力のみ
- scope: UC-Authoring|Editor|SceneEntityEditor|MissingUI|Project/Codes/Source/Scene/SceneEntityEditor.cpp:256
- primary: `SceneEntityEditor`
- summary: 【Authoring】エディットビューに配置用ギズモ／ドラッグ操作が無い
- scenarioText: 制作者がエディタ上でオブジェクトを配置・保存し、再度開いて同じ状態を得られる。
- evidence:
  - エディットパスは床グリッド＋プレビュー描画のみ（256-260 行）
  - 配置はインスペクタの初期トランスフォーム ImGui のみ（EntityEditorUi.cpp:615-631）
  - Editor 配下に Gizmo／TransformManip 等の参照なし
- suggestedUse: 制作: ビューポートで直感的にオブジェクトを置く
- firstSeenAt: 2026-11-10T03:51:00+09:00
- lastSeenAt: 2026-11-10T03:51:00+09:00

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

### GAP-Editor-EditorSceneRuntime-003

- status: open
- tier: P1
- score: 770
- primaryUseCase: UC-Authoring
- scenarioId: AUT-Component-001
- frequency: medium
- frequencyBasis: 方針メモの CmpMesh 等が UI から明示追加できない
- scope: UC-Authoring|Editor|EditorSceneRuntime|MissingUI|Project/Codes/Source/System/Editor/Scene/EditorSceneRuntime.cpp:67
- primary: `EditorSceneRuntime`
- summary: 【Authoring】CmpMesh／Billboard が TryAddEmptyComponent に無く分割例を直組みできない
- scenarioText: エンティティ／コンポント分割方針に沿ってアセットを組み立てられる（spec: component_split_guidelines.md）。
- evidence:
  - component_split_guidelines は CmpMesh／CmpEditorTextureBillboard を分割例に記載（15-16 行）
  - TryAddEmptyComponent は 6 型のみ（67-94 行）、Mesh は AssetPath 副作用で付与（253-270 行）
- suggestedUse: 制作: 描画コンポーネントを方針どおり個別スロットで組む
- roadmapLink: feature-memos/component_split_guidelines.md
- firstSeenAt: 2026-11-10T03:31:00+09:00
- lastSeenAt: 2026-11-10T03:31:00+09:00

### GAP-Editor-SceneEntityEditor-004

- status: open
- tier: P1
- score: 770
- primaryUseCase: UC-Authoring
- scenarioId: AUT-VisualTune-001
- frequency: high
- frequencyBasis: 全シーンビューが簡易平行光のみでポスト無し
- scope: UC-Authoring|Editor|SceneEntityEditor|Incomplete|Project/Codes/Source/Scene/SceneEntityEditor.cpp:251
- primary: `SceneEntityEditor`
- summary: 【Authoring】エディタプレビューにフォグ／リム／ゴッドレイ／影が無い
- scenarioText: 制作者がライト・カメラ・ポストのパラメータを編集し、プレビューで結果を確認できる。
- evidence:
  - DrawSceneViewContent は各タブで SetupEditorLighting のみ（251 行）
  - Game タブは SetupStageViewCamera 後も平行光のみ（241-251 行）で RefVolumetricFog 未使用
- suggestedUse: 制作: 本編に近い見た目をエディタ各ビューで確認
- roadmapLink: feature-memos/volumetric_fog.md
- firstSeenAt: 2026-11-10T04:31:00+09:00
- lastSeenAt: 2026-11-10T04:31:00+09:00

### GAP-Editor-EntityEditor-004

- status: open
- tier: P1
- score: 760
- primaryUseCase: UC-Authoring
- scenarioId: AUT-EditorPlace-001
- frequency: high
- frequencyBasis: 配置・トランスフォーム編集のたびに手動保存が必要
- scope: UC-Authoring|Editor|EntityEditor|Incomplete|Project/Codes/Source/System/Editor/Entity/EntityEditorUi.cpp:766
- primary: `EntityEditor`
- summary: 【Authoring】インスペクタ変更はプレビューのみ更新し自動保存しない
- scenarioText: 制作者がエディタ上でオブジェクトを配置・保存し、再度開いて同じ状態を得られる。
- evidence:
  - DrawInspectorPanel の changed は requestApplyPreview のみ（766-768 行）
  - ディスク保存はメニュー「保存」等の requestSaveToDisk 経由のみ（EditorEntityWorkbench.cpp:64-65）
- suggestedUse: 制作: 配置後に保存忘れで再オープン時に座標が戻るのを防ぐ
- firstSeenAt: 2026-11-10T03:51:00+09:00
- lastSeenAt: 2026-11-10T03:51:00+09:00

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

### GAP-Editor-EditorSceneRuntime-005

- status: open
- tier: P1
- score: 710
- primaryUseCase: UC-Authoring
- scenarioId: AUT-Component-001
- frequency: high
- frequencyBasis: 1 つの Data スロットが複数 Draw を暗黙生成
- scope: UC-Authoring|Editor|EditorSceneRuntime|Contract|Project/Codes/Source/System/Editor/Scene/EditorSceneRuntime.cpp:219
- primary: `EditorSceneRuntime`
- summary: 【Authoring】AssetPath プレビューが Draw 分割を Data に内包
- scenarioText: エンティティ／コンポント分割方針に沿ってアセットを組み立てられる（spec: component_split_guidelines.md）。
- evidence:
  - TryLoadEditorAssetPathPreview が obj と画像を同一 Data から付与（219-275 行）
  - ガイドラインは obj／png 用途別 Data 分割を推奨（9-11 行）
  - JSON components に CmpMesh スロットが残らない
- suggestedUse: 制作: Data と Draw を独立スロットで保存・再利用
- roadmapLink: feature-memos/component_split_guidelines.md
- firstSeenAt: 2026-11-10T05:21:00+09:00
- lastSeenAt: 2026-11-10T05:21:00+09:00

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

### GAP-Editor-EntityEditorShortcuts-001

- status: open
- tier: P2
- score: 700
- primaryUseCase: UC-Authoring
- scenarioId: AUT-AssetLoop-001
- frequency: high
- frequencyBasis: 複製・貼り付けの反復にマウス操作のみ
- scope: UC-Authoring|Editor|EntityEditorShortcuts|MissingUI|Project/Codes/Source/System/Editor/Entity/EntityEditorUi.cpp:364
- primary: `EntityEditorShortcuts`
- summary: 【Authoring】エンティティ複製用キーボードショートカットが無い
- scenarioText: 反復作業（複製・一括調整）がエディタ専用 UI で過度な手順にならない。
- evidence:
  - Editor Entity 配下に Ctrl+D / 複製 / Paste 処理無し（rg Duplicate|Ctrl 0 件）
  - ヒエラルキーはボタン追加・削除のみ（EntityEditorUi.cpp:374-379）
- suggestedUse: 制作: 配置の反復をキーボード中心で高速化
- firstSeenAt: 2026-11-10T03:01:00+09:00
- lastSeenAt: 2026-11-10T03:01:00+09:00

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

### GAP-Editor-EditorEntityWorkbench-002

- status: open
- tier: P2
- score: 690
- primaryUseCase: UC-Authoring
- scenarioId: AUT-VisualTune-001
- frequency: high
- frequencyBasis: ビジュアル調整用のメニュー／パネル入口が無い
- scope: UC-Authoring|Editor|EditorEntityWorkbench|MissingUI|Project/Codes/Source/System/Editor/EditorEntityWorkbench.cpp:128
- primary: `EditorEntityWorkbench`
- summary: 【Authoring】メニューにライト・ポスト調整ウィンドウが無い
- scenarioText: 制作者がライト・カメラ・ポストのパラメータを編集し、プレビューで結果を確認できる。
- evidence:
  - 「ビュー」はタブ切替のみ（128-147 行）
  - 「デバッグ」はストリーミングメモリのみで VolumetricFog 等の調整 UI 無し
- suggestedUse: 制作: 調整パネルをエディタから開きプレビューと連動
- firstSeenAt: 2026-11-10T04:31:00+09:00
- lastSeenAt: 2026-11-10T04:31:00+09:00

### GAP-Editor-EntityEditorDefinition-003

- status: open
- tier: P2
- score: 690
- primaryUseCase: UC-Authoring
- scenarioId: AUT-Component-001
- frequency: medium
- frequencyBasis: 未知 typeName の kind が Draw 既定で Upd/Data 誤配置を隠す
- scope: UC-Authoring|Editor|EntityEditorDefinition|Contract|Project/Codes/Source/System/Editor/Entity/EntityEditorDefinition.cpp:34
- primary: `EntityEditorDefinition`
- summary: 【Authoring】未登録コンポーネント種別が Draw 扱いになる
- scenarioText: エンティティ／コンポント分割方針に沿ってアセットを組み立てられる（spec: component_split_guidelines.md）。
- evidence:
  - DefaultKindForTypeName のフォールバックは ComponentSlotKind::Draw（34 行）
  - JSON 手編集の未知 typeName が Load 時に黙って Draw スロット化
- suggestedUse: 制作: Draw/Upd/Data 誤りをロード時に検出
- firstSeenAt: 2026-11-10T03:31:00+09:00
- lastSeenAt: 2026-11-10T03:31:00+09:00

### GAP-Editor-EditorFrame-001

- status: open
- tier: P2
- score: 680
- primaryUseCase: UC-Authoring
- scenarioId: AUT-VisualTune-001
- frequency: high
- frequencyBasis: シーンビュー FOV が tuning JSON と無関係な定数
- scope: UC-Authoring|Editor|EditorFrame|Contract|Project/Codes/Source/System/Editor/EditorFrame.cpp:129
- primary: `EditorFrame`
- summary: 【Authoring】FitCameraPerspectiveToViewport が FOV 0.5 固定
- scenarioText: 制作者がライト・カメラ・ポストのパラメータを編集し、プレビューで結果を確認できる。
- evidence:
  - PerspectiveFov(0.5f, aspect, ...)（129 行）
  - tuning JSON の perspectiveFovY（stage_view_camera_tuning.json:4）未参照
  - SceneEntityEditor / SceneBuildUp プレビューが共通利用（249, 148 行）
- suggestedUse: 制作: カメラデータを変えてプレビューに反映
- roadmapLink: feature-memos/odyssey_style_stage_camera.md
- firstSeenAt: 2026-11-10T06:21:00+09:00
- lastSeenAt: 2026-11-10T06:21:00+09:00

### GAP-Editor-EditorImportedAsset-001

- status: open
- tier: P2
- score: 680
- primaryUseCase: UC-Authoring
- scenarioId: AUT-AssetLoop-001
- frequency: high
- frequencyBasis: ドロップ後にアセットをエンティティへ載せる手順が別作業
- scope: UC-Authoring|Editor|EditorImportedAsset|MissingUI|Project/Codes/Source/System/Editor/Entity/EntityEditorUi.cpp:411
- primary: `EditorImportedAsset`
- summary: 【Authoring】インポート直後パス表示のみで一括適用 UI が無い
- scenarioText: 反復作業（複製・一括調整）がエディタ専用 UI で過度な手順にならない。
- evidence:
  - 直近 Imported パスは TextWrapped 表示のみ（411-412 行）
  - 選択中へ CmpEditorAssetPath を付与するボタン／ショートカット無し
- suggestedUse: 制作: 連続ドロップ→配置の反復を 1 操作に短縮
- firstSeenAt: 2026-11-10T05:01:00+09:00
- lastSeenAt: 2026-11-10T05:01:00+09:00

### GAP-Editor-EditorSceneRuntime-004

- status: open
- tier: P2
- score: 680
- primaryUseCase: UC-Authoring
- scenarioId: AUT-Component-001
- frequency: medium
- frequencyBasis: 手編集 JSON の CmpMesh スロットが存在検出不能
- scope: UC-Authoring|Editor|EditorSceneRuntime|Incomplete|Project/Codes/Source/System/Editor/Scene/EditorSceneRuntime.cpp:39
- primary: `EditorSceneRuntime`
- summary: 【Authoring】EntityHasComponentForSlot が Mesh／Billboard を認識しない
- scenarioText: エンティティ／コンポント分割方針に沿ってアセットを組み立てられる（spec: component_split_guidelines.md）。
- evidence:
  - EntityHasComponentForSlot は登録 6 型のみ（39-64 行）
  - CmpMesh は AssetPath 副作用でのみ生成されスロット直指定と乖離
  - 存在不明時に TryAddEmptyComponent を再呼びしうる（289-293 行）
- suggestedUse: 制作: 分割例どおり Draw スロットを JSON で明示管理
- roadmapLink: feature-memos/component_split_guidelines.md
- firstSeenAt: 2026-11-10T05:21:00+09:00
- lastSeenAt: 2026-11-10T05:21:00+09:00

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

### GAP-Editor-PrefabAsset-001

- status: open
- tier: P2
- score: 680
- primaryUseCase: UC-Authoring
- scenarioId: AUT-AssetLoop-001
- frequency: medium
- frequencyBasis: プレハブは 1 回 1 体追加のみ
- scope: UC-Authoring|Editor|PrefabAsset|MissingUI|Project/Codes/Source/System/Editor/Entity/EntityEditorUi.cpp:484
- primary: `PrefabAsset`
- summary: 【Authoring】プレハブの複数一括配置 UI が無い
- scenarioText: 反復作業（複製・一括調整）がエディタ専用 UI で過度な手順にならない。
- evidence:
  - 「シーンへ追加」は AddInstanceFromPrefabPath 1 回呼び出し（484-486 行）
  - グリッド複製・件数指定・オフセット配置の API/UI 無し
- suggestedUse: 制作: 同プレハブを多数並べる反復作業を短縮
- firstSeenAt: 2026-11-10T03:01:00+09:00
- lastSeenAt: 2026-11-10T03:01:00+09:00

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

### GAP-Authoring-visual_lighting_profile-001

- status: open
- tier: P2
- score: 670
- primaryUseCase: UC-Authoring
- secondaryUseCases: UC-Play
- scenarioId: AUT-VisualTune-001
- frequency: medium
- frequencyBasis: カメラ tuning 以外の見た目データ資産が無い
- scope: UC-Authoring|Authoring|visual_lighting_profile|MissingData|EditorAssets/Tools/CursorAI/docs/feature-memos/volumetric_fog.md:23
- primary: `visual_lighting_profile`
- summary: 【Authoring】ライト／フォグ用 EditorAssets プロファイル JSON が未定義
- scenarioText: 制作者がライト・カメラ・ポストのパラメータを編集し、プレビューで結果を確認できる。
- evidence:
  - volumetric_fog.md は SceneGame 既定とエンジン統合のみ（23 行）
  - Camera には stage_view_camera_tuning.json があるが fog／rim／shadow 相当の EditorAssets ファイル無し
- suggestedUse: 制作: 見た目パラメータをカメラ JSON と同様に差し替え可能に
- roadmapLink: feature-memos/volumetric_fog.md
- firstSeenAt: 2026-11-10T04:31:00+09:00
- lastSeenAt: 2026-11-10T04:31:00+09:00

### GAP-Editor-SceneEntityEditor-003

- status: open
- tier: P2
- score: 670
- primaryUseCase: UC-Authoring
- scenarioId: AUT-EditorPlace-001
- frequency: medium
- frequencyBasis: シーンエディタ再入場で未保存のメモリ上配置が消える
- scope: UC-Authoring|Editor|SceneEntityEditor|Incomplete|Project/Codes/Source/Scene/SceneEntityEditor.cpp:115
- primary: `SceneEntityEditor`
- summary: 【Authoring】EnterScene が毎回ディスクから Load し編集中ドキュメントを上書き
- scenarioText: 制作者がエディタ上でオブジェクトを配置・保存し、再度開いて同じ状態を得られる。
- evidence:
  - EnterScene は EnsureCommonResources 後に LoadFromJsonPath を必ず呼ぶ（115 行）
  - 他シーンから戻った際、未保存の initialTransform 等は JSON 最終保存状態に戻る
- suggestedUse: 制作: パーティクルエディタ往復後も配置を保持
- firstSeenAt: 2026-11-10T03:51:00+09:00
- lastSeenAt: 2026-11-10T03:51:00+09:00

### GAP-Editor-EntityEditor-006

- status: open
- tier: P2
- score: 660
- primaryUseCase: UC-Authoring
- scenarioId: AUT-AssetLoop-001
- frequency: medium
- frequencyBasis: 複数ファイルドロップがコピー止まり
- scope: UC-Authoring|Editor|EntityEditor|Incomplete|Project/Codes/Source/System/Editor/Entity/EntityEditor.cpp:271
- primary: `EntityEditor`
- summary: 【Authoring】ProcessDroppedFileImports が Entities 生成まで繋がない
- scenarioText: 反復作業（複製・一括調整）がエディタ専用 UI で過度な手順にならない。
- evidence:
  - ドロップ処理は Imported へコピーとステータス更新のみ（280-290 行）
  - 複数 obj/png を落としてもインスタンス追加ループ無し
- suggestedUse: 制作: バルクインポートをシーン配置まで一気通貫
- firstSeenAt: 2026-11-10T05:01:00+09:00
- lastSeenAt: 2026-11-10T05:01:00+09:00

### GAP-Editor-EntityEditorUi-005

- status: open
- tier: P2
- score: 660
- primaryUseCase: UC-Authoring
- scenarioId: AUT-AssetLoop-001
- frequency: medium
- frequencyBasis: ヒエラルキーが選択のみで DnD 複製・並べ替え不可
- scope: UC-Authoring|Editor|EntityEditorUi|MissingUI|Project/Codes/Source/System/Editor/Entity/EntityEditorUi.cpp:332
- primary: `EntityEditorUi`
- summary: 【Authoring】ヒエラルキーのドラッグ複製・並べ替えが無い
- scenarioText: 反復作業（複製・一括調整）がエディタ専用 UI で過度な手順にならない。
- evidence:
  - DrawHierarchyNode は ImGui::Selectable のみ（332-334 行）
  - BeginDragDropTarget 等の DnD API 使用無し（Editor 配下 rg 0 件）
  - parentIndex はインスペクタのコンボのみ（503 行付近）
- suggestedUse: 制作: ツリー操作だけで複製・親付け替えを反復
- firstSeenAt: 2026-11-10T03:01:00+09:00
- lastSeenAt: 2026-11-10T03:01:00+09:00

### GAP-Editor-EntityEditorUi-006

- status: open
- tier: P2
- score: 660
- primaryUseCase: UC-Authoring
- scenarioId: AUT-Component-001
- frequency: medium
- frequencyBasis: 将来登録した Draw 型のパラメータ編集 UI が無い
- scope: UC-Authoring|Editor|EntityEditorUi|MissingUI|Project/Codes/Source/System/Editor/Entity/EntityEditorUi.cpp:271
- primary: `EntityEditorUi`
- summary: 【Authoring】DrawComponentParams が未知 typeName を未対応表示のみ
- scenarioText: エンティティ／コンポント分割方針に沿ってアセットを組み立てられる（spec: component_split_guidelines.md）。
- evidence:
  - 分岐は既知 6 型のみ（109-268 行）
  - フォールバックは「未対応コンポーネント」TextDisabled（271 行）
  - CmpMesh 等は JSON 手編集時にインスペクタ編集不可
- suggestedUse: 制作: 新 Cmp 追加手順の UI 反映漏れを可視化
- roadmapLink: feature-memos/component_split_guidelines.md
- firstSeenAt: 2026-11-10T05:21:00+09:00
- lastSeenAt: 2026-11-10T05:21:00+09:00

### GAP-Editor-EntityEditor-007

- status: open
- tier: P2
- score: 650
- primaryUseCase: UC-Authoring
- scenarioId: AUT-EditorPlace-001
- frequency: medium
- frequencyBasis: プリファブ配置で親子・複数配置が失われる
- scope: UC-Authoring|Editor|EntityEditor|Incomplete|Project/Codes/Source/System/Editor/Entity/EntityEditor.cpp:105
- primary: `EntityEditor`
- summary: 【Authoring】プリファブから先頭 1 体のみ追加し親は常に解除
- scenarioText: 制作者がエディタ上でオブジェクトを配置・保存し、再度開いて同じ状態を得られる。
- evidence:
  - シーン形式プリファブは instances.front() のみ採用（105-110 行）
  - 追加時 parentIndex を必ず -1（110, 114 行）で保存済み階層を復元しない
- suggestedUse: 制作: プリファブで組んだ配置をそのままシーンへ載せる
- firstSeenAt: 2026-11-10T05:41:00+09:00
- lastSeenAt: 2026-11-10T05:41:00+09:00

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

### GAP-Editor-SceneEntityEditor-005

- status: open
- tier: P2
- score: 650
- primaryUseCase: UC-Authoring
- scenarioId: AUT-VisualTune-001
- frequency: medium
- frequencyBasis: エディタ起動カメラが戦闘ステージ視点と乖離
- scope: UC-Authoring|Editor|SceneEntityEditor|Incomplete|Project/Codes/Source/Scene/SceneEntityEditor.cpp:91
- primary: `SceneEntityEditor`
- summary: 【Authoring】SetupPreviewCamera が自由位置のみで StageView 未使用
- scenarioText: 制作者がライト・カメラ・ポストのパラメータを編集し、プレビューで結果を確認できる。
- evidence:
  - EnterScene で SetTranslate(0,1.6,-6) のみ（91-95 行）
  - Runtime タブ描画時のみ SetupStageViewCamera（241-246 行）
  - Edit タブは DebugCamera 自由視点のまま
- suggestedUse: 制作: 本編に近い初期カメラで配置プレビュー
- roadmapLink: feature-memos/player_friendly_camera_policy.md
- firstSeenAt: 2026-11-10T06:21:00+09:00
- lastSeenAt: 2026-11-10T06:21:00+09:00

### GAP-UX-OperatorGuidanceSystem-003

- status: open
- tier: P2
- score: 650
- primaryUseCase: UC-Authoring
- secondaryUseCases: UC-Ship
- scenarioId: AUT-Guidance-001
- frequency: medium
- frequencyBasis: hints 欠落やスキーマ誤りでロードが例外終了しうる
- scope: UC-Authoring|UX|OperatorGuidanceSystem|Contract|Project/Codes/Source/UX/OperatorGuidanceSystem.cpp:93
- primary: `OperatorGuidanceSystem`
- summary: 【Authoring】LoadFromJsonFile が hints 必須で検証・代替が無い
- scenarioText: 初回利用者がオペレータ案内に沿って主要操作を完了できる（spec: operator_guidance_bubble.md）。
- evidence:
  - json.at("hints") で配列必須（93 行）、try は JSON パースのみ（65-69 行）
  - 必須フィールド id/actionId も at で未取得時は例外
- suggestedUse: 制作: JSON 編集ミス時に案内ゼロで黙って本編に入らない
- roadmapLink: feature-memos/operator_guidance_bubble.md
- firstSeenAt: 2026-11-10T04:11:00+09:00
- lastSeenAt: 2026-11-10T04:11:00+09:00

### GAP-Editor-EntityEditor-012

- status: open
- tier: P2
- score: 640
- primaryUseCase: UC-Authoring
- scenarioId: AUT-AssetLoop-001
- frequency: high
- frequencyBasis: インスタンス複製 API が無い
- scope: UC-Authoring|Editor|EntityEditor|MissingUI|Project/Codes/Source/System/Editor/Entity/EntityEditor.cpp:46
- primary: `EntityEditor`
- summary: 【Authoring】選択エンティティの複製が無い
- scenarioText: 反復作業（複製・一括調整）がエディタ専用 UI で過度な手順にならない。
- evidence:
  - AddEmptyInstance は既定定義のみ（46-55 行）
  - プレハブ経由は 1 体ずつ追加（92-120 行）
  - Cmp／タグ／transform を保った複製が JSON 手編集依存
- suggestedUse: 制作: 配置反復を複製ワンクリックで短縮
- firstSeenAt: 2026-11-10T23:51:00+09:00
- lastSeenAt: 2026-11-10T23:51:00+09:00

### GAP-Editor-EntityEditorHierarchy-001

- status: open
- tier: P2
- score: 640
- primaryUseCase: UC-Authoring
- scenarioId: AUT-AssetLoop-001
- frequency: medium
- frequencyBasis: タグ検索がカタログ限定で一括対象を絞れない
- scope: UC-Authoring|Editor|EntityEditorHierarchy|MissingUI|Project/Codes/Source/System/Editor/Entity/EntityEditorUi.cpp:383
- primary: `EntityEditorHierarchy`
- summary: 【Authoring】ヒエラルキーがタグでフィルタ／一括選択できない
- scenarioText: 反復作業（複製・一括調整）がエディタ専用 UI で過度な手順にならない。
- evidence:
  - DrawHierarchyPanel は全 instance を常時列挙（383-390 行）
  - tagSearchFilter は DrawTagsPanel のカタログ検索のみ（774 行）でツリー未連動
- suggestedUse: 制作: 同一タグ群だけを選んで調整・複製の反復
- firstSeenAt: 2026-11-10T05:01:00+09:00
- lastSeenAt: 2026-11-10T05:01:00+09:00

### GAP-Editor-EntityEditorUi-007

- status: open
- tier: P2
- score: 640
- primaryUseCase: UC-Authoring
- scenarioId: AUT-EditorPlace-001
- frequency: medium
- frequencyBasis: 保存先パスをインスペクタで誤変更しうる
- scope: UC-Authoring|Editor|EntityEditorUi|MissingUI|Project/Codes/Source/System/Editor/Entity/EntityEditorUi.cpp:500
- primary: `EntityEditorUi`
- summary: 【Authoring】インスペクタでシーン JSON パスを自由編集できる
- scenarioText: 制作者がエディタ上でオブジェクトを配置・保存し、再度開いて同じ状態を得られる。
- evidence:
  - DrawInspectorPanel が jsonFilePath を InputString（500 行）
  - プロジェクト一覧の selectedScenePath と同期・検証なし
  - 保存は ui_.jsonFilePath へ直書き（EntityEditor.cpp:194）
- suggestedUse: 制作: 意図しないファイル上書きで再オープン時に状態が消えるのを防ぐ
- firstSeenAt: 2026-11-10T05:41:00+09:00
- lastSeenAt: 2026-11-10T05:41:00+09:00

### GAP-Editor-EntityEditor-008

- status: open
- tier: P2
- score: 630
- primaryUseCase: UC-Authoring
- scenarioId: AUT-AssetLoop-001
- frequency: high
- frequencyBasis: オブジェクト追加が常に空テンプレのみ
- scope: UC-Authoring|Editor|EntityEditor|Incomplete|Project/Codes/Source/System/Editor/Entity/EntityEditor.cpp:46
- primary: `EntityEditor`
- summary: 【Authoring】AddEmptyInstance が選択中定義を複製しない
- scenarioText: 反復作業（複製・一括調整）がエディタ専用 UI で過度な手順にならない。
- evidence:
  - AddEmptyInstance は MakeDefaultEntityDefinition のみ（46-51 行）
  - ヒエラルキー「オブジェクトを追加」は常にこの経路（EntityEditorUi.cpp:374）
  - プレハブ JSON 経由の 2 段階が複製の実質代替
- suggestedUse: 制作: 選択オブジェクトを複製して配置反復
- firstSeenAt: 2026-11-10T06:51:00+09:00
- lastSeenAt: 2026-11-10T06:51:00+09:00

### GAP-Editor-EntityEditorUi-008

- status: open
- tier: P2
- score: 630
- primaryUseCase: UC-Authoring
- scenarioId: AUT-EditorPlace-001
- frequency: medium
- frequencyBasis: 表示名とランタイム名が別フィールドで非同期
- scope: UC-Authoring|Editor|EntityEditorUi|Contract|Project/Codes/Source/System/Editor/Entity/EntityEditorUi.cpp:501
- primary: `EntityEditorUi`
- summary: 【Authoring】instanceName と entityName を別々に編集しうる
- scenarioText: 制作者がエディタ上でオブジェクトを配置・保存し、再度開いて同じ状態を得られる。
- evidence:
  - ヒエラルキーは instanceName 表示（332 行）
  - entityName は別 InputString（530 行）
  - BuildEntityFromDefinition は entityName のみ Entity 生成（EditorSceneRuntime.cpp:280）
- suggestedUse: 制作: 再オープン後も一覧名とプレビュー実体名が一致
- firstSeenAt: 2026-11-10T05:41:00+09:00
- lastSeenAt: 2026-11-10T05:41:00+09:00

### GAP-Authoring-component_split_next-001

- status: open
- tier: P2
- score: 620
- primaryUseCase: UC-Authoring
- scenarioId: AUT-Component-001
- frequency: low
- frequencyBasis: 次分割候補がガイドラインにのみあり登録手順が未整備
- scope: UC-Authoring|Authoring|component_split_next|MissingData|EditorAssets/Tools/CursorAI/docs/feature-memos/component_split_guidelines.md:32
- primary: `component_split_next`
- summary: 【Authoring】ランタイム用 Data／Collider 分割候補のエディタ登録が未着手
- scenarioText: エンティティ／コンポント分割方針に沿ってアセットを組み立てられる（spec: component_split_guidelines.md）。
- evidence:
  - ガイドライン「次の分割候補」にランタイム Data・Collider AABB（32-35 行）
  - EntityFactory 接続は「別タスク」記載のみで kKnownComponents 未反映
- suggestedUse: 制作: 方針どおり新 Cmp を追加するロードマップを追える
- roadmapLink: feature-memos/component_split_guidelines.md
- firstSeenAt: 2026-11-10T03:31:00+09:00
- lastSeenAt: 2026-11-10T03:31:00+09:00

### GAP-Editor-EntityEditorUi-009

- status: open
- tier: P2
- score: 620
- primaryUseCase: UC-Authoring
- scenarioId: AUT-AssetLoop-001
- frequency: medium
- frequencyBasis: 初期トランスフォームは 1 インスタンスずつ手入力
- scope: UC-Authoring|Editor|EntityEditorUi|MissingUI|Project/Codes/Source/System/Editor/Entity/EntityEditorUi.cpp:615
- primary: `EntityEditorUi`
- summary: 【Authoring】トランスフォームのコピー／一括適用 UI が無い
- scenarioText: 反復作業（複製・一括調整）がエディタ専用 UI で過度な手順にならない。
- evidence:
  - 初期トランスフォームは選択中 definition の ImGui のみ（615-631 行）
  - 複数選択・貼り付け・オフセット複製のボタン無し
  - グリッド配置はプレハブ GAP（PrefabAsset-001）と同系の手順不足
- suggestedUse: 制作: 同オフセットで多数配置する反復を短縮
- firstSeenAt: 2026-11-10T06:51:00+09:00
- lastSeenAt: 2026-11-10T06:51:00+09:00

### GAP-Editor-EntityEditorUi-017

- status: open
- tier: P2
- score: 620
- primaryUseCase: UC-Authoring
- scenarioId: AUT-AssetLoop-001
- frequency: medium
- frequencyBasis: CmpCamera 追加可だがパラメータ UI 無し
- scope: UC-Authoring|Editor|EntityEditorUi|MissingUI|Project/Codes/Source/System/Editor/Entity/EntityEditorUi.cpp:37
- primary: `EntityEditorUi`
- summary: 【Authoring】CmpCamera が既知型だが未対応表示
- scenarioText: 反復作業（複製・一括調整）がエディタ専用 UI で過度な手順にならない。
- evidence:
  - kKnownComponents に CmpCamera 登録（37 行）
  - DrawComponentParams に分岐無し→272 行の未対応表示
  - MainCamera 量産時に JSON 直編集が必要
- suggestedUse: 制作: camera.main タグ付き Cmp の反復調整
- roadmapLink: feature-memos/entity_camera_components.md
- firstSeenAt: 2026-11-11T00:21:00+09:00
- lastSeenAt: 2026-11-11T00:21:00+09:00

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

### GAP-Scene-SceneGame-004

- status: open
- tier: P2
- score: 620
- primaryUseCase: UC-Authoring
- secondaryUseCases: UC-Play
- scenarioId: AUT-Guidance-001
- frequency: high
- frequencyBasis: ナビ吹き出しと旧 howTo オーバーレイが同時表示
- scope: UC-Authoring|Scene|SceneGame|Incomplete|Project/Codes/Source/Scene/SceneGame.cpp:205
- primary: `SceneGame`
- summary: 【Authoring】案内システム有効でも常時 howTo オーバーレイを描画
- scenarioText: 初回利用者がオペレータ案内に沿って主要操作を完了できる（spec: operator_guidance_bubble.md）。
- evidence:
  - DrawScene は DrawHowToPlayUi と DrawOperatorGuidanceUi を連続呼び出し（205-206 行）
  - tutorialReplacement はイントロ howTo スキップのみで戦闘中の DrawGameHowToPlayUi は無条件（242-248 行）
- suggestedUse: プレイ: 初回利用者が案内どおり 1 系統の UI で操作を学べる
- roadmapLink: feature-memos/operator_guidance_bubble.md
- firstSeenAt: 2026-11-10T04:11:00+09:00
- lastSeenAt: 2026-11-10T04:11:00+09:00

### GAP-Editor-EntityEditor-009

- status: open
- tier: P2
- score: 610
- primaryUseCase: UC-Authoring
- scenarioId: AUT-AssetLoop-001
- frequency: medium
- frequencyBasis: 削除は 1 選択前提で一括整理できない
- scope: UC-Authoring|Editor|EntityEditor|Incomplete|Project/Codes/Source/System/Editor/Entity/EntityEditor.cpp:58
- primary: `EntityEditor`
- summary: 【Authoring】RemoveSelectedInstance が単一 index のみ
- scenarioText: 反復作業（複製・一括調整）がエディタ専用 UI で過度な手順にならない。
- evidence:
  - removeIndex は selectedInstanceIndex 1 件（62-63 行）
  - 子 parentIndex の補正のみで多体削除ループ無し
  - ヒエラルキー「削除」も同 API（EntityEditorUi.cpp:378）
- suggestedUse: 制作: 試配置の一括削除
- firstSeenAt: 2026-11-10T07:21:00+09:00
- lastSeenAt: 2026-11-10T07:21:00+09:00

### GAP-Editor-EditorImportedAsset-002

- status: open
- tier: P2
- score: 600
- primaryUseCase: UC-Authoring
- scenarioId: AUT-AssetLoop-001
- frequency: medium
- frequencyBasis: Imported 一覧 API が無く直近 1 件表示のみ
- scope: UC-Authoring|Editor|EditorImportedAsset|MissingData|Project/Codes/Include/System/Editor/EditorImportedAsset.h:14
- primary: `EditorImportedAsset`
- summary: 【Authoring】Imported 配下のファイル一覧 API が無い
- scenarioText: 反復作業（複製・一括調整）がエディタ専用 UI で過度な手順にならない。
- evidence:
  - Import 名前空間は Copy/Ensure/Queue のみ（EditorImportedAsset.h:11-14）
  - UI は lastImportedAssetPath の TextWrapped のみ（EntityEditorUi.cpp:411-412）
  - 過去ドロップ分を再選択して一括 CmpEditorAssetPath 付与できない
- suggestedUse: 制作: インポート済みアセットを一覧から連続配置
- firstSeenAt: 2026-11-10T06:51:00+09:00
- lastSeenAt: 2026-11-10T06:51:00+09:00

### GAP-Editor-EntityEditorUi-015

- status: open
- tier: P2
- score: 600
- primaryUseCase: UC-Authoring
- scenarioId: AUT-AssetLoop-001
- frequency: high
- frequencyBasis: ヒエラルキーは空オブジェクト追加のみ
- scope: UC-Authoring|Editor|EntityEditorUi|MissingUI|Project/Codes/Source/System/Editor/Entity/EntityEditorUi.cpp:375
- primary: `EntityEditorUi`
- summary: 【Authoring】「オブジェクトを追加」は空のみ
- scenarioText: 反復作業（複製・一括調整）がエディタ専用 UI で過度な手順にならない。
- evidence:
  - ボタンは AddEmptyInstance のみ（375-377 行）
  - 選択中定義の複製／オフセット複製ボタン無し
  - EntityEditor-012 と同系で UI 導線が未整備
- suggestedUse: 制作: 選択オブジェクトの複製をヒエラルキーから
- firstSeenAt: 2026-11-10T23:51:00+09:00
- lastSeenAt: 2026-11-10T23:51:00+09:00

### GAP-Editor-PrefabAsset-002

- status: open
- tier: P2
- score: 600
- primaryUseCase: UC-Authoring
- scenarioId: AUT-AssetLoop-001
- frequency: medium
- frequencyBasis: プレハブ出力が Definition のみで反復配置に不向き
- scope: UC-Authoring|Editor|PrefabAsset|Incomplete|Project/Codes/Source/System/Editor/Entity/EntityEditor.cpp:85
- primary: `PrefabAsset`
- summary: 【Authoring】プレハブ保存が EntityDefinition 単体 JSON
- scenarioText: 反復作業（複製・一括調整）がエディタ専用 UI で過度な手順にならない。
- evidence:
  - SaveToFile(prefabPath, definition) のみ（85 行）
  - instanceName／parentIndex は保存されない
  - ツリー複製の再利用はシーン JSON 手切り出しが必要
- suggestedUse: 制作: プレハブ単位の複製配置ループを短縮
- firstSeenAt: 2026-11-10T07:21:00+09:00
- lastSeenAt: 2026-11-10T07:21:00+09:00

### GAP-UX-OperatorGuidanceSystem-005

- status: open
- tier: P2
- score: 600
- primaryUseCase: UC-Authoring
- secondaryUseCases: UC-Play
- scenarioId: AUT-Guidance-001
- frequency: medium
- frequencyBasis: welcome の actionId session に Notify 配線が無い
- scope: UC-Authoring|UX|OperatorGuidanceSystem|Unwired|Project/Codes/Source/UX/OperatorGuidanceSystem.cpp:150
- primary: `OperatorGuidanceSystem`
- summary: 【Authoring】actionId session は JSON にあるがコードから通知されない
- scenarioText: 初回利用者がオペレータ案内に沿って主要操作を完了できる（spec: operator_guidance_bubble.md）。
- evidence:
  - welcome_battle は actionId=session（operator_guidance.json:20）
  - NotifyAction 呼び出しは parry のみ（CmpMoveByInput.cpp:119）
  - 操作完了で吹き出しを閉じる経路が session に無い（150-158 行）
- suggestedUse: 制作: 歓迎ヒントをデータ定義どおり操作で進められる
- roadmapLink: feature-memos/operator_guidance_bubble.md
- firstSeenAt: 2026-11-10T06:01:00+09:00
- lastSeenAt: 2026-11-10T06:01:00+09:00

### GAP-Editor-EntityEditorUi-010

- status: open
- tier: P2
- score: 590
- primaryUseCase: UC-Authoring
- scenarioId: AUT-AssetLoop-001
- frequency: medium
- frequencyBasis: タグ解除・コンポ削除が 1 エンティティ単位
- scope: UC-Authoring|Editor|EntityEditorUi|MissingUI|Project/Codes/Source/System/Editor/Entity/EntityEditorUi.cpp:608
- primary: `EntityEditorUi`
- summary: 【Authoring】タグをすべて解除が選択 1 体のみ
- scenarioText: 反復作業（複製・一括調整）がエディタ専用 UI で過度な手順にならない。
- evidence:
  - document.tags.clear() は SelectedDefinition のみ（608-611 行）
  - RemoveTagFromDocument はタグパネル用でインスペクタから一括無し
  - 同一タグ群の一括解除は手作業繰り返し
- suggestedUse: 制作: 試験タグ付けの一括リセット
- firstSeenAt: 2026-11-10T07:21:00+09:00
- lastSeenAt: 2026-11-10T07:21:00+09:00

### GAP-UX-OperatorGuidanceSystem-006

- status: open
- tier: P2
- score: 590
- primaryUseCase: UC-Authoring
- secondaryUseCases: UC-Play
- scenarioId: AUT-Guidance-001
- frequency: medium
- frequencyBasis: 画像読込失敗時 message も描画されず空枠
- scope: UC-Authoring|UX|OperatorGuidanceSystem|Incomplete|Project/Codes/Source/UX/OperatorGuidanceSystem.cpp:297
- primary: `OperatorGuidanceSystem`
- summary: 【Authoring】body テクスチャ無しでも message フォールバック無し
- scenarioText: 初回利用者がオペレータ案内に沿って主要操作を完了できる（spec: operator_guidance_bubble.md）。
- evidence:
  - textureIndex==UINT32_MAX 時は枠のみ残る（297-306 行）
  - message 描画は未実装（229-306 行、feature-memo 今後）
  - 制作でパス誤りに気づきにくい
- suggestedUse: 制作: テクスチャ未配置でも文言で案内を確認
- roadmapLink: feature-memos/operator_guidance_bubble.md
- firstSeenAt: 2026-11-10T06:01:00+09:00
- lastSeenAt: 2026-11-10T06:01:00+09:00

### GAP-Editor-EntityEditorTagCatalog-001

- status: open
- tier: P2
- score: 580
- primaryUseCase: UC-Authoring
- scenarioId: AUT-AssetLoop-001
- frequency: medium
- frequencyBasis: タグパネルがカタログ管理のみ
- scope: UC-Authoring|Editor|EntityEditorTagCatalog|MissingUI|Project/Codes/Source/System/Editor/Entity/EntityEditorUi.cpp:818
- primary: `EntityEditorTagCatalog`
- summary: 【Authoring】登録タグから全インスタンス一括付与無し
- scenarioText: 反復作業（複製・一括調整）がエディタ専用 UI で過度な手順にならない。
- evidence:
  - DrawTagsPanel は catalog 追加・改名・削除のみ（792-849 行）
  - インスペクタの「カタログのタグを付与」は選択 1 体（621 行）
  - camera.main 等のロールタグを多数配置へ展開する導線なし
- suggestedUse: 制作: ロールタグのシーン全域への一括付与
- firstSeenAt: 2026-11-11T00:21:00+09:00
- lastSeenAt: 2026-11-11T00:21:00+09:00

### GAP-Editor-EntityEditorUi-011

- status: open
- tier: P2
- score: 580
- primaryUseCase: UC-Authoring
- scenarioId: AUT-AssetLoop-001
- frequency: medium
- frequencyBasis: コンポーネント削除が 1 スロットずつ
- scope: UC-Authoring|Editor|EntityEditorUi|MissingUI|Project/Codes/Source/System/Editor/Entity/EntityEditorUi.cpp:686
- primary: `EntityEditorUi`
- summary: 【Authoring】コンポーネント削除が選択 1 件のみ
- scenarioText: 反復作業（複製・一括調整）がエディタ専用 UI で過度な手順にならない。
- evidence:
  - erase は selectedComponentIndex 1 件（686-688 行）
  - 同型スロットの一括削除 UI 無し
  - EntityEditorDefinition-001 のコピー不足と合わせて調整が冗長
- suggestedUse: 制作: 試した Cmp を一括で外す
- firstSeenAt: 2026-11-10T07:51:00+09:00
- lastSeenAt: 2026-11-10T07:51:00+09:00

### GAP-Editor-PrefabAsset-004

- status: open
- tier: P2
- score: 580
- primaryUseCase: UC-Authoring
- scenarioId: AUT-AssetLoop-001
- frequency: medium
- frequencyBasis: プレハブ出力が毎回新規パス生成
- scope: UC-Authoring|Editor|PrefabAsset|Incomplete|Project/Codes/Source/System/Editor/Entity/EntityEditorUi.cpp:478
- primary: `PrefabAsset`
- summary: 【Authoring】出力が MakeUnique で上書き更新できない
- scenarioText: 反復作業（複製・一括調整）がエディタ専用 UI で過度な手順にならない。
- evidence:
  - 保存は常に MakeUniquePrefabFilePath（480-483 行）
  - 一覧で選択中の selectedPrefabPath を上書きする経路無し
  - 調整ループで Entity_0.json 群が増殖しやすい
- suggestedUse: 制作: 同プレハブへの反復上書き保存
- firstSeenAt: 2026-11-11T01:21:00+09:00
- lastSeenAt: 2026-11-11T01:21:00+09:00

### GAP-Editor-EntityEditorDefinition-004

- status: open
- tier: P2
- score: 570
- primaryUseCase: UC-Authoring
- scenarioId: AUT-AssetLoop-001
- frequency: medium
- frequencyBasis: CmpCamera が既知型なのに既定 params 無し
- scope: UC-Authoring|Editor|EntityEditorDefinition|Incomplete|Project/Codes/Source/System/Editor/Entity/EntityEditorDefinition.cpp:72
- primary: `EntityEditorDefinition`
- summary: 【Authoring】CmpCamera 追加で params が空オブジェクト
- scenarioText: 反復作業（複製・一括調整）がエディタ専用 UI で過度な手順にならない。
- evidence:
  - IsKnown に CmpCamera を含む（17-21 行）
  - DefaultParamsForType は 72 行で object() フォールバック
  - カメラ大量配置時に JSON 手入力または都度編集が必要
- suggestedUse: 制作: カメラ試配置のプリセット既定化
- roadmapLink: feature-memos/entity_camera_components.md
- firstSeenAt: 2026-11-11T02:51:00+09:00
- lastSeenAt: 2026-11-11T02:51:00+09:00

### GAP-Editor-PrefabAsset-003

- status: open
- tier: P2
- score: 570
- primaryUseCase: UC-Authoring
- scenarioId: AUT-AssetLoop-001
- frequency: high
- frequencyBasis: プレハブ配置が 1 クリック 1 体
- scope: UC-Authoring|Editor|PrefabAsset|MissingUI|Project/Codes/Source/System/Editor/Entity/EntityEditorUi.cpp:486
- primary: `PrefabAsset`
- summary: 【Authoring】シーンへ追加が毎回 1 インスタンス
- scenarioText: 反復作業（複製・一括調整）がエディタ専用 UI で過度な手順にならない。
- evidence:
  - ボタン 1 回で AddInstanceFromPrefabPath 1 回（484-486 行）
  - 件数・オフセット指定無し（PrefabAsset-001 と同系）
  - 連打が複製反復の実質手段
- suggestedUse: 制作: 同プレハブ大量配置の手数削減
- firstSeenAt: 2026-11-10T07:51:00+09:00
- lastSeenAt: 2026-11-10T07:51:00+09:00

### GAP-Editor-EntityEditor-016

- status: open
- tier: P2
- score: 560
- primaryUseCase: UC-Authoring
- scenarioId: AUT-AssetLoop-001
- frequency: high
- frequencyBasis: 保存後にプレビュー再構築が走らない
- scope: UC-Authoring|Editor|EntityEditor|Incomplete|Project/Codes/Source/System/Editor/Entity/EntityEditor.cpp:192
- primary: `EntityEditor`
- summary: 【Authoring】requestSaveToDisk が needApply を立てない
- scenarioText: 反復作業（複製・一括調整）がエディタ専用 UI で過度な手順にならない。
- evidence:
  - SaveToJsonPath のみ実行（192-195 行）で needApply 無し
  - requestApplyPreview や Reload と非対称（186-198 行）
  - 保存直後の Runtime プレビューがディスク内容とずれうる
- suggestedUse: 制作: 保存とプレビュー同期をワンアクションに
- firstSeenAt: 2026-11-11T02:21:00+09:00
- lastSeenAt: 2026-11-11T02:21:00+09:00

### GAP-Editor-EntityEditorHierarchy-002

- status: open
- tier: P2
- score: 560
- primaryUseCase: UC-Authoring
- scenarioId: AUT-AssetLoop-001
- frequency: high
- frequencyBasis: ヒエラルキー選択が 1 体ずつ
- scope: UC-Authoring|Editor|EntityEditorHierarchy|MissingUI|Project/Codes/Source/System/Editor/Entity/EntityEditorUi.cpp:333
- primary: `EntityEditorHierarchy`
- summary: 【Authoring】Selectable が単一 instanceIndex のみ
- scenarioText: 反復作業（複製・一括調整）がエディタ専用 UI で過度な手順にならない。
- evidence:
  - DrawHierarchyNode はクリック 1 件で selectedInstanceIndex 更新（333-334 行）
  - 複数選択・一括複製・一括タグ付け UI 無し
  - EntityEditorHierarchy-001 と同系
- suggestedUse: 制作: 複数エンティティの一括調整
- firstSeenAt: 2026-11-10T23:21:00+09:00
- lastSeenAt: 2026-11-10T23:21:00+09:00

### GAP-Editor-EntityEditorUi-022

- status: open
- tier: P2
- score: 560
- primaryUseCase: UC-Authoring
- scenarioId: AUT-AssetLoop-001
- frequency: medium
- frequencyBasis: 型変更で params が全リセット
- scope: UC-Authoring|Editor|EntityEditorUi|Incomplete|Project/Codes/Source/System/Editor/Entity/EntityEditorUi.cpp:768
- primary: `EntityEditorUi`
- summary: 【Authoring】コンボ変更で DefaultParams に差し替え
- scenarioText: 反復作業（複製・一括調整）がエディタ専用 UI で過度な手順にならない。
- evidence:
  - 型 Combo 変更時 selected.params = DefaultParamsForType（768-776 行）
  - 既存 JSON フィールドのマージ・保持無し
  - 似た Cmp 試行でパラメータを手打ちで戻す手間
- suggestedUse: 制作: 型差し替え時に共通キーを温存
- firstSeenAt: 2026-11-11T01:21:00+09:00
- lastSeenAt: 2026-11-11T01:21:00+09:00

### GAP-Editor-EntityEditorUi-025

- status: open
- tier: P2
- score: 560
- primaryUseCase: UC-Authoring
- scenarioId: AUT-AssetLoop-001
- frequency: high
- frequencyBasis: 型変更で params 全置換
- scope: UC-Authoring|Editor|EntityEditorUi|Contract|Project/Codes/Source/System/Editor/Entity/EntityEditorUi.cpp:775
- primary: `EntityEditorUi`
- summary: 【Authoring】型 Combo で既定 params に上書き
- scenarioText: 反復作業（複製・一括調整）がエディタ専用 UI で過度な手順にならない。
- evidence:
  - Combo 変更時 selected.params = DefaultParamsForType（773-775 行）
  - 細調整済み params を保持せず JSON 手戻り
  - Ui-014 と同系だが型切替でも全消し
- suggestedUse: 制作: Cmp 型試行錯誤後も数値を維持
- firstSeenAt: 2026-11-11T03:21:00+09:00
- lastSeenAt: 2026-11-11T03:21:00+09:00

### GAP-Editor-UXAuthoring-002

- status: open
- tier: P2
- score: 560
- primaryUseCase: UC-Authoring
- secondaryUseCases: UC-Ship
- scenarioId: AUT-Guidance-001
- frequency: low
- frequencyBasis: welcomeHintId と hints.id の整合を自動検証しない
- scope: UC-Authoring|Infra|operator_guidance_verify|MissingTest|EditorAssets/UX/operator_guidance.json:12
- primary: `operator_guidance_verify`
- summary: 【Authoring】operator_guidance.json の参照整合チェックが無い
- scenarioText: 初回利用者がオペレータ案内に沿って主要操作を完了できる（spec: operator_guidance_bubble.md）。
- evidence:
  - welcomeHintId は welcome_battle（operator_guidance.json:15）
  - 誤 id でも Load 成功し ShowHintById は無視（129-137 行）
  - CI／verify スクリプト呼び出し無し
- suggestedUse: 配布: 案内データ破損をマージ前に検知
- roadmapLink: feature-memos/operator_guidance_bubble.md
- firstSeenAt: 2026-11-10T06:01:00+09:00
- lastSeenAt: 2026-11-10T06:01:00+09:00

### GAP-Infra-stage_view_camera_tuning-002

- status: open
- tier: P3
- score: 560
- primaryUseCase: UC-Authoring
- scenarioId: AUT-VisualTune-001
- frequency: medium
- frequencyBasis: 解像度フィールドがプレビューアスペクトと非連動
- scope: UC-Authoring|Infra|stage_view_camera_tuning|Contract|EditorAssets/Camera/stage_view_camera_tuning.json:5
- primary: `stage_view_camera_tuning`
- summary: 【Authoring】tuning JSON の client 解像度が参照されない
- scenarioText: 制作者がライト・カメラ・ポストのパラメータを編集し、プレビューで結果を確認できる。
- evidence:
  - clientWidth/Height 記載（5-6 行）だが StageViewCamera.cpp は Window 既定のみ
  - ImGui シーンビューは可変サイズで FitCameraPerspectiveToViewport（EditorFrame.cpp:121）
  - 本編とエディタでアスペクト前提がズレうる
- suggestedUse: 制作: JSON 調整が ImGui プレビューと本番で一致
- roadmapLink: feature-memos/odyssey_style_stage_camera.md
- firstSeenAt: 2026-11-10T06:21:00+09:00
- lastSeenAt: 2026-11-10T06:21:00+09:00

### GAP-Editor-EntityEditor-014

- status: open
- tier: P2
- score: 550
- primaryUseCase: UC-Authoring
- scenarioId: AUT-AssetLoop-001
- frequency: medium
- frequencyBasis: ロード後に選択インデックスが 0 固定
- scope: UC-Authoring|Editor|EntityEditor|Incomplete|Project/Codes/Source/System/Editor/Entity/EntityEditor.cpp:163
- primary: `EntityEditor`
- summary: 【Authoring】LoadFromJsonPath が選択を先頭に戻す
- scenarioText: 反復作業（複製・一括調整）がエディタ専用 UI で過度な手順にならない。
- evidence:
  - 成功ロード後 selectedInstanceIndex=0（163 行）
  - selectedComponentIndex も 0 リセット（164 行）
  - ディスク再読込・シーン切替のたびに末尾編集位置を失う
- suggestedUse: 制作: 同一シーン内の反復編集で選択を維持
- firstSeenAt: 2026-11-11T00:51:00+09:00
- lastSeenAt: 2026-11-11T00:51:00+09:00

### GAP-Editor-EntityEditorUi-014

- status: open
- tier: P2
- score: 550
- primaryUseCase: UC-Authoring
- scenarioId: AUT-AssetLoop-001
- frequency: medium
- frequencyBasis: コンポ型変更が選択 1 スロットのみ
- scope: UC-Authoring|Editor|EntityEditorUi|MissingUI|Project/Codes/Source/System/Editor/Entity/EntityEditorUi.cpp:768
- primary: `EntityEditorUi`
- summary: 【Authoring】型 Combo 変更は params を既定に置換
- scenarioText: 反復作業（複製・一括調整）がエディタ専用 UI で過度な手順にならない。
- evidence:
  - Combo 変更で params=DefaultParamsForType（768-776 行）
  - 同型スロットへの一括型差し替え・params 維持無し
  - 複製反復で毎スロット手作業
- suggestedUse: 制作: 同型 Cmp の型/パラメータ一括更新
- firstSeenAt: 2026-11-10T23:21:00+09:00
- lastSeenAt: 2026-11-10T23:21:00+09:00

### GAP-Editor-EntityEditorUi-023

- status: open
- tier: P2
- score: 550
- primaryUseCase: UC-Authoring
- scenarioId: AUT-AssetLoop-001
- frequency: medium
- frequencyBasis: インポート適用が 1 スロットずつ
- scope: UC-Authoring|Editor|EntityEditorUi|MissingUI|Project/Codes/Source/System/Editor/Entity/EntityEditorUi.cpp:136
- primary: `EntityEditorUi`
- summary: 【Authoring】直近ドロップ適用が選択中 Cmp のみ
- scenarioText: 反復作業（複製・一括調整）がエディタ専用 UI で過度な手順にならない。
- evidence:
  - 「直近ドロップを assetPath に適用」は CmpEditorAssetPath 編集中のみ（136-139 行）
  - 複数 mesh／billboard スロットへの一括適用無し
  - EditorImportedAsset-002 と合わせてドロップ反復が冗長
- suggestedUse: 制作: 直近インポートを複数 Cmp へ一括セット
- firstSeenAt: 2026-11-11T01:51:00+09:00
- lastSeenAt: 2026-11-11T01:51:00+09:00

### GAP-Editor-EntityEditorUi-012

- status: open
- tier: P3
- score: 540
- primaryUseCase: UC-Authoring
- scenarioId: AUT-AssetLoop-001
- frequency: medium
- frequencyBasis: 実行順変更が 1 コンポーネント単位
- scope: UC-Authoring|Editor|EntityEditorUi|MissingUI|Project/Codes/Source/System/Editor/Entity/EntityEditorUi.cpp:724
- primary: `EntityEditorUi`
- summary: 【Authoring】実行順 InputInt が 1 スロットのみ
- scenarioText: 反復作業（複製・一括調整）がエディタ専用 UI で過度な手順にならない。
- evidence:
  - InputInt は selected スロットのみ（723-726 行）
  - 同型 Draw コンポの実行順を揃える一括 UI 無し
  - 並べ替えは手入力または JSON 編集
- suggestedUse: 制作: 同型コンポの優先度をまとめて調整
- firstSeenAt: 2026-11-10T07:51:00+09:00
- lastSeenAt: 2026-11-10T07:51:00+09:00

### GAP-Editor-EntityEditorUi-016

- status: open
- tier: P3
- score: 540
- primaryUseCase: UC-Authoring
- scenarioId: AUT-AssetLoop-001
- frequency: medium
- frequencyBasis: CmpHitSphere の球パラメータが 1 個ずつ
- scope: UC-Authoring|Editor|EntityEditorUi|MissingUI|Project/Codes/Source/System/Editor/Entity/EntityEditorUi.cpp:242
- primary: `EntityEditorUi`
- summary: 【Authoring】ヒット球は選択 1 球のみ編集
- scenarioText: 反復作業（複製・一括調整）がエディタ専用 UI で過度な手順にならない。
- evidence:
  - Selectable で selectedSphereIndex を 1 件更新（242-246 行）
  - radius/offset の Drag は選択球のみ（249-267 行）
  - 複数球の半径一括・配列コピー UI 無し
- suggestedUse: 制作: コライダー配列の一括スケール調整
- firstSeenAt: 2026-11-10T23:51:00+09:00
- lastSeenAt: 2026-11-10T23:51:00+09:00

### GAP-Editor-EntityEditorUi-026

- status: open
- tier: P2
- score: 540
- primaryUseCase: UC-Authoring
- scenarioId: AUT-AssetLoop-001
- frequency: medium
- frequencyBasis: JSON パスをインスペクタで自由編集
- scope: UC-Authoring|Editor|EntityEditorUi|Contract|Project/Codes/Source/System/Editor/Entity/EntityEditorUi.cpp:521
- primary: `EntityEditorUi`
- summary: 【Authoring】シーン JSON パスを InputString 編集
- scenarioText: 反復作業（複製・一括調整）がエディタ専用 UI で過度な手順にならない。
- evidence:
  - ImGui::InputString で jsonFilePath 直接変更（521 行）
  - 存在検証やプロジェクトパネル同期無し
  - 誤パス保存→再読込失敗の反復が手戻り
- suggestedUse: 制作: シーンパスは一覧選択に限定
- firstSeenAt: 2026-11-11T03:51:00+09:00
- lastSeenAt: 2026-11-11T03:51:00+09:00

### GAP-UX-OperatorGuidanceSystem-004

- status: open
- tier: P3
- score: 540
- primaryUseCase: UC-Authoring
- scenarioId: AUT-Guidance-001
- frequency: medium
- frequencyBasis: JSON の displayName が画面に出ずナビ設定の確認ができない
- scope: UC-Authoring|UX|OperatorGuidanceSystem|Incomplete|Project/Codes/Source/UX/OperatorGuidanceSystem.cpp:73
- primary: `OperatorGuidanceSystem`
- summary: 【Authoring】navigator.displayName を読むが描画しない
- scenarioText: 初回利用者がオペレータ案内に沿って主要操作を完了できる（spec: operator_guidance_bubble.md）。
- evidence:
  - LoadFromJsonFile で displayName を navigator_ に格納（73 行）
  - DrawNavigatorPortrait は肖像テクスチャのみ（202-227 行）で名前ラベル無し
- suggestedUse: 制作: ナビキャラ名の差し替えをプレイで確認
- roadmapLink: feature-memos/operator_guidance_bubble.md
- firstSeenAt: 2026-11-10T04:11:00+09:00
- lastSeenAt: 2026-11-10T04:11:00+09:00

### GAP-Editor-EntityEditorUi-013

- status: open
- tier: P3
- score: 530
- primaryUseCase: UC-Authoring
- scenarioId: AUT-AssetLoop-001
- frequency: medium
- frequencyBasis: 未知 Cmp は TextDisabled のみでパラメータ一括編集不可
- scope: UC-Authoring|Editor|EntityEditorUi|MissingUI|Project/Codes/Source/System/Editor/Entity/EntityEditorUi.cpp:272
- primary: `EntityEditorUi`
- summary: 【Authoring】未対応コンポーネントは警告表示のみ
- scenarioText: 反復作業（複製・一括調整）がエディタ専用 UI で過度な手順にならない。
- evidence:
  - DrawComponentParams は未登録型で return false（272-273 行）
  - JSON 直編集か再追加が必要
  - CmpCamera 等追加後も未知型が混在すると一括調整不可
- suggestedUse: 制作: 未知型の最低限 JSON 編集 UI
- firstSeenAt: 2026-11-10T23:21:00+09:00
- lastSeenAt: 2026-11-10T23:21:00+09:00

### GAP-Editor-EntityEditorUi-018

- status: open
- tier: P3
- score: 530
- primaryUseCase: UC-Authoring
- scenarioId: AUT-AssetLoop-001
- frequency: medium
- frequencyBasis: コンポ削除が 1 スロットずつ
- scope: UC-Authoring|Editor|EntityEditorUi|MissingUI|Project/Codes/Source/System/Editor/Entity/EntityEditorUi.cpp:707
- primary: `EntityEditorUi`
- summary: 【Authoring】コンポーネント削除が選択 1 件のみ
- scenarioText: 反復作業（複製・一括調整）がエディタ専用 UI で過度な手順にならない。
- evidence:
  - 削除ボタンは selectedComponentIndex のみ erase（707-711 行）
  - 同型 Cmp の一括削除・置換前整理 UI 無し
  - EntityEditorUi-014 の型変更と合わせ手順が長い
- suggestedUse: 制作: 試行錯誤後の Cmp 一括掃除
- firstSeenAt: 2026-11-11T00:21:00+09:00
- lastSeenAt: 2026-11-11T00:21:00+09:00

### GAP-Editor-EntityEditor-013

- status: open
- tier: P3
- score: 520
- primaryUseCase: UC-Authoring
- scenarioId: AUT-AssetLoop-001
- frequency: low
- frequencyBasis: シーン削除後の自動切替が先頭固定
- scope: UC-Authoring|Editor|EntityEditor|Incomplete|Project/Codes/Source/System/Editor/Entity/EntityEditor.cpp:261
- primary: `EntityEditor`
- summary: 【Authoring】削除後は remaining.front() のみ開く
- scenarioText: 反復作業（複製・一括調整）がエディタ専用 UI で過度な手順にならない。
- evidence:
  - 開いていた JSON 削除時 OpenSceneFromPath(remaining.front())（261-264 行）
  - ソート済み一覧の先頭のみ・ユーザー選択なし
  - 連続削除作業で意図しないシーンへ飛ぶ
- suggestedUse: 制作: シーン整理時の作業コンテキスト維持
- firstSeenAt: 2026-11-11T00:51:00+09:00
- lastSeenAt: 2026-11-11T00:51:00+09:00

### GAP-Editor-EntityEditorUi-019

- status: open
- tier: P3
- score: 520
- primaryUseCase: UC-Authoring
- scenarioId: AUT-AssetLoop-001
- frequency: medium
- frequencyBasis: 実行順が 1 Cmp ずつ手入力
- scope: UC-Authoring|Editor|EntityEditorUi|MissingUI|Project/Codes/Source/System/Editor/Entity/EntityEditorUi.cpp:745
- primary: `EntityEditorUi`
- summary: 【Authoring】InputInt で実行順が 1 スロットのみ
- scenarioText: 反復作業（複製・一括調整）がエディタ専用 UI で過度な手順にならない。
- evidence:
  - 選択中 slot の executionOrder のみ InputInt（745-747 行）
  - 同種 Cmp の一括 priority 調整 UI 無し
  - 複製反復で毎スロット手作業
- suggestedUse: 制作: Draw/Upd 層の実行順を一括整列
- firstSeenAt: 2026-11-11T00:51:00+09:00
- lastSeenAt: 2026-11-11T00:51:00+09:00

### GAP-Editor-EntityEditorDefinition-005

- status: open
- tier: P3
- score: 510
- primaryUseCase: UC-Authoring
- scenarioId: AUT-AssetLoop-001
- frequency: medium
- frequencyBasis: 型変更時 params 非マージ
- scope: UC-Authoring|Editor|EntityEditorDefinition|Incomplete|Project/Codes/Source/System/Editor/Entity/EntityEditorDefinition.cpp:103
- primary: `EntityEditorDefinition`
- summary: 【Authoring】from_json が params を丸ごと代入
- scenarioText: 反復作業（複製・一括調整）がエディタ専用 UI で過度な手順にならない。
- evidence:
  - 既知 type は kind だけ DefaultKind（103-105 行）
  - params は j.value のみで DefaultParams 未マージ（108 行）
  - 一括コンポ差替で欠損キーが残り手修正が増える
- suggestedUse: Authoring: テンプレ差替の既定値補完
- firstSeenAt: 2026-11-11T04:21:00+09:00
- lastSeenAt: 2026-11-11T04:21:00+09:00

### GAP-Editor-EntityEditorTagCatalog-002

- status: open
- tier: P3
- score: 510
- primaryUseCase: UC-Authoring
- scenarioId: AUT-AssetLoop-001
- frequency: medium
- frequencyBasis: カタログタグ改名が 1 語ずつ
- scope: UC-Authoring|Editor|EntityEditorTagCatalog|Incomplete|Project/Codes/Source/System/Editor/Entity/EntityEditorUi.cpp:837
- primary: `EntityEditorTagCatalog`
- summary: 【Authoring】改名は選択 1 タグのみ一括反映
- scenarioText: 反復作業（複製・一括調整）がエディタ専用 UI で過度な手順にならない。
- evidence:
  - RenameTagInDocument は document 全体走査だが UI は 1 選択（835-840 行）
  - 類似タグのプレフィックス一括や検索置換無し
  - TagCatalog-001 の一括付与と逆向きが手作業
- suggestedUse: 制作: ロールタグ命名規則の一括リネーム
- firstSeenAt: 2026-11-11T03:51:00+09:00
- lastSeenAt: 2026-11-11T03:51:00+09:00

### GAP-Editor-EntityEditorUi-021

- status: open
- tier: P3
- score: 510
- primaryUseCase: UC-Authoring
- scenarioId: AUT-AssetLoop-001
- frequency: high
- frequencyBasis: シーン一覧が選択＋別ボタン二段操作
- scope: UC-Authoring|Editor|EntityEditorUi|MissingUI|Project/Codes/Source/System/Editor/Entity/EntityEditorUi.cpp:454
- primary: `EntityEditorUi`
- summary: 【Authoring】Selectable はパス選択のみで即開かない
- scenarioText: 反復作業（複製・一括調整）がエディタ専用 UI で過度な手順にならない。
- evidence:
  - Selectable は selectedScenePath 更新のみ（454-456 行）
  - 実際のロードは「シーンを開く」別ボタン（462-464 行）
  - シーン切り替え反復で毎回 2 クリック
- suggestedUse: 制作: シーン一覧のダブルクリック／即開で往復短縮
- firstSeenAt: 2026-11-11T01:21:00+09:00
- lastSeenAt: 2026-11-11T01:21:00+09:00

### GAP-Editor-EntityEditorUi-024

- status: open
- tier: P3
- score: 510
- primaryUseCase: UC-Authoring
- scenarioId: AUT-AssetLoop-001
- frequency: medium
- frequencyBasis: Cmp 削除後に requestApplyPreview のみ
- scope: UC-Authoring|Editor|EntityEditorUi|Incomplete|Project/Codes/Source/System/Editor/Entity/EntityEditorUi.cpp:710
- primary: `EntityEditorUi`
- summary: 【Authoring】選択中コンポーネントのみ削除
- scenarioText: 反復作業（複製・一括調整）がエディタ専用 UI で過度な手順にならない。
- evidence:
  - erase 後 SyncSelectedComponentToKind のみ（707-711 行）
  - Disk 保存やプレビュー差分表示のフィードバック無し
  - Ui-018 の削除粒度と合わせ反復試行が視覚追跡しづらい
- suggestedUse: 制作: コンポ削除後のプレビュー更新を明示
- firstSeenAt: 2026-11-11T02:51:00+09:00
- lastSeenAt: 2026-11-11T02:51:00+09:00

### GAP-Editor-EditorEntityWorkbench-004

- status: open
- tier: P3
- score: 500
- primaryUseCase: UC-Authoring
- scenarioId: AUT-AssetLoop-001
- frequency: medium
- frequencyBasis: プレイモードはシミュレーションのみ
- scope: UC-Authoring|Editor|EditorEntityWorkbench|Incomplete|Project/Codes/Source/System/Editor/EditorEntityWorkbench.cpp:96
- primary: `EditorEntityWorkbench`
- summary: 【Authoring】メニュー「プレイ」は保存・実行を呼ばない
- scenarioText: 反復作業（複製・一括調整）がエディタ専用 UI で過度な手順にならない。
- evidence:
  - プレイ選択で previewSimulationEnabled のみ true（96-98 行）
  - シーンを実行は Save+requestPlayRuntimeScene（77-79 行）と別
  - Edit↔Play 反復で実行パスと挙動が二系統
- suggestedUse: 制作: プレイ試行の操作を一本化
- firstSeenAt: 2026-11-11T03:51:00+09:00
- lastSeenAt: 2026-11-11T03:51:00+09:00

### GAP-Editor-EntityEditor-015

- status: open
- tier: P3
- score: 500
- primaryUseCase: UC-Authoring
- scenarioId: AUT-AssetLoop-001
- frequency: medium
- frequencyBasis: 新規シーンが即ディスク保存
- scope: UC-Authoring|Editor|EntityEditor|Incomplete|Project/Codes/Source/System/Editor/Entity/EntityEditor.cpp:230
- primary: `EntityEditor`
- summary: 【Authoring】新規作成が requestSaveToDisk 必須
- scenarioText: 反復作業（複製・一括調整）がエディタ専用 UI で過度な手順にならない。
- evidence:
  - CreateNewSceneFile 末尾で requestSaveToDisk=true（230 行）
  - メモリ上ドラフトのまま試す経路無し
  - 試行錯誤のたびに Scenes 配下に JSON が増える
- suggestedUse: 制作: 保存前に複数案をメモリで試す
- firstSeenAt: 2026-11-11T01:51:00+09:00
- lastSeenAt: 2026-11-11T01:51:00+09:00

### GAP-Editor-EntityEditor-018

- status: open
- tier: P3
- score: 500
- primaryUseCase: UC-Authoring
- scenarioId: AUT-AssetLoop-001
- frequency: medium
- frequencyBasis: 親削除で子がルートに散る
- scope: UC-Authoring|Editor|EntityEditor|Incomplete|Project/Codes/Source/System/Editor/Entity/EntityEditor.cpp:65
- primary: `EntityEditor`
- summary: 【Authoring】削除時に子を祖父へ昇格しない
- scenarioText: 反復作業（複製・一括調整）がエディタ専用 UI で過度な手順にならない。
- evidence:
  - parentIndex==removeIndex の子は -1 のみ（65-66 行）
  - 削除ノードの親へ子を付け替える処理無し
  - グループ親を消すと配下がフラット化し再親付けが手作業
- suggestedUse: 制作: ヒエラルキー整理の削除を非破壊に
- firstSeenAt: 2026-11-11T02:51:00+09:00
- lastSeenAt: 2026-11-11T02:51:00+09:00

### GAP-Editor-EntityEditor-019

- status: open
- tier: P3
- score: 500
- primaryUseCase: UC-Authoring
- scenarioId: AUT-AssetLoop-001
- frequency: medium
- frequencyBasis: 起動シーン設定が暗黙 Save を伴う
- scope: UC-Authoring|Editor|EntityEditor|Incomplete|Project/Codes/Source/System/Editor/Entity/EntityEditorUi.cpp:412
- primary: `EntityEditor`
- summary: 【Authoring】起動設定前に SaveToJsonPath
- scenarioText: 反復作業（複製・一括調整）がエディタ専用 UI で過度な手順にならない。
- evidence:
  - 起動シーンに設定で target 非空なら Save（407-413 行）
  - 未保存プレビュー差分がディスクへ確定
  - Save 専用ボタンと操作が非対称（EntityEditor-016 と関連）
- suggestedUse: 制作: launch 設定と明示保存の分離
- firstSeenAt: 2026-11-11T03:21:00+09:00
- lastSeenAt: 2026-11-11T03:21:00+09:00

### GAP-Editor-PrefabAsset-005

- status: open
- tier: P3
- score: 500
- primaryUseCase: UC-Authoring
- scenarioId: AUT-AssetLoop-001
- frequency: medium
- frequencyBasis: シーン切替でプレハブ選択が残る
- scope: UC-Authoring|Editor|PrefabAsset|Incomplete|Project/Codes/Source/System/Editor/Entity/EntityEditor.cpp:233
- primary: `PrefabAsset`
- summary: 【Authoring】OpenScene が selectedPrefabPath をクリアしない
- scenarioText: 反復作業（複製・一括調整）がエディタ専用 UI で過度な手順にならない。
- evidence:
  - OpenSceneFromPath は Load と selectedScenePath のみ（233-240 行）
  - 別シーンで前シーンのプレハブが選択表示のまま
  - 誤って「シーンへ追加」で異シーン文脈へ投入しうる
- suggestedUse: 制作: シーン切替時にプレハブ選択をリセット
- firstSeenAt: 2026-11-11T02:21:00+09:00
- lastSeenAt: 2026-11-11T02:21:00+09:00

### GAP-Editor-EditorImportedAsset-003

- status: open
- tier: P3
- score: 490
- primaryUseCase: UC-Authoring
- scenarioId: AUT-AssetLoop-001
- frequency: medium
- frequencyBasis: 同名ドロップで連番ファイル増殖
- scope: UC-Authoring|Editor|EditorImportedAsset|Incomplete|Project/Codes/Source/System/Editor/EditorImportedAsset.cpp:53
- primary: `EditorImportedAsset`
- summary: 【Authoring】重複時に stem_N へ自動リネーム
- scenarioText: 反復作業（複製・一括調整）がエディタ専用 UI で過度な手順にならない。
- evidence:
  - exists(dest) 時 suffix 1..9999 で新ファイル（53-59 行）
  - 上書き／差し替え確認 UI 無し
  - 同 obj 再ドロップで Imported が散逸しパス更新が煩雑
- suggestedUse: 制作: 反復ドロップ時の上書き方針
- firstSeenAt: 2026-11-11T03:21:00+09:00
- lastSeenAt: 2026-11-11T03:21:00+09:00

### GAP-Editor-EditorImportedAsset-004

- status: open
- tier: P3
- score: 490
- primaryUseCase: UC-Authoring
- scenarioId: AUT-AssetLoop-001
- frequency: low
- frequencyBasis: 衝突時の連番リネームのみ
- scope: UC-Authoring|Editor|EditorImportedAsset|Incomplete|Project/Codes/Source/System/Editor/EditorImportedAsset.cpp:57
- primary: `EditorImportedAsset`
- summary: 【Authoring】衝突解決が stem_N 連番のみ
- scenarioText: 反復作業（複製・一括調整）がエディタ専用 UI で過度な手順にならない。
- evidence:
  - exists(dest) 時 stem+連番+ext のみ（53-62 行）
  - バッチ全体の接頭辞やタイムスタンプを選べない
  - 大量同名アセットの整理に外部リネームが必要
- suggestedUse: Authoring: 一括インポート命名規則
- firstSeenAt: 2026-11-11T04:21:00+09:00
- lastSeenAt: 2026-11-11T04:21:00+09:00

### GAP-Editor-EntityEditor-017

- status: open
- tier: P3
- score: 490
- primaryUseCase: UC-Authoring
- scenarioId: AUT-AssetLoop-001
- frequency: medium
- frequencyBasis: 同一フレーム複数ドロップが最後だけ残る
- scope: UC-Authoring|Editor|EntityEditor|Incomplete|Project/Codes/Source/System/Editor/Entity/EntityEditor.cpp:288
- primary: `EntityEditor`
- summary: 【Authoring】ループ内で lastImported が上書き
- scenarioText: 反復作業（複製・一括調整）がエディタ専用 UI で過度な手順にならない。
- evidence:
  - for 各 dropped で lastImportedAssetPath 代入（280-289 行）
  - 複数ファイル同時ドロップで直近 1 件のみ UI に残る
  - Ui-023 の適用はさらに 1 スロットずつ
- suggestedUse: 制作: 一括ドロップをキュー表示して順適用
- firstSeenAt: 2026-11-11T02:21:00+09:00
- lastSeenAt: 2026-11-11T02:21:00+09:00

### GAP-Editor-EditorEntityWorkbench-003

- status: open
- tier: P3
- score: 480
- primaryUseCase: UC-Authoring
- scenarioId: AUT-AssetLoop-001
- frequency: low
- frequencyBasis: Dock 既定レイアウトが初回固定
- scope: UC-Authoring|Editor|EditorEntityWorkbench|Incomplete|Project/Codes/Source/System/Editor/EditorEntityWorkbench.cpp:211
- primary: `EditorEntityWorkbench`
- summary: 【Authoring】defaultLayoutBuilt が static で一回切り
- scenarioText: 反復作業（複製・一括調整）がエディタ専用 UI で過度な手順にならない。
- evidence:
  - static defaultLayoutBuilt は初回のみ DockBuilder（211-240 行）
  - レイアウトリセット／プリセット保存 UI 無し
  - パネル配置を戻すには imgui.ini 手削除依存
- suggestedUse: 制作: 作業切替で Dock を既定に戻す
- firstSeenAt: 2026-11-11T01:51:00+09:00
- lastSeenAt: 2026-11-11T01:51:00+09:00

### GAP-Editor-EntityEditor-020

- status: open
- tier: P3
- score: 480
- primaryUseCase: UC-Authoring
- scenarioId: AUT-AssetLoop-001
- frequency: medium
- frequencyBasis: 一括 D&D 後プレビュー未更新
- scope: UC-Authoring|Editor|EntityEditor|Incomplete|Project/Codes/Source/System/Editor/Entity/EntityEditor.cpp:278
- primary: `EntityEditor`
- summary: 【Authoring】インポート後プレビュー再適用なし
- scenarioText: 反復作業（複製・一括調整）がエディタ専用 UI で過度な手順にならない。
- evidence:
  - ProcessDroppedFileImports はコピーと status のみ（271-290 行）
  - requestApplyPreview を立てず手動プレビュー待ち
  - 複数ファイル時も最後の path だけ UiState に残る
- suggestedUse: Authoring: D&D 直後のビュー反映を自動化
- firstSeenAt: 2026-11-11T04:21:00+09:00
- lastSeenAt: 2026-11-11T04:21:00+09:00

#### UC-Debug

### GAP-Render-Lighting-001

- status: open
- tier: P1
- score: 820
- primaryUseCase: UC-Debug
- scenarioId: DBG-RenderDiag-001
- frequency: high
- frequencyBasis: ライティング診断 ImGui がエンジン UI から呼ばれない
- scope: UC-Debug|Render|Lighting|Unwired|Project/Codes/Source/Engine/Renderer/Lighting/Lighting.cpp:46
- primary: `Lighting`
- summary: 【Debug】Lighting::ImGui（フォグ／ゴッドレイ／影）が未配線
- scenarioText: 開発者がシャドウ・ゴッドレイ等の描画結果をログ／オーバーレイで確認できる（spec: `feature-memos/directional_shadow.md`, `feature-memos/god_rays.md`）。
- evidence:
  - Lighting::ImGui は VolumetricFog/GodRays/Shadow ヘッダーを描画（46-56 行）
  - Codes 全体で Lighting::ImGui 呼び出し無し（rg）
- suggestedUse: デバッグ: Development で CB 反映後のライトパラメータを即確認
- roadmapLink: feature-memos/god_rays.md
- firstSeenAt: 2026-11-10T02:31:00+09:00
- lastSeenAt: 2026-11-10T02:31:00+09:00

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

### GAP-Audio-StreamingDebug-001

- status: open
- tier: P1
- score: 800
- primaryUseCase: UC-Debug
- scenarioId: DBG-Streaming-001
- frequency: high
- frequencyBasis: remaining 肥大が progress バーで 1.0 に潰され切り分けできない
- scope: UC-Debug|Audio|StreamingDebug|Incomplete|Project/Codes/Source/Audio/Audio.cpp:1299
- primary: `StreamingDebug`
- summary: 【Debug】decoderBacklog01 のクランプで remaining 肥大が見えない
- scenarioText: 開発者がストリーミング再生中のメモリ・進捗を確認し、枯渇や remaining 肥大を切り分けられる（spec: `feature-memos/streaming_debug_imgui.md`）。
- evidence:
  - decoderBacklog01 = clamp(remaining/kBufferSize, 0, 1)（Audio.cpp:1299-1302）
  - ImGui は ProgressBar(decoderBacklog01) のみ（AudioStreamingDebugImGui.cpp:119）
- suggestedUse: デバッグ: remaining がバッファ複数分に膨らんだ異常を即判別
- roadmapLink: feature-memos/streaming_debug_imgui.md
- firstSeenAt: 2026-11-10T02:41:00+09:00
- lastSeenAt: 2026-11-10T02:41:00+09:00

### GAP-Editor-EditorEntityWorkbench-001

- status: open
- tier: P1
- score: 800
- primaryUseCase: UC-Debug
- secondaryUseCases: UC-Authoring
- scenarioId: DBG-FeatureScene-001
- frequency: high
- frequencyBasis: 機能検証シーンへの導線がパーティクルエディタのみ
- scope: UC-Debug|Editor|EditorEntityWorkbench|MissingUI|Project/Codes/Source/System/Editor/EditorEntityWorkbench.cpp:110
- primary: `EditorEntityWorkbench`
- summary: 【Debug】エディタメニューに機能検証シーン起動が無い
- scenarioText: 開発者がエディタ既定の機能検証シーンから、エディタ外ランタイム機能だけを最小構成で試せる。
- evidence:
  - 「エディタ」メニューはパーティクル／コードテンプレのみ（110-117 行）
  - F2 で SceneParticleEditor 遷移はあるが機能検証用 SceneBuildUp 起動無し
- suggestedUse: デバッグ: 既定のミニ検証シーンをメニューから開く
- firstSeenAt: 2026-11-10T03:11:00+09:00
- lastSeenAt: 2026-11-10T03:11:00+09:00

### GAP-Infra-frame_profiler-001

- status: open
- tier: P1
- score: 800
- primaryUseCase: UC-Debug
- scenarioId: DBG-Profiler-001
- frequency: high
- frequencyBasis: フレーム内 CPU／GPU 区間計測の仕組みが無い
- scope: UC-Debug|Infra|frame_profiler|MissingData|Project/Codes/Source/Engine/Renderer/Draw.cpp:64
- primary: `frame_profiler`
- summary: 【Debug】サブシステム別フレームプロファイラが未実装
- scenarioText: Development ビルドでフレームコストの当たりを付け、本番相当設定と切り分けられる。
- evidence:
  - Codes 配下に Profiler/Tracy/GPU timestamp クエリ参照無し
  - Draw/GameOrder に区間計測 API 無し
- suggestedUse: デバッグ: Update/Draw/オーディオのどこが重いか即座に特定
- firstSeenAt: 2026-11-10T02:21:00+09:00
- lastSeenAt: 2026-11-10T02:21:00+09:00

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

### GAP-Engine-AtrumEngineLighting-001

- status: open
- tier: P1
- score: 790
- primaryUseCase: UC-Debug
- scenarioId: DBG-RenderDiag-001
- frequency: high
- frequencyBasis: Engine 側 VolumetricFog/GodRays/Shadow の ImGui 入口が Editor に無い
- scope: UC-Debug|Engine|AtrumEngineLighting|MissingUI|Project/Codes/Include/Engine/AtrumEngine.h:535
- primary: `AtrumEngineLighting`
- summary: 【Debug】描画診断用ライティングパネルが EditorFrame に無い
- scenarioText: 開発者がシャドウ・ゴッドレイ等の描画結果をログ／オーバーレイで確認できる（spec: `feature-memos/directional_shadow.md`, `feature-memos/god_rays.md`）。
- evidence:
  - AtrumEngine::RefVolumetricFog 等は ImGui() を公開（各 Visual ヘッダ）
  - EditorFrame に Light/Fog/Shadow メニュー参照無し
  - SceneGame で設定した値をランタイムで見る UI が無い
- suggestedUse: デバッグ: シーン実行中にライティング結果をオーバーレイ確認
- firstSeenAt: 2026-11-10T02:31:00+09:00
- lastSeenAt: 2026-11-10T02:31:00+09:00

### GAP-Debug-SceneViewTab-001

- status: open
- tier: P1
- score: 760
- primaryUseCase: UC-Debug
- scenarioId: DBG-FeatureScene-001
- frequency: high
- frequencyBasis: Game/Runtime タブはエディタプレビューのみで本番ランタイム経路と異なる
- scope: UC-Debug|Debug|SceneViewTab|Incomplete|Project/Codes/Source/Scene/SceneEntityEditor.cpp:260
- primary: `SceneViewTab`
- summary: 【Debug】実行ビューがプレビュー Entity のみで機能単体試験にならない
- scenarioText: 開発者がエディタ既定の機能検証シーンから、エディタ外ランタイム機能だけを最小構成で試せる。
- evidence:
  - DrawSceneViewContent は Game/Runtime も DrawPreviewEntities のみ（260 行）
  - 本番相当の SceneBuildUp::Update/DrawScene 経路に切替える UI 無し
- suggestedUse: デバッグ: エディタ外ランタイムだけをタブから切り出して試す
- firstSeenAt: 2026-11-10T03:11:00+09:00
- lastSeenAt: 2026-11-10T03:11:00+09:00

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

### GAP-Infra-EditorScenePlayRequest-001

- status: open
- tier: P1
- score: 740
- primaryUseCase: UC-Debug
- scenarioId: DBG-FeatureScene-001
- frequency: medium
- frequencyBasis: EntityEditor→Runtime の遷移キューが未接続
- scope: UC-Debug|Infra|EditorScenePlayRequest|Unwired|Project/Codes/Include/System/Editor/Scene/EditorSceneAsset.h:28
- primary: `EditorScenePlayRequest`
- summary: 【Debug】EditorScenePlayRequest がどこからも Set されない
- scenarioText: 開発者がエディタ既定の機能検証シーンから、エディタ外ランタイム機能だけを最小構成で試せる。
- evidence:
  - EditorScenePlayRequest は JSON パス保持のみ（EditorSceneAsset.h:27-38）
  - rg Set(.*EditorScenePlayRequest 0 件、Instance() は .cpp 内のみ
- suggestedUse: デバッグ: シーン実行で本番相当 BuildUp へ JSON を渡す
- roadmapLink: feature-memos/debug_test_play_area.md
- firstSeenAt: 2026-11-10T04:41:00+09:00
- lastSeenAt: 2026-11-10T04:41:00+09:00

### GAP-Debug-FrameDeltaTime-001

- status: open
- tier: P2
- score: 720
- primaryUseCase: UC-Debug
- scenarioId: DBG-Profiler-001
- frequency: high
- frequencyBasis: フレーム Δt はあるが可視化・スパイク検出 UI が無い
- scope: UC-Debug|Debug|FrameDeltaTime|MissingUI|Project/Codes/Include/System/DeltaTime.h:35
- primary: `FrameDeltaTime`
- summary: 【Debug】フレーム時間のオーバーレイ／ヒストグラムが無い
- scenarioText: Development ビルドでフレームコストの当たりを付け、本番相当設定と切り分けられる。
- evidence:
  - FrameDeltaTime はゲームロジック用のみ（ForDebug から参照無し）
  - ImGui に FPS/ms 表示ウィンドウ無し（Audio ストリーミング専用窓のみ）
- suggestedUse: デバッグ: ヒッチや平均フレーム時間を Development で監視
- firstSeenAt: 2026-11-10T02:21:00+09:00
- lastSeenAt: 2026-11-10T02:21:00+09:00

### GAP-Debug-StreamingMenu-001

- status: open
- tier: P2
- score: 710
- primaryUseCase: UC-Debug
- scenarioId: DBG-Streaming-001
- frequency: medium
- frequencyBasis: ストリーミング窓トグルが EntityEditor メニュー限定
- scope: UC-Debug|Debug|StreamingMenu|Unwired|Project/Codes/Source/System/Editor/EditorEntityWorkbench.cpp:120
- primary: `StreamingMenu`
- summary: 【Debug】ストリーミングデバッグ ON が SceneBuildUp 経路で開けない
- scenarioText: 開発者がストリーミング再生中のメモリ・進捗を確認し、枯渇や remaining 肥大を切り分けられる（spec: `feature-memos/streaming_debug_imgui.md`）。
- evidence:
  - メニュー「デバッグ→ストリーミングメモリ」は DrawMainMenuBar のみ（120-125 行）
  - SceneBuildUp は LayoutEditorWorkbench を呼ばない（SceneBuildUp.cpp Draw）
- suggestedUse: デバッグ: BuildUp ランタイムプレビューでもストリーミング計測を有効化
- roadmapLink: feature-memos/streaming_debug_imgui.md
- firstSeenAt: 2026-11-10T02:41:00+09:00
- lastSeenAt: 2026-11-10T02:41:00+09:00

### GAP-Scene-SceneCameraRig-001

- status: open
- tier: P1
- score: 710
- primaryUseCase: UC-Debug
- scenarioId: DBG-FeatureScene-001
- frequency: high
- frequencyBasis: primary 解決が rig 内部 entities のみ
- scope: UC-Debug|Scene|SceneCameraRig|Contract|Project/Codes/Source/Scene/SceneCameraRig.cpp:76
- primary: `SceneCameraRig`
- summary: 【Debug】FindPrimaryCmpCamera が rig 配列のみ走査
- scenarioText: 開発者がエディタ既定の機能検証シーンから、エディタ外ランタイム機能だけを最小構成で試せる。
- evidence:
  - FindPrimaryCmpCamera は entities_ ベクタのみ（76-88 行）
  - SceneBuildUp の JSON 実体は別 vector（GAP-Scene-SceneBuildUp-009）
  - 機能検証で JSON の camera.main を primary にできない
- suggestedUse: Debug: シーンロード実体と rig primary を統合
- roadmapLink: feature-memos/entity_camera_components.md
- firstSeenAt: 2026-11-11T00:01:00+09:00
- lastSeenAt: 2026-11-11T00:01:00+09:00

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

### GAP-Scene-SceneBuildUp-009

- status: open
- tier: P1
- score: 700
- primaryUseCase: UC-Debug
- scenarioId: DBG-FeatureScene-001
- frequency: high
- frequencyBasis: JSON 実体と Scene::cameraRig_ が別配列
- scope: UC-Debug|Scene|SceneBuildUp|Contract|Project/Codes/Source/Scene/SceneBuildUp.cpp:109
- primary: `SceneBuildUp`
- summary: 【Debug】entities_ と GetCamera の cameraRig が未接続
- scenarioText: 開発者がエディタ既定の機能検証シーンから、エディタ外ランタイム機能だけを最小構成で試せる。
- evidence:
  - EnterScene は entities_ のみ構築（109 行）
  - GetCamera は cameraRig_.GetPrimaryCamera（Scene.cpp:43）
  - cameraRig は EnsureAutoSpawnedCameras で既定 MainCamera を別生成し JSON の CmpCamera を描画対象にしない
- suggestedUse: Debug: JSON 上の CmpCamera を機能検証の主カメラにする
- roadmapLink: feature-memos/entity_camera_components.md
- firstSeenAt: 2026-11-10T23:31:00+09:00
- lastSeenAt: 2026-11-10T23:31:00+09:00

### GAP-Audio-StreamingDebug-002

- status: open
- tier: P2
- score: 690
- primaryUseCase: UC-Debug
- scenarioId: DBG-Streaming-001
- frequency: medium
- frequencyBasis: スナップショットのファイルログ出力が無い
- scope: UC-Debug|Audio|StreamingDebug|MissingData|Project/Codes/Source/ForDebug/AudioStreamingDebugImGui.cpp:69
- primary: `StreamingDebug`
- summary: 【Debug】ストリーミング診断が ImGui 表示のみでログに残らない
- scenarioText: 開発者がストリーミング再生中のメモリ・進捗を確認し、枯渇や remaining 肥大を切り分けられる（spec: `feature-memos/streaming_debug_imgui.md`）。
- evidence:
  - CollectStreamingMemoryDebug 後 ImGui テーブル描画のみ（69-133 行）
  - scenario が想定する「ログ」経路（LogFile 等）無し
- suggestedUse: デバッグ: 再生異常をログで再現・共有
- roadmapLink: feature-memos/streaming_debug_imgui.md
- firstSeenAt: 2026-11-10T02:41:00+09:00
- lastSeenAt: 2026-11-10T02:41:00+09:00

### GAP-Ship-Development_opt-001

- status: open
- tier: P2
- score: 690
- primaryUseCase: UC-Debug
- secondaryUseCases: UC-Ship
- scenarioId: DBG-Profiler-001
- frequency: medium
- frequencyBasis: Development は最適化無効＋ImGui で Release と性能特性が一致しない
- scope: UC-Debug|Ship|Development_opt|Contract|Project/CG2_BuildUp.vcxproj:171
- primary: `Development_opt`
- summary: 【Debug】本番相当プロファイル計測用の構成／手順が未定義
- scenarioText: Development ビルドでフレームコストの当たりを付け、本番相当設定と切り分けられる。
- evidence:
  - Development|x64: Optimization Disabled＋USE_IMGUI（171-164 行）
  - Release|x64: NDEBUG＋MinSpace（125-132 行）
  - プロファイル切り分けの feature-memo／vcxproj プロファイル無し
- suggestedUse: デバッグ: 最適化とエディタ OFF で Release に近い計測を再現
- firstSeenAt: 2026-11-10T02:21:00+09:00
- lastSeenAt: 2026-11-10T02:21:00+09:00

### GAP-Editor-EditorSceneRuntime-007

- status: open
- tier: P1
- score: 680
- primaryUseCase: UC-Debug
- scenarioId: DBG-FeatureScene-001
- frequency: high
- frequencyBasis: ロード経路が MainCamera 自動スポーンを含まない
- scope: UC-Debug|Editor|EditorSceneRuntime|Incomplete|Project/Codes/Source/System/Editor/Scene/EditorSceneRuntime.cpp:354
- primary: `EditorSceneRuntime`
- summary: 【Debug】BuildEntities が camera.main 欠落を補わない
- scenarioText: 開発者がエディタ既定の機能検証シーンから、エディタ外ランタイム機能だけを最小構成で試せる。
- evidence:
  - BuildEntitiesFromSceneDocument は instances のみ（354-380 行）
  - Scene::cameraRig 自動スポーンは別経路（SceneBuildUp-009）
  - Release BuildUp と Editor プレビューでカメラ解決が二重
- suggestedUse: Debug: JSON ロード経路を本番と同じ camera 契約に
- roadmapLink: feature-memos/entity_camera_components.md
- firstSeenAt: 2026-11-11T01:01:00+09:00
- lastSeenAt: 2026-11-11T01:01:00+09:00

### GAP-Render-DirectionalShadow-001

- status: open
- tier: P2
- score: 680
- primaryUseCase: UC-Debug
- scenarioId: DBG-RenderDiag-001
- frequency: medium
- frequencyBasis: 平面影描画の成否や shadowMap 診断が無い
- scope: UC-Debug|Render|DirectionalShadow|Incomplete|Project/Codes/Source/Engine/Renderer/Lighting/DirectionalShadow.cpp:34
- primary: `DirectionalShadow`
- summary: 【Debug】シャドウ／シェード適用状態のログ・可視化が無い
- scenarioText: 開発者がシャドウ・ゴッドレイ等の描画結果をログ／オーバーレイで確認できる（spec: `feature-memos/directional_shadow.md`, `feature-memos/god_rays.md`）。
- evidence:
  - DrawPlanarGroundShadow は無言 return のみ（37-38 行）
  - directional_shadow.md の shadowMap SRV 診断は「今後」（21-23 行）
  - CAST_SHADOW フラグのオーバーレイ表示無し
- suggestedUse: デバッグ: 影が出ない原因をフラグ／描画回数で切り分け
- roadmapLink: feature-memos/directional_shadow.md
- firstSeenAt: 2026-11-10T02:31:00+09:00
- lastSeenAt: 2026-11-10T02:31:00+09:00

### GAP-Scene-CommandScene-003

- status: open
- tier: P2
- score: 670
- primaryUseCase: UC-Debug
- scenarioId: DBG-FeatureScene-001
- frequency: high
- frequencyBasis: Development 起動が EntityEditor 固定で Feature 検証シーンに直行できない
- scope: UC-Debug|Scene|CommandScene|MissingData|Project/Codes/Source/Scene/CommandScene.cpp:17
- primary: `CommandScene`
- summary: 【Debug】起動時シーンが SceneEntityEditor に固定
- scenarioText: 開発者がエディタ既定の機能検証シーンから、エディタ外ランタイム機能だけを最小構成で試せる。
- evidence:
  - DEVELOPMENT&&USE_IMGUI で sceneNow_=SceneEntityEditor::Instance()（17 行）
  - SceneBuildUpRegistry::Launch や template_test_play_area への分岐無し
  - CommandScene-002 はプレイ観点、既定 Feature シーン導線は未言及
- suggestedUse: デバッグ: ビルド起動直後から最小ランタイム検証に入れる
- roadmapLink: feature-memos/debug_test_play_area.md
- firstSeenAt: 2026-11-10T06:31:00+09:00
- lastSeenAt: 2026-11-10T06:31:00+09:00

### GAP-Editor-EditorSceneRuntime-008

- status: open
- tier: P2
- score: 640
- primaryUseCase: UC-Debug
- scenarioId: DBG-FeatureScene-001
- frequency: high
- frequencyBasis: BuildUp 共有ローダがタグ同期しない
- scope: UC-Debug|Editor|EditorSceneRuntime|Incomplete|Project/Codes/Source/System/Editor/Scene/EditorSceneRuntime.cpp:321
- primary: `EditorSceneRuntime`
- summary: 【Debug】TryLoad が tagCatalog 同期を省略
- scenarioText: 開発者がエディタ既定の機能検証シーンから、エディタ外ランタイム機能だけを最小構成で試せる。
- evidence:
  - JSON parse のみで Merge/Sync 無し（321-350 行）
  - EntityEditor::LoadFromJsonPath は同期あり（171-173 行）
  - SceneBuildUp-018 と同系だがローダ共通層の欠落
- suggestedUse: Debug: 機能検証 JSON のタグ契約をエディタと一致
- roadmapLink: feature-memos/debug_test_play_area.md
- firstSeenAt: 2026-11-11T03:01:00+09:00
- lastSeenAt: 2026-11-11T03:01:00+09:00

### GAP-Scene-SceneBuildUp-006

- status: open
- tier: P2
- score: 630
- primaryUseCase: UC-Debug
- scenarioId: DBG-FeatureScene-001
- frequency: medium
- frequencyBasis: 単体 Launch で戻り先無しのとき F1 復帰が効かない
- scope: UC-Debug|Scene|SceneBuildUp|Contract|Project/Codes/Source/Scene/SceneBuildUp.cpp:114
- primary: `SceneBuildUp`
- summary: 【Debug】returnScene が null だと機能検証シーンから抜けられない
- scenarioText: 開発者がエディタ既定の機能検証シーンから、エディタ外ランタイム機能だけを最小構成で試せる。
- evidence:
  - F1 復帰は returnScene_ != nullptr のときのみ（114-118 行）
  - README の Launch 例は returnScene 必須だが UI 導線未整備時に単体起動しうる
- suggestedUse: デバッグ: 検証シーンのみ起動した後もエディタへ戻れる
- roadmapLink: feature-memos/debug_test_play_area.md
- firstSeenAt: 2026-11-10T04:41:00+09:00
- lastSeenAt: 2026-11-10T04:41:00+09:00

### GAP-Editor-SceneEntityEditor-006

- status: open
- tier: P2
- score: 620
- primaryUseCase: UC-Debug
- scenarioId: DBG-FeatureScene-001
- frequency: medium
- frequencyBasis: パーティクル用ショートカットのみで BuildUp 起動無し
- scope: UC-Debug|Editor|SceneEntityEditor|MissingUI|Project/Codes/Source/Scene/SceneEntityEditor.cpp:147
- primary: `SceneEntityEditor`
- summary: 【Debug】F2 はパーティクル遷移のみで機能検証シーン未割当
- scenarioText: 開発者がエディタ既定の機能検証シーンから、エディタ外ランタイム機能だけを最小構成で試せる。
- evidence:
  - F2 で SceneParticleEditor へ ChangeScene（147-150 行）
  - SceneBuildUpRegistry::Launch や DebugPlayArea 既定パスへのホットキー無し
  - 「シーンを実行」はプレビュータブ内のみ（129-136 行）
- suggestedUse: デバッグ: ワンキーで最小ランタイム検証へ入る
- roadmapLink: feature-memos/debug_test_play_area.md
- firstSeenAt: 2026-11-10T07:01:00+09:00
- lastSeenAt: 2026-11-10T07:01:00+09:00

### GAP-Infra-LoopTick13Demo-001

- status: open
- tier: P2
- score: 620
- primaryUseCase: UC-Debug
- scenarioId: DBG-FeatureScene-001
- frequency: low
- frequencyBasis: デモ用 scene JSON が既定検証シーンに未登録
- scope: UC-Debug|Infra|LoopTick13Demo|MissingData|EditorAssets/DebugPlayArea/LoopTick13Demo.scene.json:1
- primary: `LoopTick13Demo`
- summary: 【Debug】LoopTick13Demo が機能検証シーンの正本として未整備
- scenarioText: 開発者がエディタ既定の機能検証シーンから、エディタ外ランタイム機能だけを最小構成で試せる。
- evidence:
  - EditorAssets/DebugPlayArea/LoopTick13Demo.scene.json は存在するが Codes から参照無し
  - CommandScene 初期シーンは EntityEditor のみ（FeatureTest 既定パス無し）
- suggestedUse: デバッグ: ループ検証用の既定 JSON を機能検証の種にする
- firstSeenAt: 2026-11-10T03:11:00+09:00
- lastSeenAt: 2026-11-10T03:11:00+09:00

### GAP-Scene-SceneBuildUp-013

- status: open
- tier: P2
- score: 620
- primaryUseCase: UC-Debug
- scenarioId: DBG-FeatureScene-001
- frequency: medium
- frequencyBasis: JSON 読込失敗がダミーシーンに隠蔽
- scope: UC-Debug|Scene|SceneBuildUp|MissingData|Project/Codes/Source/Scene/SceneBuildUp.cpp:96
- primary: `SceneBuildUp`
- summary: 【Debug】ロード失敗時に空の Entity_0 を生成
- scenarioText: 開発者がエディタ既定の機能検証シーンから、エディタ外ランタイム機能だけを最小構成で試せる。
- evidence:
  - TryLoadSceneDocument 失敗で MakeDefaultEntityDefinition（96-102 行）
  - エラー表示や Launch 中止が無い
  - 壊れた機能検証 JSON でもプレイヤブルに見える
- suggestedUse: Debug: 検証シーン JSON 不整合を即検知
- roadmapLink: feature-memos/debug_test_play_area.md
- firstSeenAt: 2026-11-11T01:01:00+09:00
- lastSeenAt: 2026-11-11T01:01:00+09:00

### GAP-Editor-SceneEntityEditor-007

- status: open
- tier: P2
- score: 610
- primaryUseCase: UC-Debug
- scenarioId: DBG-FeatureScene-001
- frequency: high
- frequencyBasis: シーン実行がタブフォーカス変更で終わる
- scope: UC-Debug|Editor|SceneEntityEditor|Incomplete|Project/Codes/Source/Scene/SceneEntityEditor.cpp:136
- primary: `SceneEntityEditor`
- summary: 【Debug】シーン実行後は Runtime タブフォーカスのみ
- scenarioText: 開発者がエディタ既定の機能検証シーンから、エディタ外ランタイム機能だけを最小構成で試せる。
- evidence:
  - requestPlayRuntimeScene 処理は preview 再構築＋タブ切替（129-136 行）
  - SceneBuildUpRegistry や DebugPlayArea パスへの遷移無し
  - requestFocusRuntimeSceneView フラグのみ立てる
- suggestedUse: デバッグ: 実行＝BuildUp ランタイムシーンへ
- roadmapLink: feature-memos/debug_test_play_area.md
- firstSeenAt: 2026-11-10T07:31:00+09:00
- lastSeenAt: 2026-11-10T07:31:00+09:00

### GAP-Scene-SceneBuildUp-011

- status: open
- tier: P2
- score: 610
- primaryUseCase: UC-Debug
- scenarioId: DBG-FeatureScene-001
- frequency: high
- frequencyBasis: Update が JSON 実体ベクタのみ
- scope: UC-Debug|Scene|SceneBuildUp|Incomplete|Project/Codes/Source/Scene/SceneBuildUp.cpp:125
- primary: `SceneBuildUp`
- summary: 【Debug】cameraRig 実体は Update されない
- scenarioText: 開発者がエディタ既定の機能検証シーンから、エディタ外ランタイム機能だけを最小構成で試せる。
- evidence:
  - Update は entities_ の Active のみ（125-128 行）
  - EnsureAutoSpawnedCameras の MainCamera は rig 側のみ（SceneCameraRig.cpp:61-62）
  - PrepareBeforeDraw 以外の Cmp ロジックが機能検証で欠落しうる
- suggestedUse: Debug: 自動スポーンカメラもランタイム更新対象に
- roadmapLink: feature-memos/entity_camera_components.md
- firstSeenAt: 2026-11-11T00:31:00+09:00
- lastSeenAt: 2026-11-11T00:31:00+09:00

### GAP-Entity-CmpCamera-001

- status: open
- tier: P2
- score: 600
- primaryUseCase: UC-Debug
- scenarioId: DBG-FeatureScene-001
- frequency: medium
- frequencyBasis: Cmp 初期化が正射影行列固定
- scope: UC-Debug|Entity|CmpCamera|Contract|Project/Codes/Source/System/Entity/Components/CmpCamera.cpp:19
- primary: `CmpCamera`
- summary: 【Debug】Initialize が Orthographic を既定生成
- scenarioText: 開発者がエディタ既定の機能検証シーンから、エディタ外ランタイム機能だけを最小構成で試せる。
- evidence:
  - CreateOrthographicMatrix が Initialize 直後（19-20 行）
  - BuildUp RT は FitCameraPerspectiveToViewport（SceneBuildUp.cpp:152）
  - 機能検証で JSON カメラの透視パラメータを試しにくい
- suggestedUse: Debug: ステージビュー透視と Cmp 既定の整合
- roadmapLink: feature-memos/entity_camera_components.md
- firstSeenAt: 2026-11-11T00:31:00+09:00
- lastSeenAt: 2026-11-11T00:31:00+09:00

### GAP-Infra-EditorScenePlayRequest-002

- status: open
- tier: P2
- score: 600
- primaryUseCase: UC-Debug
- scenarioId: DBG-FeatureScene-001
- frequency: medium
- frequencyBasis: PlayRequest の読み取り口が未使用で遷移キューが死蔵
- scope: UC-Debug|Infra|EditorScenePlayRequest|Unwired|Project/Codes/Include/System/Editor/Scene/EditorSceneAsset.h:33
- primary: `EditorScenePlayRequest`
- summary: 【Debug】EditorScenePlayRequest::Path が参照されない
- scenarioText: 開発者がエディタ既定の機能検証シーンから、エディタ外ランタイム機能だけを最小構成で試せる。
- evidence:
  - Path() は jsonFilePath_ を返すのみ（EditorSceneAsset.h:33）
  - Set/Clear は .cpp にあるが Editor／Scene 配下で Path() 呼び出し 0 件
  - SceneBuildUp は EditorSceneAsset を include するが PlayRequest を消費しない
- suggestedUse: デバッグ: エディタから渡した JSON を BuildUp 起動に反映
- roadmapLink: feature-memos/debug_test_play_area.md
- firstSeenAt: 2026-11-10T06:31:00+09:00
- lastSeenAt: 2026-11-10T06:31:00+09:00

### GAP-Infra-combat_bounds_sandbox-001

- status: open
- tier: P2
- score: 600
- primaryUseCase: UC-Debug
- scenarioId: DBG-FeatureScene-001
- frequency: low
- frequencyBasis: StageBounds 可視化サンドボックスが Docs のみ
- scope: UC-Debug|Infra|combat_bounds_sandbox|MissingData|EditorAssets/DebugPlayArea/combat_bounds_sandbox.scene.json:1
- primary: `combat_bounds_sandbox`
- summary: 【Debug】combat_bounds_sandbox がコード／既定導線から参照されない
- scenarioText: 開発者がエディタ既定の機能検証シーンから、エディタ外ランタイム機能だけを最小構成で試せる。
- evidence:
  - debug_test_play_area.md と Generator README で例示（combat_bounds_sandbox）
  - Project/Codes からファイル名参照無し（rg combat_bounds 0 件）
- suggestedUse: デバッグ: 境界マーカー検証をワンクリックで開く
- roadmapLink: feature-memos/debug_test_play_area.md
- firstSeenAt: 2026-11-10T04:41:00+09:00
- lastSeenAt: 2026-11-10T04:41:00+09:00

### GAP-Scene-CommandScene-004

- status: open
- tier: P2
- score: 600
- primaryUseCase: UC-Debug
- scenarioId: DBG-FeatureScene-001
- frequency: medium
- frequencyBasis: 起動シーン未設定時に Title 固定
- scope: UC-Debug|Scene|CommandScene|Incomplete|Project/Codes/Source/Scene/CommandScene.cpp:25
- primary: `CommandScene`
- summary: 【Debug】startup 空なら BuildUp ではなく Title
- scenarioText: 開発者がエディタ既定の機能検証シーンから、エディタ外ランタイム機能だけを最小構成で試せる。
- evidence:
  - Release 分岐で ResolveStartupScenePath 空時 sceneNow_=SceneTitle（25-27 行）
  - DebugPlayArea テンプレや FeatureTestScene へのフォールバック無し
  - launch_config 欠損時も機能検証 JSON へ直行できない
- suggestedUse: Debug: 未設定時も既定 Feature シーンを BuildUp
- roadmapLink: feature-memos/debug_test_play_area.md
- firstSeenAt: 2026-11-11T01:31:00+09:00
- lastSeenAt: 2026-11-11T01:31:00+09:00

### GAP-Editor-EntityEditor-011

- status: open
- tier: P1
- score: 590
- primaryUseCase: UC-Debug
- scenarioId: DBG-FeatureScene-001
- frequency: high
- frequencyBasis: プレビュー生成が SceneBuildUp 経路に接続されない
- scope: UC-Debug|Editor|EntityEditor|Incomplete|Project/Codes/Source/System/Editor/Entity/EntityEditor.cpp:208
- primary: `EntityEditor`
- summary: 【Debug】BuildPreviewScene がエディタ内ベクタのみ
- scenarioText: 開発者がエディタ既定の機能検証シーンから、エディタ外ランタイム機能だけを最小構成で試せる。
- evidence:
  - BuildPreviewScene は BuildEntitiesFromSceneDocument のみ（208-209 行）
  - SceneBuildUpRegistry::Launch や EditorScenePlayRequest へ渡さない
  - FeatureScene シナリオのエディタ外最小ランタイムが未接続
- suggestedUse: Debug: JSON→BuildUp ワンショット検証
- firstSeenAt: 2026-11-10T23:06:00+09:00
- lastSeenAt: 2026-11-10T23:06:00+09:00

### GAP-Editor-EntityEditorUi-020

- status: open
- tier: P2
- score: 590
- primaryUseCase: UC-Debug
- scenarioId: DBG-FeatureScene-001
- frequency: medium
- frequencyBasis: 起動シーンパス表示のみで試行不可
- scope: UC-Debug|Editor|EntityEditorUi|MissingUI|Project/Codes/Source/System/Editor/Entity/EntityEditorUi.cpp:422
- primary: `EntityEditorUi`
- summary: 【Debug】Release 起動パスを表示するだけ
- scenarioText: 開発者がエディタ既定の機能検証シーンから、エディタ外ランタイム機能だけを最小構成で試せる。
- evidence:
  - ResolveStartupScenePath を TextDisabled 表示のみ（422-425 行）
  - 起動シーンに設定（407 行）は launch_config 保存のみ
  - EditorScenePlayRequest-004 と同系で DEVELOPMENT から BuildUp 試行無し
- suggestedUse: Debug: 設定済み起動 JSON をエディタから BuildUp 試験
- roadmapLink: feature-memos/debug_test_play_area.md
- firstSeenAt: 2026-11-11T01:01:00+09:00
- lastSeenAt: 2026-11-11T01:01:00+09:00

### GAP-Scene-SceneBuildUp-008

- status: open
- tier: P2
- score: 590
- primaryUseCase: UC-Debug
- scenarioId: DBG-FeatureScene-001
- frequency: high
- frequencyBasis: BuildUp でも ImGui エディタフレーム必須
- scope: UC-Debug|Scene|SceneBuildUp|Incomplete|Project/Codes/Source/Scene/SceneBuildUp.cpp:171
- primary: `SceneBuildUp`
- summary: 【Debug】SceneBuildUp::Draw が EditorFrame ワークベンチを常時描画
- scenarioText: 開発者がエディタ既定の機能検証シーンから、エディタ外ランタイム機能だけを最小構成で試せる。
- evidence:
  - DEVELOPMENT で LayoutDedicatedSceneViewWindow を毎フレーム呼ぶ（174-179 行）
  - DrawScene は早期 return し RT パス依存（155-157, 194 行）
  - GameOrder 単体ループ相当の最小ランタイム構成にならない
- suggestedUse: デバッグ: UI 薄いランタイムのみで Cmp を試す
- roadmapLink: feature-memos/debug_test_play_area.md
- firstSeenAt: 2026-11-10T07:01:00+09:00
- lastSeenAt: 2026-11-10T07:01:00+09:00

### GAP-Scene-SceneBuildUp-014

- status: open
- tier: P2
- score: 590
- primaryUseCase: UC-Debug
- scenarioId: DBG-FeatureScene-001
- frequency: high
- frequencyBasis: Enter 直後に戦闘ステージカメラ固定
- scope: UC-Debug|Scene|SceneBuildUp|Incomplete|Project/Codes/Source/Scene/SceneBuildUp.cpp:110
- primary: `SceneBuildUp`
- summary: 【Debug】ロード直後 SetupStageViewCamera を呼ぶ
- scenarioText: 開発者がエディタ既定の機能検証シーンから、エディタ外ランタイム機能だけを最小構成で試せる。
- evidence:
  - BuildEntities 直後に SetupStageViewCamera（110 行）
  - PrepareCamerasForDraw 前に rig カメラへ戦闘ポーズ適用
  - DrawRuntimeViewContent でも再適用（147 行）で JSON Cmp 試験と二重
- suggestedUse: Debug: 機能検証 Enter でカメラを JSON 主導に
- roadmapLink: feature-memos/entity_camera_components.md
- firstSeenAt: 2026-11-11T01:31:00+09:00
- lastSeenAt: 2026-11-11T01:31:00+09:00

### GAP-Editor-SceneEntityEditor-011

- status: open
- tier: P2
- score: 580
- primaryUseCase: UC-Debug
- scenarioId: DBG-FeatureScene-001
- frequency: high
- frequencyBasis: プレビュー用カメラ初期化が rig 直操作
- scope: UC-Debug|Editor|SceneEntityEditor|Incomplete|Project/Codes/Source/Scene/SceneEntityEditor.cpp:94
- primary: `SceneEntityEditor`
- summary: 【Debug】SetupPreviewCamera が GetCamera のみ更新
- scenarioText: 開発者がエディタ既定の機能検証シーンから、エディタ外ランタイム機能だけを最小構成で試せる。
- evidence:
  - EnterScene で SetupPreviewCamera→GetCamera translate（94-98 行）
  - previewEntities_ の CmpCamera は行列に反映されない
  - 機能検証シーンでもエディタプレビューは rig カメラ前提
- suggestedUse: Debug: JSON カメラをプレビュー初期姿勢に反映
- roadmapLink: feature-memos/entity_camera_components.md
- firstSeenAt: 2026-11-11T00:01:00+09:00
- lastSeenAt: 2026-11-11T00:01:00+09:00

### GAP-Infra-launch_config-001

- status: open
- tier: P2
- score: 580
- primaryUseCase: UC-Debug
- scenarioId: DBG-FeatureScene-001
- frequency: high
- frequencyBasis: 共有 launch_config が汎用 Scene_0 固定
- scope: UC-Debug|Infra|launch_config|MissingData|EditorAssets/launch_config.json:2
- primary: `launch_config`
- summary: 【Debug】startup が Feature 検証シーンではない
- scenarioText: 開発者がエディタ既定の機能検証シーンから、エディタ外ランタイム機能だけを最小構成で試せる。
- evidence:
  - startupScenePath は ../EditorAssets/Scenes/Scene_0.json（2 行）
  - DebugPlayArea/template_test_play_area は未参照
  - Release BuildUp 起動がエディタ新規シーン相当になる
- suggestedUse: Debug: リポジトリ既定を最小検証 JSON に
- roadmapLink: feature-memos/debug_test_play_area.md
- firstSeenAt: 2026-11-11T01:31:00+09:00
- lastSeenAt: 2026-11-11T01:31:00+09:00

### GAP-Scene-SceneBuildUp-010

- status: open
- tier: P2
- score: 580
- primaryUseCase: UC-Debug
- scenarioId: DBG-FeatureScene-001
- frequency: high
- frequencyBasis: RT 描画パスが GameOrder 後に StageView を再適用
- scope: UC-Debug|Scene|SceneBuildUp|Incomplete|Project/Codes/Source/Scene/SceneBuildUp.cpp:147
- primary: `SceneBuildUp`
- summary: 【Debug】DrawRuntimeViewContent が Prepare 後に SetupStageViewCamera
- scenarioText: 開発者がエディタ既定の機能検証シーンから、エディタ外ランタイム機能だけを最小構成で試せる。
- evidence:
  - GameOrder が Draw 直前に ActiveCameraSystem::PrepareSceneAndBindEngine（GameOrder.h:74-75）
  - DrawRuntimeViewContent が再度 SetupStageViewCamera→SetCamera（147-153 行）
  - 機能検証でも CmpCamera 行列が戦闘ステージ固定ポーズに上書きされる
- suggestedUse: Debug: BuildUp RT が本編 GameOrder カメラ契約と一致
- roadmapLink: feature-memos/entity_camera_components.md
- firstSeenAt: 2026-11-10T23:31:00+09:00
- lastSeenAt: 2026-11-10T23:31:00+09:00

### GAP-Scene-SceneBuildUp-012

- status: open
- tier: P2
- score: 580
- primaryUseCase: UC-Debug
- scenarioId: DBG-FeatureScene-001
- frequency: medium
- frequencyBasis: BuildUp が stageView ロック固定
- scope: UC-Debug|Scene|SceneBuildUp|Contract|Project/Codes/Include/Scene/SceneBuildUp.h:30
- primary: `SceneBuildUp`
- summary: 【Debug】IsStageViewCameraLocked が常時 true
- scenarioText: 開発者がエディタ既定の機能検証シーンから、エディタ外ランタイム機能だけを最小構成で試せる。
- evidence:
  - override で true 固定（SceneBuildUp.h:30）
  - PrepareBeforeDraw は stageViewLocked 時 UpdateMatrix のみ（CmpCamera.cpp:29-31）
  - EntityEditor は false（SceneEntityEditor.h:32）と非対称で検証シーン差分が大きい
- suggestedUse: Debug: ロック ON/OFF を切り替えて Cmp を試す
- roadmapLink: feature-memos/debug_test_play_area.md
- firstSeenAt: 2026-11-11T00:31:00+09:00
- lastSeenAt: 2026-11-11T00:31:00+09:00

### GAP-Scene-SceneBuildUpRegistry-002

- status: open
- tier: P2
- score: 580
- primaryUseCase: UC-Debug
- scenarioId: DBG-FeatureScene-001
- frequency: medium
- frequencyBasis: Launch が EnterScene／遷移を担わない
- scope: UC-Debug|Scene|SceneBuildUpRegistry|Incomplete|Project/Codes/Source/Scene/SceneBuildUp.cpp:40
- primary: `SceneBuildUpRegistry`
- summary: 【Debug】Launch は生成してポインタ返却のみ
- scenarioText: 開発者がエディタ既定の機能検証シーンから、エディタ外ランタイム機能だけを最小構成で試せる。
- evidence:
  - Launch は unique_ptr 登録と raw 返却のみ（37-40 行）
  - EnterScene / CommandChangeScene::Set の呼び出し無し
  - README は手動 Launch 後に呼び手が ChangeScene 想定
- suggestedUse: デバッグ: 検証シーン起動 API を 1 本化
- roadmapLink: feature-memos/debug_test_play_area.md
- firstSeenAt: 2026-11-10T07:31:00+09:00
- lastSeenAt: 2026-11-10T07:31:00+09:00

### GAP-Editor-SceneEntityEditor-010

- status: open
- tier: P2
- score: 570
- primaryUseCase: UC-Debug
- scenarioId: DBG-FeatureScene-001
- frequency: medium
- frequencyBasis: Runtime タブ描画が戦闘カメラに一時差し替え
- scope: UC-Debug|Editor|SceneEntityEditor|Incomplete|Project/Codes/Source/Scene/SceneEntityEditor.cpp:248
- primary: `SceneEntityEditor`
- summary: 【Debug】Runtime タブで SetupStageViewCamera に差し替え
- scenarioText: 開発者がエディタ既定の機能検証シーンから、エディタ外ランタイム機能だけを最小構成で試せる。
- evidence:
  - DrawSceneViewContent の非 Edit パスで DebugCamera 退避後 SetupStageViewCamera（248-252 行）
  - 描画後 RestoreDebugCamera（270-272 行）でプレビュー用に戻す
  - JSON CmpCamera の挙動を Runtime タブ単体で検証できない
- suggestedUse: Debug: エディタ実行ビューでエンティティカメラをそのまま試す
- roadmapLink: feature-memos/entity_camera_components.md
- firstSeenAt: 2026-11-10T23:31:00+09:00
- lastSeenAt: 2026-11-10T23:31:00+09:00

### GAP-Infra-EditorScenePlayRequest-004

- status: open
- tier: P2
- score: 570
- primaryUseCase: UC-Debug
- scenarioId: DBG-FeatureScene-001
- frequency: medium
- frequencyBasis: 起動シーン設定が PlayRequest を書くだけ
- scope: UC-Debug|Infra|EditorScenePlayRequest|Unwired|Project/Codes/Source/System/Editor/Scene/EditorSceneAsset.cpp:161
- primary: `EditorScenePlayRequest`
- summary: 【Debug】Set 後に BuildUp 起動が無い
- scenarioText: 開発者がエディタ既定の機能検証シーンから、エディタ外ランタイム機能だけを最小構成で試せる。
- evidence:
  - TrySetRepositoryStartupScene が Set のみ（161 行）
  - DEVELOPMENT 起動は SceneEntityEditor 固定（CommandScene-003）
  - PlayRequest 読み取りは未配線（001〜003 と同系）
- suggestedUse: Debug: 起動シーン JSON からワンアクションで検証ランタイム
- roadmapLink: feature-memos/debug_test_play_area.md
- firstSeenAt: 2026-11-11T00:01:00+09:00
- lastSeenAt: 2026-11-11T00:01:00+09:00

### GAP-Scene-SceneBuildUp-017

- status: open
- tier: P2
- score: 570
- primaryUseCase: UC-Debug
- scenarioId: DBG-FeatureScene-001
- frequency: medium
- frequencyBasis: フェード中は Update 停止で F1 無効
- scope: UC-Debug|Scene|SceneBuildUp|Incomplete|Project/Codes/Source/Scene/SceneBuildUp.cpp:117
- primary: `SceneBuildUp`
- summary: 【Debug】遷移演出中 F1 復帰が処理されない
- scenarioText: 開発者がエディタ既定の機能検証シーンから、エディタ外ランタイム機能だけを最小構成で試せる。
- evidence:
  - F1 は Update 内のみ（117-122 行）
  - GameOrder は GetIsExecute 中 sceneNow->Update を呼ばない（GameOrder.h:61-67）
  - BuildUp 出入りの暗転中はエディタへ戻れない
- suggestedUse: Debug: 検証シーンの出入りをフェード中も中断可能に
- roadmapLink: feature-memos/debug_test_play_area.md
- firstSeenAt: 2026-11-11T02:01:00+09:00
- lastSeenAt: 2026-11-11T02:01:00+09:00

### GAP-Editor-SceneEntityEditor-008

- status: open
- tier: P2
- score: 560
- primaryUseCase: UC-Debug
- scenarioId: DBG-FeatureScene-001
- frequency: high
- frequencyBasis: エディタ Draw 内で GameOrder 外の camera->Update
- scope: UC-Debug|Editor|SceneEntityEditor|Incomplete|Project/Codes/Source/Scene/SceneEntityEditor.cpp:181
- primary: `SceneEntityEditor`
- summary: 【Debug】Edit ホバー時に Draw 内で camera->Update
- scenarioText: 開発者がエディタ既定の機能検証シーンから、エディタ外ランタイム機能だけを最小構成で試せる。
- evidence:
  - AllowDebugCameraControl は false だが Draw 内で camera->Update（181-183 行）
  - GameOrder は PrepareCamerasForDraw 経由（GameOrder.h:74）
  - 機能検証シーンでもカメラ更新経路が二重
- suggestedUse: Debug: エディタ検証シーンのカメラ政策を一本化
- firstSeenAt: 2026-11-10T23:06:00+09:00
- lastSeenAt: 2026-11-10T23:06:00+09:00

### GAP-Infra-launch_config-002

- status: open
- tier: P2
- score: 560
- primaryUseCase: UC-Debug
- scenarioId: DBG-FeatureScene-001
- frequency: medium
- frequencyBasis: 欠損パスは静かに空文字
- scope: UC-Debug|Infra|launch_config|Incomplete|Project/Codes/Source/System/Editor/Scene/EditorSceneAsset.cpp:167
- primary: `launch_config`
- summary: 【Debug】設定読込失敗を区別できない
- scenarioText: 開発者がエディタ既定の機能検証シーンから、エディタ外ランタイム機能だけを最小構成で試せる。
- evidence:
  - TryLoadLaunchConfig 失敗で {}（167-168 行）
  - startup ファイル欠損も同様に空（171-173 行）
  - CommandScene-004 と合わせ原因特定が難しい
- suggestedUse: Debug: 起動不能時に launch_config 理由を示す
- roadmapLink: feature-memos/debug_test_play_area.md
- firstSeenAt: 2026-11-11T03:31:00+09:00
- lastSeenAt: 2026-11-11T03:31:00+09:00

### GAP-Scene-SceneBuildUp-018

- status: open
- tier: P2
- score: 560
- primaryUseCase: UC-Debug
- scenarioId: DBG-FeatureScene-001
- frequency: medium
- frequencyBasis: JSON ロード後にタグカタログ同期が無い
- scope: UC-Debug|Scene|SceneBuildUp|Incomplete|Project/Codes/Source/Scene/SceneBuildUp.cpp:105
- primary: `SceneBuildUp`
- summary: 【Debug】BuildUp が tagCatalog マージを飛ばす
- scenarioText: 開発者がエディタ既定の機能検証シーンから、エディタ外ランタイム機能だけを最小構成で試せる。
- evidence:
  - TryLoad 後 BuildEntities 直前にタグ同期無し（105-109 行）
  - EntityEditor::LoadFromJsonPath は Merge/Sync 実施（171-173 行）
  - camera.main 等ロールタグの検証結果がエディタとずれうる
- suggestedUse: Debug: Feature JSON のタグ契約を本編と同一化
- roadmapLink: feature-memos/debug_test_play_area.md
- firstSeenAt: 2026-11-11T02:31:00+09:00
- lastSeenAt: 2026-11-11T02:31:00+09:00

### GAP-Editor-SceneEntityEditor-009

- status: open
- tier: P2
- score: 550
- primaryUseCase: UC-Debug
- scenarioId: DBG-FeatureScene-001
- frequency: medium
- frequencyBasis: RT 描画パスが毎回 engine->SetCamera する
- scope: UC-Debug|Editor|SceneEntityEditor|Incomplete|Project/Codes/Source/Scene/SceneEntityEditor.cpp:250
- primary: `SceneEntityEditor`
- summary: 【Debug】シーンビュー RT が GameOrder 後に SetCamera
- scenarioText: 開発者がエディタ既定の機能検証シーンから、エディタ外ランタイム機能だけを最小構成で試せる。
- evidence:
  - DrawSceneViewContent で Fit 後 engine->SetCamera（249-250 行）
  - GameOrder 直前に ActiveCameraSystem が既に Bind（GameOrder.h:74）
  - 本番相当 Runtime タブ試験でもエンジンカメラがフレーム内で上書き
- suggestedUse: Debug: 検証シーンのカメラ解決をエンジン主導に揃える
- firstSeenAt: 2026-11-10T23:06:00+09:00
- lastSeenAt: 2026-11-10T23:06:00+09:00

### GAP-Scene-CommandScene-005

- status: open
- tier: P2
- score: 550
- primaryUseCase: UC-Debug
- scenarioId: DBG-FeatureScene-001
- frequency: medium
- frequencyBasis: 起動 JSON の事前検証なし
- scope: UC-Debug|Scene|CommandScene|Incomplete|Project/Codes/Source/Scene/CommandScene.cpp:21
- primary: `CommandScene`
- summary: 【Debug】Launch 前に TryLoad を呼ばない
- scenarioText: 開発者がエディタ既定の機能検証シーンから、エディタ外ランタイム機能だけを最小構成で試せる。
- evidence:
  - exists(startupScene) 判定まで（21-24 行）
  - JSON 内容や tag 同期は EnterScene 到達後（BuildUp-013/018）
  - 壊れた Feature シーンで Title フォールバック無し
- suggestedUse: Debug: 起動前に Feature JSON を検証
- firstSeenAt: 2026-11-11T04:01:00+09:00
- lastSeenAt: 2026-11-11T04:01:00+09:00

### GAP-Editor-EditorSceneAsset-001

- status: open
- tier: P2
- score: 540
- primaryUseCase: UC-Debug
- scenarioId: DBG-FeatureScene-001
- frequency: medium
- frequencyBasis: 起動パスを正規化せずそのまま保存
- scope: UC-Debug|Editor|EditorSceneAsset|Contract|Project/Codes/Source/System/Editor/Scene/EditorSceneAsset.cpp:157
- primary: `EditorSceneAsset`
- summary: 【Debug】startupScenePath を生文字列で保存
- scenarioText: 開発者がエディタ既定の機能検証シーンから、エディタ外ランタイム機能だけを最小構成で試せる。
- evidence:
  - config.startupScenePath = sceneJsonPath 直代入（157 行）
  - lexically_normal / EditorAssets 基準への統一無し
  - cwd 依存で Release BuildUp とエディタ表示が乖離（BuildUp-005）
- suggestedUse: Debug: launch パスをリポジトリ基準で固定
- firstSeenAt: 2026-11-11T03:31:00+09:00
- lastSeenAt: 2026-11-11T03:31:00+09:00

### GAP-Scene-CommandScene-006

- status: open
- tier: P2
- score: 540
- primaryUseCase: UC-Debug
- scenarioId: DBG-FeatureScene-001
- frequency: medium
- frequencyBasis: Release 起動で Enter 無条件
- scope: UC-Debug|Scene|CommandScene|Incomplete|Project/Codes/Source/Scene/CommandScene.cpp:30
- primary: `CommandScene`
- summary: 【Debug】sceneNow 確定後 Enter を無条件
- scenarioText: 開発者がエディタ既定の機能検証シーンから、エディタ外ランタイム機能だけを最小構成で試せる。
- evidence:
  - BuildUp Launch 成否を見ず EnterScene（22-30 行）
  - 壊れた JSON でも Enter が走りフォールバックと混同
  - CommandScene-005 は path 解決のみ、Enter 検証なし
- suggestedUse: Debug: 起動シーン検証とタイトルフォールバック
- firstSeenAt: 2026-11-11T04:31:00+09:00
- lastSeenAt: 2026-11-11T04:31:00+09:00

### GAP-Scene-SceneBuildUp-019

- status: open
- tier: P2
- score: 540
- primaryUseCase: UC-Debug
- scenarioId: DBG-FeatureScene-001
- frequency: medium
- frequencyBasis: ステージロックとデバッグカメラ許可が矛盾
- scope: UC-Debug|Scene|SceneBuildUp|Contract|Project/Codes/Include/Scene/SceneBuildUp.h:30
- primary: `SceneBuildUp`
- summary: 【Debug】AllowDebugCameraControl をオーバーライドしない
- scenarioText: 開発者がエディタ既定の機能検証シーンから、エディタ外ランタイム機能だけを最小構成で試せる。
- evidence:
  - IsStageViewCameraLocked のみ true（30 行）
  - Scene 既定 AllowDebugCameraControl true（Scene.h:71）
  - PrepareBeforeDraw に allowDebug=true が渡り本番相当試験と方針が曖昧
- suggestedUse: Debug: 機能検証は本番カメラ契約に固定
- roadmapLink: feature-memos/entity_camera_components.md
- firstSeenAt: 2026-11-11T02:31:00+09:00
- lastSeenAt: 2026-11-11T02:31:00+09:00

### GAP-Infra-DebugPlayAreaGenerator-001

- status: open
- tier: P3
- score: 520
- primaryUseCase: UC-Debug
- scenarioId: DBG-FeatureScene-001
- frequency: medium
- frequencyBasis: 実行手順が README 上の C++ 直書きのみ
- scope: UC-Debug|Infra|DebugPlayAreaGenerator|MissingUI|EditorAssets/Tools/DebugPlayAreaGenerator/README.md:17
- primary: `DebugPlayAreaGenerator`
- summary: 【Debug】ランタイムプレビュー手順がコード断片のみ
- scenarioText: 開発者がエディタ既定の機能検証シーンから、エディタ外ランタイム機能だけを最小構成で試せる。
- evidence:
  - README 手順 2 は Launch 呼び出し例のみ（17 行）
  - メニュー／ボタン／既定 returnScene の記載無し
  - scaffold はファイル生成のみ（scaffold_play_area-001）
- suggestedUse: デバッグ: ドキュメントではなくエディタから完結
- roadmapLink: feature-memos/debug_test_play_area.md
- firstSeenAt: 2026-11-10T07:31:00+09:00
- lastSeenAt: 2026-11-10T07:31:00+09:00

### GAP-Scene-SceneBuildUp-007

- status: open
- tier: P3
- score: 520
- primaryUseCase: UC-Debug
- scenarioId: DBG-FeatureScene-001
- frequency: low
- frequencyBasis: BuildUp が EditorSceneRuntime を include するがシンボル未使用
- scope: UC-Debug|Scene|SceneBuildUp|Unwired|Project/Codes/Source/Scene/SceneBuildUp.cpp:10
- primary: `SceneBuildUp`
- summary: 【Debug】SceneBuildUp が EditorSceneRuntime を参照しない
- scenarioText: 開発者がエディタ既定の機能検証シーンから、エディタ外ランタイム機能だけを最小構成で試せる。
- evidence:
  - #include EditorSceneRuntime.h（10 行）
  - Entity 構築は EntityEditor::TryLoadSceneDocument のみ（91-105 行）
  - ランタイム専用ローダ／検証が EditorSceneRuntime.cpp と分断
- suggestedUse: デバッグ: 機能検証シーンがランタイム用ロード経路を共有
- roadmapLink: feature-memos/debug_test_play_area.md
- firstSeenAt: 2026-11-10T06:31:00+09:00
- lastSeenAt: 2026-11-10T06:31:00+09:00

### GAP-Scene-SceneBuildUp-015

- status: open
- tier: P3
- score: 510
- primaryUseCase: UC-Debug
- scenarioId: DBG-FeatureScene-001
- frequency: medium
- frequencyBasis: 退出時にエディタタブが Runtime のまま
- scope: UC-Debug|Scene|SceneBuildUp|Incomplete|Project/Codes/Source/Scene/SceneBuildUp.cpp:210
- primary: `SceneBuildUp`
- summary: 【Debug】ExitScene が SceneViewTab を戻さない
- scenarioText: 開発者がエディタ既定の機能検証シーンから、エディタ外ランタイム機能だけを最小構成で試せる。
- evidence:
  - EnterScene は Runtime タブ固定（112-113 行）
  - ExitScene は Teardown のみ（210-213 行）で EditorFrame 更新無し
  - F1 復帰後も EntityEditor の表示タブが Runtime 想定のまま
- suggestedUse: Debug: 検証終了後にエディットビューへ自動復帰
- roadmapLink: feature-memos/debug_test_play_area.md
- firstSeenAt: 2026-11-11T02:01:00+09:00
- lastSeenAt: 2026-11-11T02:01:00+09:00

### GAP-Editor-EditorSceneAsset-002

- status: open
- tier: P3
- score: 500
- primaryUseCase: UC-Debug
- scenarioId: DBG-FeatureScene-001
- frequency: low
- frequencyBasis: PlayRequest が Clear されない
- scope: UC-Debug|Editor|EditorSceneAsset|Incomplete|Project/Codes/Source/System/Editor/Scene/EditorSceneAsset.cpp:182
- primary: `EditorSceneAsset`
- summary: 【Debug】PlayRequest がパスだけ保持
- scenarioText: 開発者がエディタ既定の機能検証シーンから、エディタ外ランタイム機能だけを最小構成で試せる。
- evidence:
  - Set は jsonFilePath_ 代入のみ（182-184 行）
  - BuildUp Exit で Clear 呼び出しなし
  - エディタ再実行と launch 設定の食い違いが残る
- suggestedUse: Debug: 検証シーン実行後のリクエスト整合
- firstSeenAt: 2026-11-11T04:31:00+09:00
- lastSeenAt: 2026-11-11T04:31:00+09:00

### GAP-Infra-EditorScenePlayRequest-003

- status: open
- tier: P3
- score: 500
- primaryUseCase: UC-Debug
- scenarioId: DBG-FeatureScene-001
- frequency: low
- frequencyBasis: PlayRequest クリアが未配線でパスが残りうる
- scope: UC-Debug|Infra|EditorScenePlayRequest|Unwired|Project/Codes/Source/System/Editor/Scene/EditorSceneAsset.cpp:105
- primary: `EditorScenePlayRequest`
- summary: 【Debug】EditorScenePlayRequest::Clear が呼ばれない
- scenarioText: 開発者がエディタ既定の機能検証シーンから、エディタ外ランタイム機能だけを最小構成で試せる。
- evidence:
  - Clear は jsonFilePath_.clear() のみ（EditorSceneAsset.cpp:105）
  - Set 呼び出しも未配線のため二重未使用（001/002）
  - 将来配線時に終了シーンでクリアされず古い JSON が残る
- suggestedUse: デバッグ: 検証シーン入退場で再生パスを確実にリセット
- roadmapLink: feature-memos/debug_test_play_area.md
- firstSeenAt: 2026-11-10T07:01:00+09:00
- lastSeenAt: 2026-11-10T07:01:00+09:00

### GAP-Scene-SceneBuildUp-016

- status: open
- tier: P3
- score: 500
- primaryUseCase: UC-Debug
- scenarioId: DBG-FeatureScene-001
- frequency: high
- frequencyBasis: 実行中 JSON パスが UI に出ない
- scope: UC-Debug|Scene|SceneBuildUp|MissingUI|Project/Codes/Source/Scene/SceneBuildUp.cpp:180
- primary: `SceneBuildUp`
- summary: 【Debug】専用ランタイムウィンドウが汎用文言のみ
- scenarioText: 開発者がエディタ既定の機能検証シーンから、エディタ外ランタイム機能だけを最小構成で試せる。
- evidence:
  - statusLine は固定説明文（182 行）で sceneJsonPath_ 非表示
  - sceneDisplayName_ はコンストラクタ由来のみ（26-27 行）
  - cwd ずれで別 JSON を読んでも UI から判別しづらい（BuildUp-005 と同系）
- suggestedUse: Debug: どの JSON を試しているかヘッダで確認
- roadmapLink: feature-memos/debug_test_play_area.md
- firstSeenAt: 2026-11-11T02:01:00+09:00
- lastSeenAt: 2026-11-11T02:01:00+09:00

### GAP-Scene-SceneBuildUp-020

- status: open
- tier: P3
- score: 500
- primaryUseCase: UC-Debug
- scenarioId: DBG-FeatureScene-001
- frequency: medium
- frequencyBasis: ランタイムビュー毎フレームでライト再設定
- scope: UC-Debug|Scene|SceneBuildUp|Incomplete|Project/Codes/Source/Scene/SceneBuildUp.cpp:144
- primary: `SceneBuildUp`
- summary: 【Debug】ビュー描画ごとに SetupRuntimeLighting
- scenarioText: 開発者がエディタ既定の機能検証シーンから、エディタ外ランタイム機能だけを最小構成で試せる。
- evidence:
  - DrawRuntimeViewContent 先頭で SetupRuntimeLighting（144 行）
  - EnterScene でも SetupStageViewCamera（110 行）と非対称
  - ImGui 機能検証でライト状態がフレーム毎に上書き
- suggestedUse: Debug: ランタイムプレビューの描画コストと一貫性
- firstSeenAt: 2026-11-11T03:01:00+09:00
- lastSeenAt: 2026-11-11T03:01:00+09:00

### GAP-Scene-SceneBuildUp-021

- status: open
- tier: P3
- score: 490
- primaryUseCase: UC-Debug
- scenarioId: DBG-FeatureScene-001
- frequency: medium
- frequencyBasis: 描画はルート Entity のみ走査
- scope: UC-Debug|Scene|SceneBuildUp|Incomplete|Project/Codes/Source/Scene/SceneBuildUp.cpp:85
- primary: `SceneBuildUp`
- summary: 【Debug】子付きインスタンスは親 Draw 任せ
- scenarioText: 開発者がエディタ既定の機能検証シーンから、エディタ外ランタイム機能だけを最小構成で試せる。
- evidence:
  - GetParent()==nullptr のみ Draw（85-86 行）
  - 親子不整合や親非 Draw 時は子が表示されない
  - エディタ階層 JSON の機能検証で欠落に気付きにくい
- suggestedUse: Debug: ヒエラルキー試験の描画契約を明示
- firstSeenAt: 2026-11-11T03:31:00+09:00
- lastSeenAt: 2026-11-11T03:31:00+09:00

### GAP-Infra-launch_config-003

- status: open
- tier: P3
- score: 480
- primaryUseCase: UC-Debug
- scenarioId: DBG-FeatureScene-001
- frequency: medium
- frequencyBasis: launch_config が 1 キーのみ上書き保存
- scope: UC-Debug|Infra|launch_config|Incomplete|Project/Codes/Source/System/Editor/Scene/EditorSceneAsset.cpp:136
- primary: `launch_config`
- summary: 【Debug】JSON は startupScenePath のみ
- scenarioText: 開発者がエディタ既定の機能検証シーンから、エディタ外ランタイム機能だけを最小構成で試せる。
- evidence:
  - json 代入は startupScenePath のみ（136-137 行）
  - schemaVersion や DebugPlayArea 既定無し
  - 手編集メタとエディタ保存の競合リスク
- suggestedUse: Debug: 起動設定 JSON の拡張余地を明示
- roadmapLink: feature-memos/debug_test_play_area.md
- firstSeenAt: 2026-11-11T04:01:00+09:00
- lastSeenAt: 2026-11-11T04:01:00+09:00

### GAP-Infra-scaffold_play_area-002

- status: open
- tier: P3
- score: 480
- primaryUseCase: UC-Debug
- scenarioId: DBG-FeatureScene-001
- frequency: medium
- frequencyBasis: scaffold 完了メッセージが UI 導線を含まない
- scope: UC-Debug|Infra|scaffold_play_area|Incomplete|EditorAssets/Tools/DebugPlayAreaGenerator/scaffold_play_area.py:44
- primary: `scaffold_play_area`
- summary: 【Debug】生成後は Launch のみ案内
- scenarioText: 開発者がエディタ既定の機能検証シーンから、エディタ外ランタイム機能だけを最小構成で試せる。
- evidence:
  - print は Launch または EntityEditor OpenScene（44 行）
  - Scenes/ と DebugPlayArea/ の一覧差・起動シーン設定手順無し
  - scaffold_play_area-001 / DebugPlayAreaGenerator-001 と手順分散
- suggestedUse: Debug: scaffold から試験起動まで一続きの手順
- roadmapLink: feature-memos/debug_test_play_area.md
- firstSeenAt: 2026-11-11T02:31:00+09:00
- lastSeenAt: 2026-11-11T02:31:00+09:00

### GAP-Scene-SceneBuildUpRegistry-003

- status: open
- tier: P3
- score: 480
- primaryUseCase: UC-Debug
- scenarioId: DBG-FeatureScene-001
- frequency: low
- frequencyBasis: Launch が毎回 new して vector に蓄積
- scope: UC-Debug|Scene|SceneBuildUpRegistry|Incomplete|Project/Codes/Source/Scene/SceneBuildUp.cpp:43
- primary: `SceneBuildUpRegistry`
- summary: 【Debug】Launch が同一パス重複を防がない
- scenarioText: 開発者がエディタ既定の機能検証シーンから、エディタ外ランタイム機能だけを最小構成で試せる。
- evidence:
  - make_unique+push_back のみ（39-44 行）
  - Exit まで OnSceneExited 以外で解放されない（47-56 行）
  - シーン実行の連打で所有リストが膨らみうる
- suggestedUse: Debug: 機能検証の再 Launch ポリシーを明示
- firstSeenAt: 2026-11-11T03:01:00+09:00
- lastSeenAt: 2026-11-11T03:01:00+09:00

### GAP-Scene-SceneBuildUp-022

- status: open
- tier: P3
- score: 470
- primaryUseCase: UC-Debug
- scenarioId: DBG-FeatureScene-001
- frequency: low
- frequencyBasis: 表示名がロード前のパス stem のまま
- scope: UC-Debug|Scene|SceneBuildUp|Incomplete|Project/Codes/Source/Scene/SceneBuildUp.cpp:26
- primary: `SceneBuildUp`
- summary: 【Debug】ctor で displayName を仮設定
- scenarioText: 開発者がエディタ既定の機能検証シーンから、エディタ外ランタイム機能だけを最小構成で試せる。
- evidence:
  - コンストラクタが path stem を設定（24-27 行）
  - EnterScene 成功まで sceneName 未反映（105-107 行）
  - UI ヘッダと実ロード名のずれ（BuildUp-016 と同系）
- suggestedUse: Debug: 試験シーン名をウィンドウへ即反映
- firstSeenAt: 2026-11-11T04:01:00+09:00
- lastSeenAt: 2026-11-11T04:01:00+09:00

### GAP-Scene-SceneBuildUp-023

- status: open
- tier: P3
- score: 460
- primaryUseCase: UC-Debug
- scenarioId: DBG-FeatureScene-001
- frequency: low
- frequencyBasis: Feature シーンでも遷移 UI 常時
- scope: UC-Debug|Scene|SceneBuildUp|Incomplete|Project/Codes/Source/Scene/SceneBuildUp.cpp:191
- primary: `SceneBuildUp`
- summary: 【Debug】Draw で ChangeScene 常時描画
- scenarioText: 開発者がエディタ既定の機能検証シーンから、エディタ外ランタイム機能だけを最小構成で試せる。
- evidence:
  - ImGui 有効でも engine->PreDraw 後に CommandChangeScene::Draw（191-192 行）
  - Release 相当の DrawScene は #else 分岐のみ（188-190 行）
  - 最小検証ビューにフェード UI が重なる
- suggestedUse: Debug: 機能検証用の描画経路分離
- firstSeenAt: 2026-11-11T04:31:00+09:00
- lastSeenAt: 2026-11-11T04:31:00+09:00

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

### GAP-Editor-EditorSceneRuntime-006

- status: open
- tier: P1
- score: 780
- primaryUseCase: UC-Play
- secondaryUseCases: UC-Authoring
- scenarioId: PLY-SceneLoad-001
- frequency: medium
- frequencyBasis: 子オブジェクト付き JSON が Init されない
- scope: UC-Play|Editor|EditorSceneRuntime|Incomplete|Project/Codes/Source/System/Editor/Scene/EditorSceneRuntime.cpp:364
- primary: `EditorSceneRuntime`
- summary: 【Play】親付きインスタンスが Initialize を呼ばれない
- scenarioText: プレイヤーがシーンを開き、主要オブジェクトとカメラが安定して動作する。
- evidence:
  - Initialize ループは parentIndex<0 のみ（364-370 行）
  - 子 Entity の Cmp Upd が未初期化のまま Update されうる
  - SceneBuildUp EnterScene が同 API 経由で entities_ 構築（105 行）
- suggestedUse: プレイ: 階層シーンを開いても子オブジェクトが安定動作
- firstSeenAt: 2026-11-10T05:51:00+09:00
- lastSeenAt: 2026-11-10T05:51:00+09:00

### GAP-Scene-SceneMovie-004

- status: open
- tier: P1
- score: 780
- primaryUseCase: UC-Play
- scenarioId: PLY-Movie-001
- frequency: medium
- frequencyBasis: returnScene 未設定時に終了遷移が成立せず Update が空回り
- scope: UC-Play|Scene|SceneMovie|Contract|Project/Codes/Source/Scene/SceneMovie.cpp:67
- primary: `SceneMovie`
- summary: 【Play】returnScene が null だとムービー終了／失敗後もシーンが抜けられない
- scenarioText: シーン内ムービー／カットシーンが再生完了まで破綻しない（spec: scene_movie.md）。
- evidence:
  - RequestReturnToCaller は returnScene_==nullptr で早期 return（67-70 行）
  - トラック終了／失敗時も Update が毎フレーム RequestReturnToCaller を呼ぶが遷移しない（106-118 行）
- suggestedUse: プレイ: Launch 誤用時でもハングせず復帰または明示エラー
- roadmapLink: feature-memos/scene_movie.md
- firstSeenAt: 2026-11-10T03:41:00+09:00
- lastSeenAt: 2026-11-10T03:41:00+09:00

### GAP-Camera-StageBounds-001

- status: open
- tier: P1
- score: 750
- primaryUseCase: UC-Play
- secondaryUseCases: UC-Debug
- scenarioId: PLY-Camera-001
- frequency: high
- frequencyBasis: ステージカメラのフレーミングが全シーンで同一の固定 AABB
- scope: UC-Play|Camera|StageBounds|Contract|Project/Codes/Include/System/StageBounds.h:9
- primary: `StageBounds`
- summary: 【Play】StageView カメラが StageBounds 定数のみ参照しシーン毎の範囲に追従しない
- scenarioText: プレイ中カメラが地形・オブジェクトを見失わず、操作に追従する（spec: feature-memos/player_friendly_camera_policy.md）。
- evidence:
  - ComputeStageBattleViewPose は kLeftEndX/kRightEndX 等のグローバル定数（StageViewCamera.cpp:57-69）
  - DebugPlayArea の境界マーカー JSON はカメラ計算に未接続
- suggestedUse: プレイ: ステージサイズに応じた視界を維持
- roadmapLink: feature-memos/player_friendly_camera_policy.md
- firstSeenAt: 2026-11-10T03:21:00+09:00
- lastSeenAt: 2026-11-10T03:21:00+09:00

### GAP-Render-SceneGame-002

- status: open
- tier: P1
- score: 750
- primaryUseCase: UC-Play
- secondaryUseCases: UC-Authoring
- scenarioId: PLY-Visual-001
- frequency: high
- frequencyBasis: 毎フレーム SetupGameLighting でエンジン側設定がリセット
- scope: UC-Play|Render|SceneGame|Incomplete|Project/Codes/Source/Scene/SceneGame.cpp:200
- primary: `SceneGame`
- summary: 【Play】DrawScene 毎に見た目リテラルを再適用しデータ反映が上書きされる
- scenarioText: ランタイムでライト・フォグ等の見た目がデータどおり反映される（spec: rim_light / volumetric_fog）。
- evidence:
  - DrawScene 先頭で SetupGameLighting を毎回呼ぶ（200 行）
  - EnterScene 以外の経路で RefVolumetricFog 等を変えても次フレームで C++ 既定に戻る
- suggestedUse: プレイ・制作: ロードした見た目データがランタイム中も保持される
- roadmapLink: feature-memos/volumetric_fog.md
- firstSeenAt: 2026-11-10T04:21:00+09:00
- lastSeenAt: 2026-11-10T04:21:00+09:00

### GAP-Scene-GameOrder-001

- status: open
- tier: P1
- score: 750
- primaryUseCase: UC-Play
- scenarioId: PLY-SceneLoad-001
- frequency: high
- frequencyBasis: フェード中は現シーンも遷移先も Update が止まる
- scope: UC-Play|Scene|GameOrder|Incomplete|Project/Codes/Include/Scene/GameOrder.h:69
- primary: `GameOrder`
- summary: 【Play】遷移演出中はシーン Update が全面停止
- scenarioText: プレイヤーがシーンを開き、主要オブジェクトとカメラが安定して動作する。
- evidence:
  - GetIsExecute 中は sceneNow->Update を呼ばない（69-75 行）
  - ChangeSceneFade 中に ChangeScene 後も isExecute 継続（CommandChangeScene.h:108-129）
  - 遷移先 EnterScene 直後も Update 停止が続く
- suggestedUse: プレイ: ロード直後のカメラ／オブジェクトがフレーム進行する
- firstSeenAt: 2026-11-10T05:51:00+09:00
- lastSeenAt: 2026-11-10T05:51:00+09:00

### GAP-Render-Lighting-002

- status: open
- tier: P1
- score: 730
- primaryUseCase: UC-Play
- secondaryUseCases: UC-Debug
- scenarioId: PLY-Visual-001
- frequency: high
- frequencyBasis: フォグ／ゴッドレイ設定が Engine と Lighting で二重管理
- scope: UC-Play|Render|Lighting|Contract|Project/Codes/Include/Engine/Renderer/Lighting/Lighting.h:42
- primary: `Lighting`
- summary: 【Play】Visual::Lighting と AtrumEngine の VolumetricFog が非共有
- scenarioText: ランタイムでライト・フォグ等の見た目がデータどおり反映される（spec: rim_light / volumetric_fog）。
- evidence:
  - Lighting は独自 volumetricFog_（Lighting.h:19-42）を SceneLightData CB に適用
  - SceneGame は Engine::RefVolumetricFog().SetSettings のみ（SceneGame.cpp:57）
  - volumetric_fog.md の「同一 VolumetricFogSettings を共有」と不一致
- suggestedUse: プレイ: 一元的なデータ変更が Object3d CB に確実に届く
- roadmapLink: feature-memos/volumetric_fog.md
- firstSeenAt: 2026-11-10T04:21:00+09:00
- lastSeenAt: 2026-11-10T04:21:00+09:00

### GAP-Render-SceneGame-003

- status: open
- tier: P1
- score: 720
- primaryUseCase: UC-Play
- scenarioId: PLY-Visual-001
- frequency: high
- frequencyBasis: フォグ等の SetSettings が CB 反映されないフレームが生じうる
- scope: UC-Play|Render|SceneGame|Incomplete|Project/Codes/Source/Scene/SceneGame.cpp:57
- primary: `SceneGame`
- summary: 【Play】SetupGameLighting が Fog 設定後に Flush しない
- scenarioText: ランタイムでライト・フォグ等の見た目がデータどおり反映される（spec: rim_light / volumetric_fog）。
- evidence:
  - SetDirectionalLightData のみ Flush（48 行）
  - RefVolumetricFog/GodRays/Shadow の SetSettings は settings_ 更新のみ（VolumetricFog.h:35）
  - 同一フレーム CB は Fog 適用前の状態を反映しうる（AtrumEngine.cpp:1254-1263）
- suggestedUse: プレイ: フォグ密度変更が即 Object3d に届く
- roadmapLink: feature-memos/volumetric_fog.md
- firstSeenAt: 2026-11-10T06:11:00+09:00
- lastSeenAt: 2026-11-10T06:11:00+09:00

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

### GAP-Scene-CommandChangeScene-001

- status: open
- tier: P2
- score: 700
- primaryUseCase: UC-Play
- scenarioId: PLY-SceneLoad-001
- frequency: medium
- frequencyBasis: フェード遷移中の二重 Set が黙って捨てられる
- scope: UC-Play|Scene|CommandChangeScene|Contract|Project/Codes/Include/Scene/CommandChangeScene.h:178
- primary: `CommandChangeScene`
- summary: 【Play】遷移実行中の Set は無視され次シーン要求が失われる
- scenarioText: プレイヤーがシーンを開き、主要オブジェクトとカメラが安定して動作する。
- evidence:
  - Set は isExecute_ が false のときのみ受理（178-196 行）
  - フェード中の連続遷移や入力連打で 2 件目以降が警告なく破棄
- suggestedUse: プレイ: ロード／戻り遷移が意図どおり最後の要求を反映
- firstSeenAt: 2026-11-10T04:01:00+09:00
- lastSeenAt: 2026-11-10T04:01:00+09:00

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

### GAP-Scene-SceneGame-005

- status: open
- tier: P2
- score: 670
- primaryUseCase: UC-Play
- scenarioId: PLY-Audio-001
- frequency: medium
- frequencyBasis: ボス撃破時に BGM が即停止し演出中は無音
- scope: UC-Play|Scene|SceneGame|Incomplete|Project/Codes/Source/Scene/SceneGame.cpp:498
- primary: `SceneGame`
- summary: 【Play】OnBossDefeated で BGM を演出完了前に停止
- scenarioText: プレイ中に BGM／SE が途切れず再生され、シーン遷移後も期待どおり鳴り続ける。
- evidence:
  - OnBossDefeated 先頭で StopBgmLoop（498-500 行）
  - ボス死亡演出・リザルト遷移までは BGM 再生ポリシー未記載
- suggestedUse: プレイ: 撃破演出からリザルト BGM への自然な切替
- firstSeenAt: 2026-11-10T04:51:00+09:00
- lastSeenAt: 2026-11-10T04:51:00+09:00

### GAP-Audio-PauseBgmLoop-001

- status: open
- tier: P2
- score: 660
- primaryUseCase: UC-Play
- scenarioId: PLY-Audio-001
- frequency: medium
- frequencyBasis: 遷移フェードと BGM 制御 API が未接続
- scope: UC-Play|Audio|PauseBgmLoop|Unwired|Project/Codes/Include/System/PlaySe.h:112
- primary: `PauseBgmLoop`
- summary: 【Play】Pause/Resume/Volume 付き BGM API がシーンから未使用
- scenarioText: プレイ中に BGM／SE が途切れず再生され、シーン遷移後も期待どおり鳴り続ける。
- evidence:
  - PauseBgmLoop/ResumeBgmLoop/SetBgmLoopVolume は PlaySe.h に定義のみ
  - Codes 全体からの呼び出し無し（rg PauseBgmLoop 0 件）
  - ChangeSceneFade は音声と独立した暗転のみ
- suggestedUse: プレイ: フェード中の BGM ダックやクロスフェードを実装可能にする
- firstSeenAt: 2026-11-10T02:51:00+09:00
- lastSeenAt: 2026-11-10T02:51:00+09:00

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

### GAP-Scene-SceneBuildUp-005

- status: open
- tier: P2
- score: 660
- primaryUseCase: UC-Play
- secondaryUseCases: UC-Ship
- scenarioId: PLY-SceneLoad-001
- frequency: medium
- frequencyBasis: JSON シーンパスが cwd 依存の生文字列
- scope: UC-Play|Scene|SceneBuildUp|MissingData|Project/Codes/Source/Scene/SceneBuildUp.cpp:24
- primary: `SceneBuildUp`
- summary: 【Play】BuildUp の sceneJsonPath_ を基準パス解決しない
- scenarioText: プレイヤーがシーンを開き、主要オブジェクトとカメラが安定して動作する。
- evidence:
  - コンストラクタで渡されたパスをそのまま保持（24-27 行）
  - EnterScene が TryLoadSceneDocument(sceneJsonPath_) に直渡し（92 行）で Ship 起動 cwd と不一致時に意図しない代替 1 体
- suggestedUse: 配布: どの作業ディレクトリからも同じシーン JSON を開ける
- firstSeenAt: 2026-11-10T04:01:00+09:00
- lastSeenAt: 2026-11-10T04:01:00+09:00

### GAP-Audio-StopBgmLoop-001

- status: open
- tier: P2
- score: 650
- primaryUseCase: UC-Play
- scenarioId: PLY-Audio-001
- frequency: medium
- frequencyBasis: シーン BGM 停止が全ストリーミングボイスを巻き添え停止
- scope: UC-Play|Audio|StopBgmLoop|Incomplete|Project/Codes/Include/System/PlaySe.h:106
- primary: `StopBgmLoop`
- summary: 【Play】StopBgmLoop がシーン BGM 以外のストリーミングも停止
- scenarioText: プレイ中に BGM／SE が途切れず再生され、シーン遷移後も期待どおり鳴り続ける。
- evidence:
  - StopBgmLoop は StopSe 後に StopAllStreamingVoices を呼ぶ（106-109 行）
  - シーン遷移の ExitScene 毎に呼ばれ共有 SE ストリームも切断しうる
- suggestedUse: プレイ: 遷移時に意図しない SE 切れを防ぐ
- firstSeenAt: 2026-11-10T04:51:00+09:00
- lastSeenAt: 2026-11-10T04:51:00+09:00

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

### GAP-Scene-SceneMovie-006

- status: open
- tier: P2
- score: 650
- primaryUseCase: UC-Play
- secondaryUseCases: UC-Ship
- scenarioId: PLY-Movie-001
- frequency: medium
- frequencyBasis: トラック JSON パスが cwd 依存の生文字列のまま
- scope: UC-Play|Scene|SceneMovie|MissingData|Project/Codes/Source/Scene/SceneMovie.cpp:88
- primary: `SceneMovie`
- summary: 【Play】movie トラックパスを EditorAssets 基準に解決していない
- scenarioText: シーン内ムービー／カットシーンが再生完了まで破綻しない（spec: scene_movie.md）。
- evidence:
  - trackJsonPath_ をそのまま TryBeginFromJsonFile へ渡す（88 行）
  - 起動例は ../EditorAssets/Camera/... の相対パス（movie_camera_controller.md）で Ship と cwd 不一致リスク
- suggestedUse: 配布: 作業ディレクトリに依存せず同梱トラックを再生
- roadmapLink: feature-memos/movie_camera_controller.md
- firstSeenAt: 2026-11-10T03:41:00+09:00
- lastSeenAt: 2026-11-10T03:41:00+09:00

### GAP-Audio-AudioManager-003

- status: open
- tier: P2
- score: 640
- primaryUseCase: UC-Play
- scenarioId: PLY-Audio-001
- frequency: medium
- frequencyBasis: ストリーム SE プール枯渇で強制停止
- scope: UC-Play|Audio|AudioManager|Incomplete|Project/Codes/Source/Audio/Audio.cpp:449
- primary: `AudioManager`
- summary: 【Play】空き無しで全汎用ストリームをリセット
- scenarioText: プレイ中に BGM／SE が途切れず再生され、シーン遷移後も期待どおり鳴り続ける。
- evidence:
  - Stopped が無いと index>=1 を ResetStreamingVoice（449-453 行）
  - BGM 用 voice 0 は除外だが同時ストリーム SE は奪い合い
  - PlayStreaming は Handle を返すが呼び元が Stop 不能だと鳴り直し不能
- suggestedUse: プレイ: 長尺ストリーム SE の同時発音上限と優先度
- firstSeenAt: 2026-11-11T02:41:00+09:00
- lastSeenAt: 2026-11-11T02:41:00+09:00

### GAP-Audio-PlaySceneBgm-001

- status: open
- tier: P2
- score: 640
- primaryUseCase: UC-Play
- scenarioId: PLY-Audio-001
- frequency: medium
- frequencyBasis: BGM 再開のたびに全ストリーム／ループ SE を停止
- scope: UC-Play|Audio|PlaySceneBgm|Incomplete|Project/Codes/Source/Audio/Audio.cpp:687
- primary: `PlaySceneBgm`
- summary: 【Play】PlaySceneBgm 開始時に全ボイスを一括停止
- scenarioText: プレイ中に BGM／SE が途切れず再生され、シーン遷移後も期待どおり鳴り続ける。
- evidence:
  - PlaySceneBgm 先頭で StopAllStreamingVoices / StopAllLoopingShortVoices（687-688 行）
  - SceneGame::MaintainSceneBgmIfNeeded が切断検知で再 PlayBgmLoop（238 行）
  - 再開のたびに弾幕・ヒット等の SE ストリームが巻き添え停止しうる
- suggestedUse: プレイ: BGM 復帰と SE 再生の共存
- firstSeenAt: 2026-11-10T06:41:00+09:00
- lastSeenAt: 2026-11-10T06:41:00+09:00

### GAP-Render-SceneGame-004

- status: open
- tier: P2
- score: 640
- primaryUseCase: UC-Play
- scenarioId: PLY-Visual-001
- frequency: medium
- frequencyBasis: 遷移直後〜初回 Draw まで前シーンのライティング残存
- scope: UC-Play|Render|SceneGame|Incomplete|Project/Codes/Source/Scene/SceneGame.cpp:87
- primary: `SceneGame`
- summary: 【Play】EnterScene が本編見た目プリセットを適用しない
- scenarioText: ランタイムでライト・フォグ等の見た目がデータどおり反映される（spec: rim_light / volumetric_fog）。
- evidence:
  - SetupGameLighting は DrawScene のみ（200 行）
  - EnterScene は Spawn/カメラ/BGM のみ（87-117 行）
  - タイトル等からのフェードイン中 Update は動くが CB は前シーン設定のまま
- suggestedUse: プレイ: シーン開始直後からリム／フォグが安定
- roadmapLink: feature-memos/rim_light.md
- firstSeenAt: 2026-11-10T06:11:00+09:00
- lastSeenAt: 2026-11-10T06:11:00+09:00

### GAP-Scene-SceneBuildUp-004

- status: open
- tier: P2
- score: 640
- primaryUseCase: UC-Play
- scenarioId: PLY-Camera-001
- frequency: medium
- frequencyBasis: BuildUp プレビューも本編と同じ固定境界でフレーム
- scope: UC-Play|Scene|SceneBuildUp|Incomplete|Project/Codes/Source/Scene/SceneBuildUp.cpp:106
- primary: `SceneBuildUp`
- summary: 【Play】SceneBuildUp のカメラがシーン JSON の境界情報を読まない
- scenarioText: プレイ中カメラが地形・オブジェクトを見失わず、操作に追従する（spec: feature-memos/player_friendly_camera_policy.md）。
- evidence:
  - EnterScene/DrawRuntimeViewContent で SetupStageViewCamera のみ（106,143 行）
  - combat_bounds_sandbox 等のマーカーは可視化のみでカメラ未反映
- suggestedUse: プレイ: JSON 検証シーンでも本編相当のカメラフレーム
- firstSeenAt: 2026-11-10T03:21:00+09:00
- lastSeenAt: 2026-11-10T03:21:00+09:00

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

### GAP-Scene-SceneTitle-001

- status: open
- tier: P2
- score: 640
- primaryUseCase: UC-Play
- scenarioId: PLY-Audio-001
- frequency: medium
- frequencyBasis: BGM 再開・維持は SceneGame のみで Title/Result に無い
- scope: UC-Play|Scene|SceneTitle|Incomplete|Project/Codes/Source/Scene/SceneTitle.cpp:32
- primary: `SceneTitle`
- summary: 【Play】Title の BGM 途切れを検知・再開する処理が無い
- scenarioText: プレイ中に BGM／SE が途切れず再生され、シーン遷移後も期待どおり鳴り続ける。
- evidence:
  - SceneGame::MaintainSceneBgmIfNeeded のみ Update で BGM 再開（SceneGame.cpp:227-239）
  - SceneTitle::EnterScene で一度 PlayBgmLoop のみ（SceneTitle.cpp:37-38）、Update に維持ロジック無し
- suggestedUse: プレイ: ストリーム異常でタイトル BGM が止まっても復帰
- firstSeenAt: 2026-11-10T02:51:00+09:00
- lastSeenAt: 2026-11-10T02:51:00+09:00

### GAP-Audio-PlaySe-001

- status: open
- tier: P2
- score: 630
- primaryUseCase: UC-Play
- scenarioId: PLY-Audio-001
- frequency: high
- frequencyBasis: ワンショット SE がハンドル無しで追跡不可
- scope: UC-Play|Audio|PlaySe|Incomplete|Project/Codes/Include/System/PlaySe.h:54
- primary: `PlaySe`
- summary: 【Play】PlaySe が Play の Handle を捨てる
- scenarioText: プレイ中に BGM／SE が途切れず再生され、シーン遷移後も期待どおり鳴り続ける。
- evidence:
  - PlaySe は Manager::Play(index) の戻り値未使用（48-54 行）
  - シーン Exit は StopBgmLoop のみでワンショット停止なし
  - Combat の PlaySe が遷移後も鳴り続けうる（StopSe-001 と同系）
- suggestedUse: プレイ: シーン切替で SE 残骸を制御
- firstSeenAt: 2026-11-11T02:11:00+09:00
- lastSeenAt: 2026-11-11T02:11:00+09:00

### GAP-Camera-StageViewCamera-001

- status: open
- tier: P2
- score: 630
- primaryUseCase: UC-Play
- scenarioId: PLY-Camera-001
- frequency: medium
- frequencyBasis: 実ウィンドウサイズと無関係なアスペクトで視野がずれる
- scope: UC-Play|Camera|StageViewCamera|Contract|Project/Codes/Source/Camera/StageViewCamera.cpp:19
- primary: `StageViewCamera`
- summary: 【Play】ステージカメラのアスペクトが既定解像度固定
- scenarioText: プレイ中カメラが地形・オブジェクトを見失わず、操作に追従する（spec: feature-memos/player_friendly_camera_policy.md）。
- evidence:
  - GetAspectRatio は kDefaultClientWidth/Height のみ（19-21 行）
  - SetupPerspectiveMatrix も同比率（50-52 行）でランタイムリサイズ未反映
  - CombatCameraShake::ApplyStageBattleView が毎フレーム同式を適用（61-73 行）
- suggestedUse: プレイ: 画面比率変更でも戦闘域が切れない視野を維持
- firstSeenAt: 2026-11-10T05:11:00+09:00
- lastSeenAt: 2026-11-10T05:11:00+09:00

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

### GAP-Scene-ChangeSceneFade-003

- status: open
- tier: P2
- score: 630
- primaryUseCase: UC-Play
- scenarioId: PLY-Audio-001
- frequency: high
- frequencyBasis: 暗転中に次シーン Enter で BGM が先行
- scope: UC-Play|Scene|ChangeSceneFade|Incomplete|Project/Codes/Include/Scene/CommandChangeScene.h:114
- primary: `ChangeSceneFade`
- summary: 【Play】フェード中盤で次 BGM が始まる
- scenarioText: プレイ中に BGM／SE が途切れず再生され、シーン遷移後も期待どおり鳴り続ける。
- evidence:
  - isSceneChange 到達で ChangeScene 即実行（CommandChangeScene.h:108-114）
  - 新シーン EnterScene が PlayBgmLoop、旧 Exit が StopBgmLoop
  - フェードイン完了前から次 BGM が鳴り二重／不自然切替（ChangeSceneFade-002 と音声面）
- suggestedUse: プレイ: 暗転完了後に BGM 開始を同期
- firstSeenAt: 2026-11-11T01:11:00+09:00
- lastSeenAt: 2026-11-11T01:11:00+09:00

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

### GAP-Scene-SceneGame-003

- status: open
- tier: P2
- score: 630
- primaryUseCase: UC-Play
- scenarioId: PLY-SceneLoad-001
- frequency: high
- frequencyBasis: 本編シーン入場フレームで背景カメラ参照が未確定
- scope: UC-Play|Scene|SceneGame|Incomplete|Project/Codes/Source/Scene/SceneGame.cpp:95
- primary: `SceneGame`
- summary: 【Play】SpawnEntities がイントロカメラ設定より先に実行される
- scenarioText: プレイヤーがシーンを開き、主要オブジェクトとカメラが安定して動作する。
- evidence:
  - EnterScene で SpawnEntities 後に introCameraController 開始（95-105 行）
  - SkyDome は SpawnEntities 内で GetCamera() を渡す（301 行）がその時点は初期姿勢
- suggestedUse: プレイ: シーン開始直後の背景・主要オブジェクト表示を安定
- firstSeenAt: 2026-11-10T04:01:00+09:00
- lastSeenAt: 2026-11-10T04:01:00+09:00

### GAP-Infra-verify_camera_policy-002

- status: open
- tier: P2
- score: 620
- primaryUseCase: UC-Play
- secondaryUseCases: UC-Ship
- scenarioId: PLY-Camera-001
- frequency: low
- frequencyBasis: 静的検証がエディタシーンの AllowDebugCamera 限定を見ない
- scope: UC-Play|Infra|verify_camera_policy|Incomplete|EditorAssets/Tools/StageViewCameraTuning/verify_camera_policy.py:61
- primary: `verify_camera_policy`
- summary: 【Play】verify_camera_policy が AllowDebugCameraControl を検査しない
- scenarioText: プレイ中カメラが地形・オブジェクトを見失わず、操作に追従する（spec: feature-memos/player_friendly_camera_policy.md）。
- evidence:
  - UNLOCKED_EDITOR は locked false のみ確認（61-67 行）
  - SceneParticleEditor は Allow オーバーライド無しで GameOrder 更新しうる
  - policy 表の AllowDebugCameraControl 行は自動検証対象外
- suggestedUse: 配布: エディタ外での意図しないフリーカメラ更新を CI で検出
- firstSeenAt: 2026-11-10T05:11:00+09:00
- lastSeenAt: 2026-11-10T05:11:00+09:00

### GAP-Scene-ChangeSceneFade-002

- status: open
- tier: P2
- score: 620
- primaryUseCase: UC-Play
- scenarioId: PLY-SceneLoad-001
- frequency: high
- frequencyBasis: 暗転中にシーン入替えし旧シーン描画が止まる
- scope: UC-Play|Scene|ChangeSceneFade|Incomplete|Project/Codes/Source/Scene/ChangeSceneFade.cpp:54
- primary: `ChangeSceneFade`
- summary: 【Play】timeRatio 満了で即 isSceneChange となり中間で EnterScene
- scenarioText: プレイヤーがシーンを開き、主要オブジェクトとカメラが安定して動作する。
- evidence:
  - フェードアウト 0.5s 到達で isSceneChange=true（52-58 行）
  - CommandChangeScene が同フレームで ChangeScene 実行（108-114 行）
  - フェードイン完了までは旧シーン Update 停止のまま（GameOrder.h:69）
- suggestedUse: プレイ: ロード演出とシーン切替タイミングを一貫
- firstSeenAt: 2026-11-10T05:51:00+09:00
- lastSeenAt: 2026-11-10T05:51:00+09:00

### GAP-Scene-SceneGame-012

- status: open
- tier: P2
- score: 620
- primaryUseCase: UC-Play
- scenarioId: PLY-Audio-001
- frequency: medium
- frequencyBasis: イントロ中も BGM 復帰が走る
- scope: UC-Play|Scene|SceneGame|Incomplete|Project/Codes/Source/Scene/SceneGame.cpp:162
- primary: `SceneGame`
- summary: 【Play】イントロ中に Maintain が PlaySceneBgm
- scenarioText: プレイ中に BGM／SE が途切れず再生され、シーン遷移後も期待どおり鳴り続ける。
- evidence:
  - intro 分岐の前で MaintainSceneBgmIfNeeded（162 行）
  - 復帰 PlayBgmLoop→PlaySceneBgm で全ストリーム停止（PlaySceneBgm-001）
  - イントロ SE（PlaySubmitSe 等）が BGM 再開で切れうる
- suggestedUse: プレイ: カットシーン中の BGM/SE 優先度
- firstSeenAt: 2026-11-11T01:41:00+09:00
- lastSeenAt: 2026-11-11T01:41:00+09:00

### GAP-Scene-SceneResult-001

- status: open
- tier: P2
- score: 620
- primaryUseCase: UC-Play
- scenarioId: PLY-Audio-001
- frequency: medium
- frequencyBasis: リザルト BGM の途切れ復帰が SceneGame と非対称
- scope: UC-Play|Scene|SceneResult|Incomplete|Project/Codes/Source/Scene/SceneResult.cpp:41
- primary: `SceneResult`
- summary: 【Play】SceneResult に BGM 維持・再開処理が無い
- scenarioText: プレイ中に BGM／SE が途切れず再生され、シーン遷移後も期待どおり鳴り続ける。
- evidence:
  - EnterScene で PlayBgmLoop のみ（37-38 行）
  - Update に MaintainSceneBgmIfNeeded 相当無し（41-48 行）
- suggestedUse: プレイ: クリア後リザルトで BGM が止まっても自動復帰
- firstSeenAt: 2026-11-10T04:51:00+09:00
- lastSeenAt: 2026-11-10T04:51:00+09:00

### GAP-Audio-AudioManager-001

- status: open
- tier: P2
- score: 610
- primaryUseCase: UC-Play
- scenarioId: PLY-Audio-001
- frequency: medium
- frequencyBasis: 一時停止中も IsPlaying が true
- scope: UC-Play|Audio|AudioManager|Incomplete|Project/Codes/Source/Audio/Audio.cpp:810
- primary: `AudioManager`
- summary: 【Play】BGM 一時停止中は復帰判定が誤る
- scenarioText: プレイ中に BGM／SE が途切れず再生され、シーン遷移後も期待どおり鳴り続ける。
- evidence:
  - IsPlaying は Paused も true（810-811 行、Audio.h:157 コメント）
  - MaintainSceneBgmIfNeeded は IsPlaying のみ見る（SceneGame.cpp:236-238）
  - PauseBgmLoop 未配線と組み合わせると永久無音に見える
- suggestedUse: プレイ: ポーズ／OS 割込み後の BGM 再開
- firstSeenAt: 2026-11-11T01:11:00+09:00
- lastSeenAt: 2026-11-11T01:11:00+09:00

### GAP-Scene-Scene-002

- status: open
- tier: P2
- score: 610
- primaryUseCase: UC-Play
- scenarioId: PLY-Camera-001
- frequency: medium
- frequencyBasis: 新シーンでロック忘れ時に GameOrder が Debug カメラ入力を許す
- scope: UC-Play|Scene|Scene|Contract|Project/Codes/Include/Scene/Scene.h:71
- primary: `Scene`
- summary: 【Play】AllowDebugCameraControl が既定 true でガードが二重依存
- scenarioText: プレイ中カメラが地形・オブジェクトを見失わず、操作に追従する（spec: feature-memos/player_friendly_camera_policy.md）。
- evidence:
  - Scene::AllowDebugCameraControl 既定は true（Scene.h:71）
  - GameOrder はロック false かつ Allow true で camera->Update()（GameOrder.h:63-66）
  - policy の verify は IsStageViewCameraLocked のみ（Allow 未検査）
- suggestedUse: プレイ: 新規シーン追加時のカメラ入力漏れを構造で防ぐ
- roadmapLink: feature-memos/player_friendly_camera_policy.md
- firstSeenAt: 2026-11-10T05:11:00+09:00
- lastSeenAt: 2026-11-10T05:11:00+09:00

### GAP-Scene-SceneGame-006

- status: open
- tier: P2
- score: 610
- primaryUseCase: UC-Play
- scenarioId: PLY-Audio-001
- frequency: medium
- frequencyBasis: 死亡演出中は BGM 維持ロジックが停止
- scope: UC-Play|Scene|SceneGame|Incomplete|Project/Codes/Source/Scene/SceneGame.cpp:228
- primary: `SceneGame`
- summary: 【Play】撃破演出中は MaintainSceneBgmIfNeeded が無効
- scenarioText: プレイ中に BGM／SE が途切れず再生され、シーン遷移後も期待どおり鳴り続ける。
- evidence:
  - pendingDefeat_!=None で即 return（228-230 行）
  - Update 先頭で撃破演出分岐が BGM 維持より先（131-158 行）
  - ボス撃破は OnBossDefeated で BGM 停止済み、自機撃墜は演出中も維持呼び出し無し
- suggestedUse: プレイ: 演出フェーズでもストリーム切れを検知復帰
- firstSeenAt: 2026-11-10T06:41:00+09:00
- lastSeenAt: 2026-11-10T06:41:00+09:00

### GAP-Scene-SceneMovie-005

- status: open
- tier: P2
- score: 610
- primaryUseCase: UC-Play
- scenarioId: PLY-Movie-001
- frequency: medium
- frequencyBasis: EnterScene でスカイドーム生成がカメラトラック開始より先
- scope: UC-Play|Scene|SceneMovie|Incomplete|Project/Codes/Source/Scene/SceneMovie.cpp:62
- primary: `SceneMovie`
- summary: 【Play】SkyDome がトラック先頭姿勢前のカメラに紐づく
- scenarioText: シーン内ムービー／カットシーンが再生完了まで破綻しない（spec: scene_movie.md）。
- evidence:
  - SpawnVisuals が EnterScene で TryBeginFromJsonFile より前（82-88 行）
  - SkyDome は GetCamera() を渡すのみ（62-64 行）で姿勢更新はトラック開始後
- suggestedUse: プレイ: カットシーン冒頭の背景・パララックスのちらつき防止
- roadmapLink: feature-memos/scene_movie.md
- firstSeenAt: 2026-11-10T03:41:00+09:00
- lastSeenAt: 2026-11-10T03:41:00+09:00

### GAP-Audio-AudioManager-004

- status: open
- tier: P2
- score: 600
- primaryUseCase: UC-Play
- scenarioId: PLY-Audio-001
- frequency: medium
- frequencyBasis: ストリーム BGM 一時停止後の再開が薄い
- scope: UC-Play|Audio|AudioManager|Incomplete|Project/Codes/Source/Audio/Audio.cpp:884
- primary: `AudioManager`
- summary: 【Play】Resume は Start のみでデコード再開なし
- scenarioText: プレイ中に BGM／SE が途切れず再生され、シーン遷移後も期待どおり鳴り続ける。
- evidence:
  - Pause で pVoice->Stop（842-843 行）
  - Resume は state 確認後 Start のみ（884-889 行）
  - SubmitInitialBuffer / RefillBuffer を呼ばずバッファ枯渇で無音化しうる
- suggestedUse: プレイ: ポーズ／OS 割込み後のストリーム BGM 復帰
- firstSeenAt: 2026-11-11T02:41:00+09:00
- lastSeenAt: 2026-11-11T02:41:00+09:00

### GAP-Audio-StreamingVoiceCallback-001

- status: open
- tier: P2
- score: 600
- primaryUseCase: UC-Play
- scenarioId: PLY-Audio-001
- frequency: medium
- frequencyBasis: ストリーム終端で Waiting→即停止
- scope: UC-Play|Audio|StreamingVoiceCallback|Incomplete|Project/Codes/Source/Audio/StreamingVoiceCallBack.cpp:13
- primary: `StreamingVoiceCallback`
- summary: 【Play】OnStreamEnd が Waiting のみセット
- scenarioText: プレイ中に BGM／SE が途切れず再生され、シーン遷移後も期待どおり鳴り続ける。
- evidence:
  - OnStreamEnd→Waiting（13-16 行）
  - 非ループは Update で Stop→Stopped（100-110 行）
  - ループ以外の長尺 SE が終端で瞬断
- suggestedUse: プレイ: ストリーム終端のフェード／シーン跨ぎ
- firstSeenAt: 2026-11-11T03:11:00+09:00
- lastSeenAt: 2026-11-11T03:11:00+09:00

### GAP-Scene-GameOrder-002

- status: open
- tier: P2
- score: 600
- primaryUseCase: UC-Play
- scenarioId: PLY-Audio-001
- frequency: medium
- frequencyBasis: GameOrder が audio_ を保持するのみ
- scope: UC-Play|Scene|GameOrder|Incomplete|Project/Codes/Source/Scene/GameOrder.cpp:14
- primary: `GameOrder`
- summary: 【Play】フレームループが音声を管理しない
- scenarioText: プレイ中に BGM／SE が途切れず再生され、シーン遷移後も期待どおり鳴り続ける。
- evidence:
  - コンストラクタで audio_ 取得のみ（GameOrder.cpp:14）
  - Update ループに Pause/Resume や遷移中ダッキング無し（GameOrder.h:61-78）
  - シーン個別（SceneGame Maintain）頼みで統一ポリシーが無い
- suggestedUse: プレイ: 暗転・ポーズ時の BGM/SE を中央制御
- firstSeenAt: 2026-11-11T01:41:00+09:00
- lastSeenAt: 2026-11-11T01:41:00+09:00

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

### GAP-Scene-SceneBuildUp-003

- status: open
- tier: P2
- score: 600
- primaryUseCase: UC-Play
- secondaryUseCases: UC-Debug
- scenarioId: PLY-Audio-001
- frequency: low
- frequencyBasis: BuildUp プレイ検証で SE/BGM 文脈が本編と異なる
- scope: UC-Play|Scene|SceneBuildUp|MissingData|Project/Codes/Source/Scene/SceneBuildUp.cpp:87
- primary: `SceneBuildUp`
- summary: 【Play】SceneBuildUp に BGM／シーン SE の入退場が無い
- scenarioText: プレイ中に BGM／SE が途切れず再生され、シーン遷移後も期待どおり鳴り続ける。
- evidence:
  - SceneBuildUp::EnterScene に Audio 呼び出し無し（87-111 行、rg Bgm/PlaySe 0 件）
  - 本編 SceneGame は Enter/Exit で PlayBgmLoop/StopBgmLoop
- suggestedUse: プレイ: 検証シーンでも戦闘 SE と BGM 文脈を再現
- firstSeenAt: 2026-11-10T02:51:00+09:00
- lastSeenAt: 2026-11-10T02:51:00+09:00

### GAP-Scene-SceneGame-007

- status: open
- tier: P2
- score: 600
- primaryUseCase: UC-Play
- scenarioId: PLY-Audio-001
- frequency: medium
- frequencyBasis: 自機撃墜とボス撃破で BGM 停止ポリシーが非対称
- scope: UC-Play|Scene|SceneGame|Incomplete|Project/Codes/Source/Scene/SceneGame.cpp:516
- primary: `SceneGame`
- summary: 【Play】OnPlayerDefeated は BGM を止めず撃破は即停止
- scenarioText: プレイ中に BGM／SE が途切れず再生され、シーン遷移後も期待どおり鳴り続ける。
- evidence:
  - OnBossDefeated 先頭で StopBgmLoop（498-500 行）
  - OnPlayerDefeated は演出開始のみ（516-523 行）
  - 再生ポリシーがルート分岐ごとに分散
- suggestedUse: プレイ: 撃破／撃墜で一貫した BGM 遷移
- firstSeenAt: 2026-11-10T07:11:00+09:00
- lastSeenAt: 2026-11-10T07:11:00+09:00

### GAP-Scene-SceneGame-010

- status: open
- tier: P2
- score: 600
- primaryUseCase: UC-Play
- scenarioId: PLY-Audio-001
- frequency: medium
- frequencyBasis: 敗北／撃破演出中は BGM 維持をスキップ
- scope: UC-Play|Scene|SceneGame|Incomplete|Project/Codes/Source/Scene/SceneGame.cpp:231
- primary: `SceneGame`
- summary: 【Play】pendingDefeat 中は Maintain が return
- scenarioText: プレイ中に BGM／SE が途切れず再生され、シーン遷移後も期待どおり鳴り続ける。
- evidence:
  - MaintainSceneBgmIfNeeded は pendingDefeat_!=None で即 return（231-233 行）
  - 撃破演出 Update は冒頭 return で Maintain 未到達（134-141 行）
  - ストリーム切断時も演出中は BGM 自動復帰しない
- suggestedUse: プレイ: 長い死亡演出中も BGM 切断を復帰
- firstSeenAt: 2026-11-10T23:41:00+09:00
- lastSeenAt: 2026-11-10T23:41:00+09:00

### GAP-Scene-SceneMovie-007

- status: open
- tier: P2
- score: 600
- primaryUseCase: UC-Play
- scenarioId: PLY-Movie-001
- frequency: medium
- frequencyBasis: トラック失敗時に方針外の StageView 固定カメラへフォールバック
- scope: UC-Play|Scene|SceneMovie|Contract|Project/Codes/Source/Scene/SceneMovie.cpp:90
- primary: `SceneMovie`
- summary: 【Play】トラック開始失敗時に SetupStageViewCamera を呼ぶ
- scenarioText: シーン内ムービー／カットシーンが再生完了まで破綻しない（spec: scene_movie.md）。
- evidence:
  - scene_movie 方針は失敗時即 return（scene_movie.md:14）
  - EnterScene は TryBegin 失敗で SetupStageViewCamera（90-92 行）
  - 次フレーム Update が RequestReturnToCaller（106-108 行）まで一瞬固定視点
- suggestedUse: プレイ: 失敗パスでも MovieCamera 専用ポリシーを一貫
- roadmapLink: feature-memos/scene_movie.md
- firstSeenAt: 2026-11-10T05:31:00+09:00
- lastSeenAt: 2026-11-10T05:31:00+09:00

### GAP-Scene-SceneParticleEditor-001

- status: open
- tier: P2
- score: 600
- primaryUseCase: UC-Play
- secondaryUseCases: UC-Debug
- scenarioId: PLY-Camera-001
- frequency: low
- frequencyBasis: パーティクルエディタで GameOrder が常に DebugCamera 更新しうる
- scope: UC-Play|Scene|SceneParticleEditor|Incomplete|Project/Codes/Include/Scene/SceneParticleEditor.h:29
- primary: `SceneParticleEditor`
- summary: 【Play】SceneParticleEditor が AllowDebugCameraControl を限定していない
- scenarioText: プレイ中カメラが地形・オブジェクトを見失わず、操作に追従する（spec: feature-memos/player_friendly_camera_policy.md）。
- evidence:
  - IsStageViewCameraLocked false（29-30 行）かつ AllowDebugCameraControl オーバーライド無し
  - GameOrder はロックでなければ camera->Update()（GameOrder.h:63-66）
  - player_friendly_camera_policy: エディタはビューポート内のみ Debug 許可と記載
- suggestedUse: 制作: 誤操作でプレイ相当のカメラポリシーを検証から外さない
- roadmapLink: feature-memos/player_friendly_camera_policy.md
- firstSeenAt: 2026-11-10T03:21:00+09:00
- lastSeenAt: 2026-11-10T03:21:00+09:00

### GAP-Audio-AudioManager-002

- status: open
- tier: P2
- score: 590
- primaryUseCase: UC-Play
- scenarioId: PLY-Audio-001
- frequency: medium
- frequencyBasis: ワンショット停止の集約 API が無い
- scope: UC-Play|Audio|AudioManager|Incomplete|Project/Codes/Include/Audio/Audio.h:147
- primary: `AudioManager`
- summary: 【Play】StopAll は BGM 系ループのみ
- scenarioText: プレイ中に BGM／SE が途切れず再生され、シーン遷移後も期待どおり鳴り続ける。
- evidence:
  - Stop(handle) は個別のみ（147 行）
  - StopAllLoopingShortVoices は PlaySceneBgm/StopBgmLoop 経路（295, 686 行）
  - 非ループ Play 音のシーン一括停止ポリシーなし
- suggestedUse: プレイ: ExitScene で戦闘 SE を確実に消す
- firstSeenAt: 2026-11-11T02:11:00+09:00
- lastSeenAt: 2026-11-11T02:11:00+09:00

### GAP-Scene-SceneGame-008

- status: open
- tier: P2
- score: 590
- primaryUseCase: UC-Play
- scenarioId: PLY-Audio-001
- frequency: medium
- frequencyBasis: イントロ中はエンティティ Update 停止で SE 発火も止まる
- scope: UC-Play|Scene|SceneGame|Incomplete|Project/Codes/Source/Scene/SceneGame.cpp:161
- primary: `SceneGame`
- summary: 【Play】イントロ中の Update 早期 return で弾幕 SE 無音
- scenarioText: プレイ中に BGM／SE が途切れず再生され、シーン遷移後も期待どおり鳴り続ける。
- evidence:
  - イントロ未完で return（161-169 行）、UpdateEntities 未到達
  - BGM は EnterScene で開始し Maintain はその前に実行（115,159 行）
  - 演出中の戦闘音效と BGM だけ先行の体感ばらつき
- suggestedUse: プレイ: イントロと戦闘音のポリシーを統一
- firstSeenAt: 2026-11-10T07:41:00+09:00
- lastSeenAt: 2026-11-10T07:41:00+09:00

### GAP-Audio-AudioManager-005

- status: open
- tier: P2
- score: 580
- primaryUseCase: UC-Play
- scenarioId: PLY-Audio-001
- frequency: medium
- frequencyBasis: 一時停止中にデコード補充が止まる
- scope: UC-Play|Audio|AudioManager|Incomplete|Project/Codes/Source/Audio/Audio.cpp:82
- primary: `AudioManager`
- summary: 【Play】Paused ストリームは Update をスキップ
- scenarioText: プレイ中に BGM／SE が途切れず再生され、シーン遷移後も期待どおり鳴り続ける。
- evidence:
  - state==Paused で continue（82-84 行）
  - RefillBuffer / TryRecover 未到達
  - ResumeSceneBgm-004 と合わせバッファ枯渇で無音化しうる
- suggestedUse: プレイ: ポーズ中もストリームキューを維持
- firstSeenAt: 2026-11-11T03:11:00+09:00
- lastSeenAt: 2026-11-11T03:11:00+09:00

### GAP-Audio-PlayBgmLoop-001

- status: open
- tier: P2
- score: 580
- primaryUseCase: UC-Play
- scenarioId: PLY-Audio-001
- frequency: medium
- frequencyBasis: BGM 再試行が二度 PlaySceneBgm を呼ぶ
- scope: UC-Play|Audio|PlayBgmLoop|Incomplete|Project/Codes/Include/System/PlaySe.h:187
- primary: `PlayBgmLoop`
- summary: 【Play】PlayBgmLoop 失敗時に PlaySceneBgm を再実行
- scenarioText: プレイ中に BGM／SE が途切れず再生され、シーン遷移後も期待どおり鳴り続ける。
- evidence:
  - 失敗時 1 度だけ再 PlaySceneBgm（185-188 行）
  - 各回が StopAllStreamingVoices を呼ぶ（Audio.cpp:687）
  - ストリーム復帰試行で戦闘 SE が二重切断しうる
- suggestedUse: プレイ: BGM 再開と SE 共存
- firstSeenAt: 2026-11-10T07:41:00+09:00
- lastSeenAt: 2026-11-10T07:41:00+09:00

### GAP-Render-SceneTitle-001

- status: open
- tier: P2
- score: 580
- primaryUseCase: UC-Play
- scenarioId: PLY-Visual-001
- frequency: medium
- frequencyBasis: タイトル→本編遷移でリム／フォグパイプラインが切り替わる
- scope: UC-Play|Render|SceneTitle|MissingData|Project/Codes/Source/Scene/SceneTitle.cpp:16
- primary: `SceneTitle`
- summary: 【Play】SceneTitle のライティングにリム・フォグ・ゴッドレイが無い
- scenarioText: ランタイムでライト・フォグ等の見た目がデータどおり反映される（spec: rim_light / volumetric_fog）。
- evidence:
  - SetupStageLighting は DirectionalLightData のみ（SceneTitle.cpp:16-25）
  - rimEnabled や RefVolumetricFog 未設定で本編 SceneGame と見た目ポリシー不一致
- suggestedUse: プレイ: シーン遷移時に見た目が急変しない
- roadmapLink: feature-memos/rim_light.md
- firstSeenAt: 2026-11-10T04:21:00+09:00
- lastSeenAt: 2026-11-10T04:21:00+09:00

### GAP-Runtime-MovieCameraTrackLoader-002

- status: open
- tier: P2
- score: 580
- primaryUseCase: UC-Play
- scenarioId: PLY-Movie-001
- frequency: medium
- frequencyBasis: JSON 一部キーフレーム不正で全体ロード失敗
- scope: UC-Play|Runtime|MovieCameraTrackLoader|Incomplete|Project/Codes/Source/Camera/MovieCameraTrackLoader.cpp:58
- primary: `MovieCameraTrackLoader`
- summary: 【Play】キーフレーム 1 件の不正でトラック全体を拒否
- scenarioText: シーン内ムービー／カットシーンが再生完了まで破綻しない（spec: scene_movie.md）。
- evidence:
  - ParsePose 失敗で即 return false（58-60 行）
  - エラー位置・理由のログ／UI なし
  - EnterScene では単に trackPlaybackStarted_=false（SceneMovie.cpp:87-88）
- suggestedUse: 制作: 壊れた JSON を部分再生または明確に修正指示
- roadmapLink: feature-memos/movie_camera_controller.md
- firstSeenAt: 2026-11-10T05:31:00+09:00
- lastSeenAt: 2026-11-10T05:31:00+09:00

### GAP-Audio-AudioManager-007

- status: open
- tier: P2
- score: 570
- primaryUseCase: UC-Play
- scenarioId: PLY-Audio-001
- frequency: medium
- frequencyBasis: ストリーム BGM 失敗で playId が不整合
- scope: UC-Play|Audio|AudioManager|Contract|Project/Codes/Source/Audio/Audio.cpp:719
- primary: `AudioManager`
- summary: 【Play】Handle と voice の playId がずれる
- scenarioText: プレイ中に BGM／SE が途切れず再生され、シーン遷移後も期待どおり鳴り続ける。
- evidence:
  - 失敗時 voice.playId=65536（710-711 行）
  - 返却 Handle は currentPlayId のまま（714-719 行）
  - IsPlaying/Stop が voice と不一致になり復帰試行が空振りしうる
- suggestedUse: プレイ: ストリーム BGM 再開ハンドル契約
- firstSeenAt: 2026-11-11T03:41:00+09:00
- lastSeenAt: 2026-11-11T03:41:00+09:00

### GAP-Audio-ResumeBgmLoop-001

- status: open
- tier: P2
- score: 570
- primaryUseCase: UC-Play
- scenarioId: PLY-Audio-001
- frequency: low
- frequencyBasis: フェード後の BGM 再開 API が未配線
- scope: UC-Play|Audio|ResumeBgmLoop|Unwired|Project/Codes/Include/System/PlaySe.h:119
- primary: `ResumeBgmLoop`
- summary: 【Play】ResumeBgmLoop がシーンから呼ばれない
- scenarioText: プレイ中に BGM／SE が途切れず再生され、シーン遷移後も期待どおり鳴り続ける。
- evidence:
  - ResumeSceneBgm ラッパーのみ（119-124 行）
  - rg ResumeBgmLoop は定義のみ（PauseBgmLoop-001 参照と同系）
  - フェードイン後に Pause した BGM を復帰するシーン共通処理無し
- suggestedUse: プレイ: 暗転を挟んでも BGM を再開可能に
- firstSeenAt: 2026-11-10T07:11:00+09:00
- lastSeenAt: 2026-11-10T07:11:00+09:00

### GAP-Render-SceneResult-001

- status: open
- tier: P2
- score: 570
- primaryUseCase: UC-Play
- scenarioId: PLY-Visual-001
- frequency: medium
- frequencyBasis: 本編→リザルトでリム／フォグが消える
- scope: UC-Play|Render|SceneResult|Incomplete|Project/Codes/Source/Scene/SceneResult.cpp:52
- primary: `SceneResult`
- summary: 【Play】SceneResult が毎フレーム簡易平行光のみ適用
- scenarioText: ランタイムでライト・フォグ等の見た目がデータどおり反映される（spec: rim_light / volumetric_fog）。
- evidence:
  - DrawScene で SetupStageLighting のみ（52 行）
  - rimEnabled や RefVolumetricFog 未設定（16-25 行）
  - 本編終了後に UI 背景の見え方が急変
- suggestedUse: プレイ: クリア画面でも本編と同系のシルエット可読性
- roadmapLink: feature-memos/rim_light.md
- firstSeenAt: 2026-11-10T06:11:00+09:00
- lastSeenAt: 2026-11-10T06:11:00+09:00

### GAP-Scene-SceneGame-011

- status: open
- tier: P2
- score: 570
- primaryUseCase: UC-Play
- scenarioId: PLY-Audio-001
- frequency: medium
- frequencyBasis: ExitScene で BGM を即停止
- scope: UC-Play|Scene|SceneGame|Incomplete|Project/Codes/Source/Scene/SceneGame.cpp:122
- primary: `SceneGame`
- summary: 【Play】フェード中も Exit で StopBgmLoop
- scenarioText: プレイ中に BGM／SE が途切れず再生され、シーン遷移後も期待どおり鳴り続ける。
- evidence:
  - ExitScene で StopBgmLoop（122-125 行）
  - リスタート／リザルト遷移は ChangeSceneFade 経由
  - SceneTitle-002・SceneResult-002 と同様の暗転中音切れ
- suggestedUse: プレイ: ゲーム→他シーンの BGM をフェード連動
- firstSeenAt: 2026-11-11T00:41:00+09:00
- lastSeenAt: 2026-11-11T00:41:00+09:00

### GAP-Scene-SceneTitle-002

- status: open
- tier: P2
- score: 570
- primaryUseCase: UC-Play
- scenarioId: PLY-Audio-001
- frequency: medium
- frequencyBasis: タイトル離脱時 ExitScene で BGM を即停止
- scope: UC-Play|Scene|SceneTitle|Incomplete|Project/Codes/Source/Scene/SceneTitle.cpp:67
- primary: `SceneTitle`
- summary: 【Play】ExitScene がフェード完了前に StopBgmLoop
- scenarioText: プレイ中に BGM／SE が途切れず再生され、シーン遷移後も期待どおり鳴り続ける。
- evidence:
  - ExitScene で StopBgmLoop（67-69 行）
  - 遷移は ChangeSceneFade 経由で暗転中も旧シーンが一時表示（CommandScene-001）
  - タイトル BGM 維持ロジック無し（SceneTitle-001）
- suggestedUse: プレイ: タイトル→ゲームの音切りをフェードに合わせる
- firstSeenAt: 2026-11-10T07:41:00+09:00
- lastSeenAt: 2026-11-10T07:41:00+09:00

### GAP-Audio-AudioManager-009

- status: open
- tier: P2
- score: 560
- primaryUseCase: UC-Play
- scenarioId: PLY-Audio-001
- frequency: medium
- frequencyBasis: 初期再生後の Refill が 1 回
- scope: UC-Play|Audio|AudioManager|Incomplete|Project/Codes/Source/Audio/Audio.cpp:554
- primary: `AudioManager`
- summary: 【Play】Start 後に Refill を 1 回だけ
- scenarioText: プレイ中に BGM／SE が途切れず再生され、シーン遷移後も期待どおり鳴り続ける。
- evidence:
  - SubmitInitialBuffer→StartStreaming 後 Refill 1 回（549-554 行）
  - ダブルバッファ満タン前提が弱くアンダーランしうる
  - Update の Streaming 状態遷移と初期ギャップが連鎖
- suggestedUse: プレイ: ストリーム開始時のプリロード強化
- firstSeenAt: 2026-11-11T04:11:00+09:00
- lastSeenAt: 2026-11-11T04:11:00+09:00

### GAP-Audio-SetBgmLoopVolume-001

- status: open
- tier: P2
- score: 560
- primaryUseCase: UC-Play
- scenarioId: PLY-Audio-001
- frequency: low
- frequencyBasis: クロスフェード用音量 API が未使用
- scope: UC-Play|Audio|SetBgmLoopVolume|Unwired|Project/Codes/Include/System/PlaySe.h:166
- primary: `SetBgmLoopVolume`
- summary: 【Play】SetBgmLoopVolume が未使用
- scenarioText: プレイ中に BGM／SE が途切れず再生され、シーン遷移後も期待どおり鳴り続ける。
- evidence:
  - SetBgmLoopVolume は SetSeVolume 委譲のみ（166-168 行）
  - シーン／フェードからの呼び出し 0 件
  - 遷移は Stop→Play の瞬断のみ（CommandScene-001 と整合）
- suggestedUse: プレイ: 遷移中の BGM 音量フェード
- firstSeenAt: 2026-11-10T07:11:00+09:00
- lastSeenAt: 2026-11-10T07:11:00+09:00

### GAP-Scene-SceneGame-009

- status: open
- tier: P2
- score: 560
- primaryUseCase: UC-Play
- scenarioId: PLY-Audio-001
- frequency: medium
- frequencyBasis: フェード遷移中は BGM 切断検知が止まる
- scope: UC-Play|Scene|SceneGame|Incomplete|Project/Codes/Source/Scene/SceneGame.cpp:158
- primary: `SceneGame`
- summary: 【Play】遷移演出中は MaintainSceneBgm を呼ばない
- scenarioText: プレイ中に BGM／SE が途切れず再生され、シーン遷移後も期待どおり鳴り続ける。
- evidence:
  - GetIsExecute 時は Update 冒頭で return（158-160 行）
  - MaintainSceneBgmIfNeeded はその手前のみ到達
  - 遷移暗転中にストリーミング切れても再試行しない
- suggestedUse: プレイ: フェード中も BGM 生存監視
- firstSeenAt: 2026-11-10T23:11:00+09:00
- lastSeenAt: 2026-11-10T23:11:00+09:00

### GAP-Scene-SceneResult-002

- status: open
- tier: P2
- score: 560
- primaryUseCase: UC-Play
- scenarioId: PLY-Audio-001
- frequency: medium
- frequencyBasis: リザルト離脱で BGM がフェード前に停止
- scope: UC-Play|Scene|SceneResult|Incomplete|Project/Codes/Source/Scene/SceneResult.cpp:62
- primary: `SceneResult`
- summary: 【Play】ExitScene が即 StopBgmLoop
- scenarioText: プレイ中に BGM／SE が途切れず再生され、シーン遷移後も期待どおり鳴り続ける。
- evidence:
  - ExitScene で StopBgmLoop（62-63 行）
  - 遷移は ChangeSceneFade 経由（45-47 行）
  - SceneTitle-002 と同様に暗転中の音切れが起きうる
- suggestedUse: プレイ: リザルト→タイトルの BGM をフェードに合わせる
- firstSeenAt: 2026-11-11T00:11:00+09:00
- lastSeenAt: 2026-11-11T00:11:00+09:00

### GAP-Audio-AudioManager-008

- status: open
- tier: P2
- score: 550
- primaryUseCase: UC-Play
- scenarioId: PLY-Audio-001
- frequency: medium
- frequencyBasis: ストリーム失敗後に短音経路へ奪取
- scope: UC-Play|Audio|AudioManager|Incomplete|Project/Codes/Source/Audio/Audio.cpp:728
- primary: `AudioManager`
- summary: 【Play】BGM フォールバックで短音ボイス解放
- scenarioText: プレイ中に BGM／SE が途切れず再生され、シーン遷移後も期待どおり鳴り続ける。
- evidence:
  - PlayShort 失敗後 ReleaseWaitingShortVoices（728-729 行）
  - 遷移直後のワンショット SE が待機中から切られる
  - ストリーム非対応 BGM と短音 SE の同居が脆い
- suggestedUse: プレイ: シーン遷移時の SE 優先度
- firstSeenAt: 2026-11-11T03:41:00+09:00
- lastSeenAt: 2026-11-11T03:41:00+09:00

### GAP-Audio-AudioManager-010

- status: open
- tier: P2
- score: 550
- primaryUseCase: UC-Play
- scenarioId: PLY-Audio-001
- frequency: medium
- frequencyBasis: BGM 一時停止が Stop のみ
- scope: UC-Play|Audio|AudioManager|Incomplete|Project/Codes/Source/Audio/Audio.cpp:821
- primary: `AudioManager`
- summary: 【Play】ストリーム BGM 停止は XAudio Stop
- scenarioText: プレイ中に BGM／SE が途切れず再生され、シーン遷移後も期待どおり鳴り続ける。
- evidence:
  - PauseSceneBgm は pVoice->Stop のみ（842-843 行）
  - キュー済みバッファや Streaming 状態を整理しない
  - ポーズ解除やシーン復帰で途切れ・クリックが出うる
- suggestedUse: プレイ: ポーズ中も BGM キューを維持
- firstSeenAt: 2026-11-11T04:41:00+09:00
- lastSeenAt: 2026-11-11T04:41:00+09:00

### GAP-Audio-AudioManager-006

- status: open
- tier: P3
- score: 540
- primaryUseCase: UC-Play
- scenarioId: PLY-Audio-001
- frequency: low
- frequencyBasis: ストリーム BGM 秒数が 44.1kHz 固定
- scope: UC-Play|Audio|AudioManager|Contract|Project/Codes/Source/Audio/Audio.cpp:972
- primary: `AudioManager`
- summary: 【Play】再生位置計算が 44100 固定
- scenarioText: プレイ中に BGM／SE が途切れず再生され、シーン遷移後も期待どおり鳴り続ける。
- evidence:
  - kStreamSampleRate = 44100（972-974 行）
  - Short 経路は wfEx.nSamplesPerSec を参照（993-999 行）
  - MF PCM 強制 44.1kHz と整合はするが将来レート差でズレ
- suggestedUse: プレイ: ストリーム BGM のシーク／ループ位置
- firstSeenAt: 2026-11-11T03:11:00+09:00
- lastSeenAt: 2026-11-11T03:11:00+09:00

### GAP-Audio-AudioManager-011

- status: open
- tier: P2
- score: 540
- primaryUseCase: UC-Play
- scenarioId: PLY-Audio-001
- frequency: medium
- frequencyBasis: Resume が Start のみ
- scope: UC-Play|Audio|AudioManager|Incomplete|Project/Codes/Source/Audio/Audio.cpp:884
- primary: `AudioManager`
- summary: 【Play】Resume は Start 後 Refill なし
- scenarioText: プレイ中に BGM／SE が途切れず再生され、シーン遷移後も期待どおり鳴り続ける。
- evidence:
  - ストリーム Paused→Playing は Start のみ（884-889 行）
  - SubmitInitialBuffer/Refill を再実行しない
  - 長時間ポーズ後に無音やアンダーラン
- suggestedUse: プレイ: 復帰時のストリーム再プライム
- firstSeenAt: 2026-11-11T04:41:00+09:00
- lastSeenAt: 2026-11-11T04:41:00+09:00

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

### GAP-Scene-SceneMovie-008

- status: open
- tier: P3
- score: 540
- primaryUseCase: UC-Play
- scenarioId: PLY-Movie-001
- frequency: low
- frequencyBasis: フェード復帰待ち中に更新が止まり描画だけ続く
- scope: UC-Play|Scene|SceneMovie|Incomplete|Project/Codes/Source/Scene/SceneMovie.cpp:96
- primary: `SceneMovie`
- summary: 【Play】遷移予約後はカメラ／SkyDome 更新が停止
- scenarioText: シーン内ムービー／カットシーンが再生完了まで破綻しない（spec: scene_movie.md）。
- evidence:
  - hasPendingSceneChange_ で Update 全体を return（96-98 行）
  - movieCamera_.Update／skyDome_->Update は以降呼ばれない（111-115 行）
  - DrawScene はスカイ描画を継続（122-125 行）
- suggestedUse: プレイ: フェードアウト中もカメラ終端姿勢／背景アニメを維持
- roadmapLink: feature-memos/scene_movie.md
- firstSeenAt: 2026-11-10T05:31:00+09:00
- lastSeenAt: 2026-11-10T05:31:00+09:00

### GAP-Audio-PlaySceneBgm-002

- status: open
- tier: P2
- score: 530
- primaryUseCase: UC-Play
- scenarioId: PLY-Audio-001
- frequency: low
- frequencyBasis: 失敗時 playId 不一致
- scope: UC-Play|Audio|PlaySceneBgm|Incomplete|Project/Codes/Source/Audio/Audio.cpp:710
- primary: `PlaySceneBgm`
- summary: 【Play】Init 失敗で voice playId だけ巻き戻し
- scenarioText: プレイ中に BGM／SE が途切れず再生され、シーン遷移後も期待どおり鳴り続ける。
- evidence:
  - InitializeStreaming 失敗時 streamingVoice.playId=65536（710-712 行）
  - 返却 Handle は currentPlayId のまま isSuccess=false
  - 再試行 PlayBgmLoop が古い Handle で Stop/Pause できない
- suggestedUse: プレイ: シーン遷移 BGM 失敗のハンドル整合
- firstSeenAt: 2026-11-11T04:41:00+09:00
- lastSeenAt: 2026-11-11T04:41:00+09:00

### GAP-Audio-AudioDecoder-001

- status: open
- tier: P3
- score: 520
- primaryUseCase: UC-Play
- scenarioId: PLY-Audio-001
- frequency: low
- frequencyBasis: PCM 端数バイトを remaining へ退避
- scope: UC-Play|Audio|AudioDecoder|Incomplete|Project/Codes/Source/Audio/AudioDecoder.cpp:124
- primary: `AudioDecoder`
- summary: 【Play】4 バイト揃えで端数を次回へ
- scenarioText: プレイ中に BGM／SE が途切れず再生され、シーン遷移後も期待どおり鳴り続ける。
- evidence:
  - remainder!=0 で validBytes を切り詰め（124-140 行）
  - ストリーム境界で無音／クリックが出うる
  - MF デコードと XAUDIO2 PCM ブロック境界の仮定
- suggestedUse: プレイ: 長尺ストリーム SE の途切れ診断
- firstSeenAt: 2026-11-11T03:41:00+09:00
- lastSeenAt: 2026-11-11T03:41:00+09:00

### GAP-Audio-IsBgmLoopPaused-001

- status: open
- tier: P3
- score: 520
- primaryUseCase: UC-Play
- scenarioId: PLY-Audio-001
- frequency: low
- frequencyBasis: BGM 一時停止状態を UI／ロジックが参照できない
- scope: UC-Play|Audio|IsBgmLoopPaused|Unwired|Project/Codes/Include/System/PlaySe.h:127
- primary: `IsBgmLoopPaused`
- summary: 【Play】IsBgmLoopPaused がシーンから未使用
- scenarioText: プレイ中に BGM／SE が途切れず再生され、シーン遷移後も期待どおり鳴り続ける。
- evidence:
  - PlaySe.h に定義のみ（127-129 行）
  - Pause/Resume API は gap 化済みだが状態照会が無い
  - シーン側は IsPlaying のみ（MaintainSceneBgmIfNeeded）
- suggestedUse: プレイ: ポーズメニューと BGM 状態の同期
- firstSeenAt: 2026-11-10T23:11:00+09:00
- lastSeenAt: 2026-11-10T23:11:00+09:00

### GAP-Audio-IsSuitableStreaming-001

- status: open
- tier: P3
- score: 520
- primaryUseCase: UC-Play
- scenarioId: PLY-Audio-001
- frequency: low
- frequencyBasis: 5MB 閾値のみで経路が分岐
- scope: UC-Play|Audio|IsSuitableStreaming|Contract|Project/Codes/Source/Audio/Audio.cpp:191
- primary: `IsSuitableStreaming`
- summary: 【Play】ファイルサイズだけでストリーム判定
- scenarioText: プレイ中に BGM／SE が途切れず再生され、シーン遷移後も期待どおり鳴り続ける。
- evidence:
  - GetFileSize > kFileSizeThreshold（5MB, Audio.h:70）のみ（191-193 行）
  - 短い高ビットレート BGM は全載せ、長い低容量は非ストリーム
  - 遷移後の BGM 経路・メモリピークがアセット設計に依存
- suggestedUse: プレイ: BGM 長尺のロード／ストリーム方針を明示
- firstSeenAt: 2026-11-11T02:41:00+09:00
- lastSeenAt: 2026-11-11T02:41:00+09:00

### GAP-Audio-StreamingVoiceCallback-002

- status: open
- tier: P3
- score: 520
- primaryUseCase: UC-Play
- scenarioId: PLY-Audio-001
- frequency: medium
- frequencyBasis: バッファ終了で state のみ更新
- scope: UC-Play|Audio|StreamingVoiceCallback|Incomplete|Project/Codes/Source/Audio/StreamingVoiceCallBack.cpp:7
- primary: `StreamingVoiceCallback`
- summary: 【Play】OnBufferEnd が Streaming へ遷移
- scenarioText: プレイ中に BGM／SE が途切れず再生され、シーン遷移後も期待どおり鳴り続ける。
- evidence:
  - OnBufferEnd は parentVoice->state=Streaming（7-9 行）
  - 同フレーム Refill に依存し Update 待ち
  - 高負荷時にキュー枯渇とクリックが出うる
- suggestedUse: プレイ: コールバック内プリフィル検討
- firstSeenAt: 2026-11-11T04:11:00+09:00
- lastSeenAt: 2026-11-11T04:11:00+09:00

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

### GAP-Scene-SceneTitle-003

- status: open
- tier: P3
- score: 520
- primaryUseCase: UC-Play
- scenarioId: PLY-Audio-001
- frequency: medium
- frequencyBasis: 決定 SE と暗転開始が同フレーム
- scope: UC-Play|Scene|SceneTitle|Incomplete|Project/Codes/Source/Scene/SceneTitle.cpp:50
- primary: `SceneTitle`
- summary: 【Play】Submit 直後にフェード遷移
- scenarioText: プレイ中に BGM／SE が途切れず再生され、シーン遷移後も期待どおり鳴り続ける。
- evidence:
  - isSubmitTrigger で PlaySubmitSe の直後 ChangeScene（50-52 行）
  - ExitScene が StopBgmLoop（67 行）だが Submit は Handle 無し
  - 暗転中にワンショットと次 BGM の重なり（ChangeSceneFade-003 と同系）
- suggestedUse: プレイ: タイトル→ゲームの SE/BGM 手応え
- firstSeenAt: 2026-11-11T02:11:00+09:00
- lastSeenAt: 2026-11-11T02:11:00+09:00

### GAP-Audio-GetBgmLoopPlaybackSeconds-001

- status: open
- tier: P3
- score: 510
- primaryUseCase: UC-Play
- scenarioId: PLY-Audio-001
- frequency: low
- frequencyBasis: BGM 再生位置のシーン復元 API が未接続
- scope: UC-Play|Audio|GetBgmLoopPlaybackSeconds|Unwired|Project/Codes/Include/System/PlaySe.h:132
- primary: `GetBgmLoopPlaybackSeconds`
- summary: 【Play】再生秒取得がゲームシーン未使用
- scenarioText: プレイ中に BGM／SE が途切れず再生され、シーン遷移後も期待どおり鳴り続ける。
- evidence:
  - Get/SetBgmLoopPlaybackSeconds が PlaySe.h のみ（132-145 行）
  - シーン再入時は毎回 PlayBgmLoop 先頭から（SceneTitle/Game/Result）
  - 遷移後も期待位置の継続ポリシーなし
- suggestedUse: プレイ: シーン戻りで BGM 位置を復元
- firstSeenAt: 2026-11-10T23:11:00+09:00
- lastSeenAt: 2026-11-10T23:11:00+09:00

### GAP-Audio-StopSe-001

- status: open
- tier: P3
- score: 510
- primaryUseCase: UC-Play
- scenarioId: PLY-Audio-001
- frequency: low
- frequencyBasis: StopSe がシーン退出で未使用
- scope: UC-Play|Audio|StopSe|Unwired|Project/Codes/Include/System/PlaySe.h:97
- primary: `StopSe`
- summary: 【Play】ループ SE ハンドルの個別停止が無い
- scenarioText: プレイ中に BGM／SE が途切れず再生され、シーン遷移後も期待どおり鳴り続ける。
- evidence:
  - StopSe は Handle 単位停止（97-104 行）
  - シーン Exit は StopBgmLoop のみ（SceneTitle/Game/Result）
  - PlaySeLoop で開始した SE が遷移後も残りうる
- suggestedUse: プレイ: 環境ループ SE の明示破棄
- firstSeenAt: 2026-11-11T00:41:00+09:00
- lastSeenAt: 2026-11-11T00:41:00+09:00

### GAP-Audio-PlaySeDetail-002

- status: open
- tier: P3
- score: 500
- primaryUseCase: UC-Play
- scenarioId: PLY-Audio-001
- frequency: medium
- frequencyBasis: SE index キャッシュが永久
- scope: UC-Play|Audio|PlaySeDetail|Contract|Project/Codes/Include/System/PlaySe.h:32
- primary: `PlaySeDetail`
- summary: 【Play】キャッシュ命中で再 Load しない
- scenarioText: プレイ中に BGM／SE が途切れず再生され、シーン遷移後も期待どおり鳴り続ける。
- evidence:
  - cache ヒットは index 返却のみ（22-32 行）
  - ファイル差し替えやストリーム閾値変化を反映しない
  - PlaySeDetail-001 の path キャッシュと長尺 SE 経路が固定化
- suggestedUse: プレイ: 開発中の SE 差し替えを無停止反映
- firstSeenAt: 2026-11-11T04:11:00+09:00
- lastSeenAt: 2026-11-11T04:11:00+09:00

### GAP-Audio-SetBgmLoopPlaybackSeconds-001

- status: open
- tier: P3
- score: 500
- primaryUseCase: UC-Play
- scenarioId: PLY-Audio-001
- frequency: low
- frequencyBasis: BGM シーク API がシーン遷移に未使用
- scope: UC-Play|Audio|SetBgmLoopPlaybackSeconds|Unwired|Project/Codes/Include/System/PlaySe.h:141
- primary: `SetBgmLoopPlaybackSeconds`
- summary: 【Play】再生位置指定がゲームシーン未使用
- scenarioText: プレイ中に BGM／SE が途切れず再生され、シーン遷移後も期待どおり鳴り続ける。
- evidence:
  - SetBgmLoopPlaybackSeconds は Manager 委譲のみ（141-145 行）
  - 取得側 GAP 済みだが Title/Game/Result は再開時常に先頭から
  - Pause/Resume と組み合わせた位置復元ポリシーなし
- suggestedUse: プレイ: シーン戻りで BGM 位置を復元
- firstSeenAt: 2026-11-10T23:41:00+09:00
- lastSeenAt: 2026-11-10T23:41:00+09:00

### GAP-Audio-SetBgmLoopPitch-001

- status: open
- tier: P3
- score: 490
- primaryUseCase: UC-Play
- scenarioId: PLY-Audio-001
- frequency: low
- frequencyBasis: BGM ピッチ API が未配線
- scope: UC-Play|Audio|SetBgmLoopPitch|Unwired|Project/Codes/Include/System/PlaySe.h:163
- primary: `SetBgmLoopPitch`
- summary: 【Play】SetBgmLoopPitch がシーンから未呼び出し
- scenarioText: プレイ中に BGM／SE が途切れず再生され、シーン遷移後も期待どおり鳴り続ける。
- evidence:
  - SetBgmLoopPitch は SetSeFrequencyRatio 委譲（163-165 行）
  - rg SetBgmLoopPitch は PlaySe.h 定義のみ
  - スロー演出・ポーズ中の音程調整が BGM に反映されない
- suggestedUse: プレイ: 演出に合わせた BGM ピッチ制御
- firstSeenAt: 2026-11-10T23:41:00+09:00
- lastSeenAt: 2026-11-10T23:41:00+09:00

### GAP-Audio-SetSeVolume-001

- status: open
- tier: P3
- score: 490
- primaryUseCase: UC-Play
- scenarioId: PLY-Audio-001
- frequency: low
- frequencyBasis: ループ SE／BGM 音量 API が未配線
- scope: UC-Play|Audio|SetSeVolume|Unwired|Project/Codes/Include/System/PlaySe.h:155
- primary: `SetSeVolume`
- summary: 【Play】SetSeVolume がゲームロジックから未使用
- scenarioText: プレイ中に BGM／SE が途切れず再生され、シーン遷移後も期待どおり鳴り続ける。
- evidence:
  - SetSeVolume は AudioManager::SetHandleVolume 委譲（155-157 行）
  - rg SetSeVolume は PlaySe.h 定義のみ（SetBgmLoopVolume-001 と対）
  - ダメージ演出などで SE 音量を下げる契約が無い
- suggestedUse: プレイ: ヒットストップ中の SE ダッキング
- firstSeenAt: 2026-11-11T01:11:00+09:00
- lastSeenAt: 2026-11-11T01:11:00+09:00

### GAP-Audio-GetSeVolume-001

- status: open
- tier: P3
- score: 480
- primaryUseCase: UC-Play
- scenarioId: PLY-Audio-001
- frequency: low
- frequencyBasis: 音量照会 API がゲーム未使用
- scope: UC-Play|Audio|GetSeVolume|Unwired|Project/Codes/Include/System/PlaySe.h:159
- primary: `GetSeVolume`
- summary: 【Play】GetSeVolume がシーンから未呼び出し
- scenarioText: プレイ中に BGM／SE が途切れず再生され、シーン遷移後も期待どおり鳴り続ける。
- evidence:
  - GetSeVolume は Manager 委譲のみ（159-161 行）
  - SetBgmLoopVolume は GAP 済みで取得側未配線
  - フェード／ポーズ UI と実音量の同期ポリシーなし
- suggestedUse: プレイ: BGM ダッキングの状態復元
- firstSeenAt: 2026-11-11T00:11:00+09:00
- lastSeenAt: 2026-11-11T00:11:00+09:00

### GAP-Audio-PlaySeDetail-001

- status: open
- tier: P3
- score: 480
- primaryUseCase: UC-Play
- scenarioId: PLY-Audio-001
- frequency: low
- frequencyBasis: SE パスキャッシュが無期限保持
- scope: UC-Play|Audio|PlaySeDetail|Incomplete|Project/Codes/Include/System/PlaySe.h:19
- primary: `PlaySeDetail`
- summary: 【Play】static cache がアンロードされない
- scenarioText: プレイ中に BGM／SE が途切れず再生され、シーン遷移後も期待どおり鳴り続ける。
- evidence:
  - GetOrLoadSeIndex の static unordered_map（19-32 行）
  - シーン遷移・長時間プレイでエントリ追記のみ
  - ボイス停止とデコーダキャッシュの整合ポリシーなし
- suggestedUse: プレイ: 長時間セッションの SE メモリ管理
- firstSeenAt: 2026-11-11T00:41:00+09:00
- lastSeenAt: 2026-11-11T00:41:00+09:00

### GAP-Audio-PlaySeLoop-001

- status: open
- tier: P3
- score: 480
- primaryUseCase: UC-Play
- scenarioId: PLY-Audio-001
- frequency: low
- frequencyBasis: 環境ループ SE API がシーンから未使用
- scope: UC-Play|Audio|PlaySeLoop|Unwired|Project/Codes/Include/System/PlaySe.h:87
- primary: `PlaySeLoop`
- summary: 【Play】PlaySeLoop がコードベースで未呼び出し
- scenarioText: プレイ中に BGM／SE が途切れず再生され、シーン遷移後も期待どおり鳴り続ける。
- evidence:
  - PlaySeLoop は Manager::Play(index, true) を返すのみ（87-94 行）
  - rg PlaySeLoop は PlaySe.h 定義のみ
  - 本編環境音・警告ループ等の層が BGM 以外に無い
- suggestedUse: プレイ: BGM と独立したループ SE 層
- firstSeenAt: 2026-11-10T06:41:00+09:00
- lastSeenAt: 2026-11-10T06:41:00+09:00

### GAP-Audio-GetSeFrequencyRatio-001

- status: open
- tier: P3
- score: 470
- primaryUseCase: UC-Play
- scenarioId: PLY-Audio-001
- frequency: low
- frequencyBasis: ピッチ照会 API が未配線
- scope: UC-Play|Audio|GetSeFrequencyRatio|Unwired|Project/Codes/Include/System/PlaySe.h:151
- primary: `GetSeFrequencyRatio`
- summary: 【Play】GetSeFrequencyRatio が未使用
- scenarioText: プレイ中に BGM／SE が途切れず再生され、シーン遷移後も期待どおり鳴り続ける。
- evidence:
  - GetSeFrequencyRatio は定義のみ（151-153 行）
  - SetBgmLoopPitch GAP（163 行）と非対称
  - 演出後にピッチを元に戻すロジック無し
- suggestedUse: プレイ: スロー演出後の BGM ピッチ復帰
- firstSeenAt: 2026-11-11T00:11:00+09:00
- lastSeenAt: 2026-11-11T00:11:00+09:00

### GAP-Audio-SetSeFrequencyRatio-001

- status: open
- tier: P3
- score: 470
- primaryUseCase: UC-Play
- scenarioId: PLY-Audio-001
- frequency: low
- frequencyBasis: SE ピッチ API が未配線
- scope: UC-Play|Audio|SetSeFrequencyRatio|Unwired|Project/Codes/Include/System/PlaySe.h:147
- primary: `SetSeFrequencyRatio`
- summary: 【Play】SetSeFrequencyRatio がゲームから未使用
- scenarioText: プレイ中に BGM／SE が途切れず再生され、シーン遷移後も期待どおり鳴り続ける。
- evidence:
  - SetSeFrequencyRatio は SetHandleFrequencyRatio 委譲（147-148 行）
  - rg SetSeFrequencyRatio は PlaySe.h 定義のみ
  - SetBgmLoopPitch-001 と対で一時変化 SE が演出に無い
- suggestedUse: プレイ: スロー演出時の SE ピッチ連動
- firstSeenAt: 2026-11-11T01:41:00+09:00
- lastSeenAt: 2026-11-11T01:41:00+09:00

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

- scanId: SCAN-20261111-021016 | at: 2026-11-11T02:11:03.810105+09:00 | scenario: PLY-Audio-001 | new: 3 | updated: 0
- scanId: SCAN-20261111-022018 | at: 2026-11-11T02:21:03.662472+09:00 | scenario: AUT-AssetLoop-001 | new: 3 | updated: 0
- scanId: SCAN-20261111-023031 | at: 2026-11-11T02:31:18.921192+09:00 | scenario: DBG-FeatureScene-001 | new: 3 | updated: 0
- scanId: SCAN-20261111-024014 | at: 2026-11-11T02:40:47.012554+09:00 | scenario: PLY-Audio-001 | new: 3 | updated: 0
- scanId: SCAN-20261111-025013 | at: 2026-11-11T02:51:07.177239+09:00 | scenario: AUT-AssetLoop-001 | new: 3 | updated: 0
- scanId: SCAN-20261111-030016 | at: 2026-11-11T03:01:32.865102+09:00 | scenario: DBG-FeatureScene-001 | new: 3 | updated: 0
- scanId: SCAN-20261111-031017 | at: 2026-11-11T03:11:06.426582+09:00 | scenario: PLY-Audio-001 | new: 3 | updated: 0
- scanId: SCAN-20261111-032014 | at: 2026-11-11T03:21:02.300717+09:00 | scenario: AUT-AssetLoop-001 | new: 3 | updated: 0
- scanId: SCAN-20261111-033012 | at: 2026-11-11T03:31:23.423901+09:00 | scenario: DBG-FeatureScene-001 | new: 3 | updated: 0
- scanId: SCAN-20261111-034021 | at: 2026-11-11T03:40:47.570879+09:00 | scenario: PLY-Audio-001 | new: 3 | updated: 0
- scanId: SCAN-20261111-035014 | at: 2026-11-11T03:50:30.395021+09:00 | scenario: AUT-AssetLoop-001 | new: 3 | updated: 0
- scanId: SCAN-20261111-040020 | at: 2026-11-11T04:00:54.388539+09:00 | scenario: DBG-FeatureScene-001 | new: 3 | updated: 0
- scanId: SCAN-20261111-041012 | at: 2026-11-11T04:10:35.991951+09:00 | scenario: PLY-Audio-001 | new: 3 | updated: 0
- scanId: SCAN-20261111-042015 | at: 2026-11-11T04:21:02.733755+09:00 | scenario: AUT-AssetLoop-001 | new: 3 | updated: 0
- scanId: SCAN-20261111-043006 | at: 2026-11-11T04:30:30.823712+09:00 | scenario: DBG-FeatureScene-001 | new: 3 | updated: 0

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
