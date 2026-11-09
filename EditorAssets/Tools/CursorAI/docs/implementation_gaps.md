# 実装ギャップ一覧（用途別・正本）

人間が読む正本。機械による重複判定・マージの正本は **`implementation_gaps_index.json`**。

<!-- gaps-schema: 2 -->

## メタ

- **schemaVersion:** 2
- **lastFullScanAt:** （未実施）
- **lastIncrementalScanAt:** 2026-11-10T05:20:54.212485+09:00
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

- scanId: SCAN-20261110-030010 | at: 2026-11-10T03:00:33.132938+09:00 | scenario: AUT-AssetLoop-001 | new: 3 | updated: 0
- scanId: SCAN-20261110-031009 | at: 2026-11-10T03:10:29.006133+09:00 | scenario: DBG-FeatureScene-001 | new: 3 | updated: 0
- scanId: SCAN-20261110-032016 | at: 2026-11-10T03:20:51.711983+09:00 | scenario: PLY-Camera-001 | new: 3 | updated: 0
- scanId: SCAN-20261110-033008 | at: 2026-11-10T03:30:33.093958+09:00 | scenario: AUT-Component-001 | new: 3 | updated: 0
- scanId: SCAN-20261110-034006 | at: 2026-11-10T03:40:52.914629+09:00 | scenario: PLY-Movie-001 | new: 3 | updated: 0
- scanId: SCAN-20261110-035010 | at: 2026-11-10T03:50:48.091396+09:00 | scenario: AUT-EditorPlace-001 | new: 3 | updated: 0
- scanId: SCAN-20261110-040022 | at: 2026-11-10T04:01:02.891193+09:00 | scenario: PLY-SceneLoad-001 | new: 3 | updated: 0
- scanId: SCAN-20261110-041018 | at: 2026-11-10T04:10:44.427042+09:00 | scenario: AUT-Guidance-001 | new: 3 | updated: 0
- scanId: SCAN-20261110-042012 | at: 2026-11-10T04:21:03.907927+09:00 | scenario: PLY-Visual-001 | new: 3 | updated: 0
- scanId: SCAN-20261110-043016 | at: 2026-11-10T04:30:49.405497+09:00 | scenario: AUT-VisualTune-001 | new: 3 | updated: 0
- scanId: SCAN-20261110-044018 | at: 2026-11-10T04:40:56.930223+09:00 | scenario: DBG-FeatureScene-001 | new: 3 | updated: 0
- scanId: SCAN-20261110-045017 | at: 2026-11-10T04:50:42.063624+09:00 | scenario: PLY-Audio-001 | new: 3 | updated: 0
- scanId: SCAN-20261110-050024 | at: 2026-11-10T05:01:03.395407+09:00 | scenario: AUT-AssetLoop-001 | new: 3 | updated: 0
- scanId: SCAN-20261110-051012 | at: 2026-11-10T05:11:23.847271+09:00 | scenario: PLY-Camera-001 | new: 3 | updated: 0
- scanId: SCAN-20261110-052013 | at: 2026-11-10T05:20:54.212485+09:00 | scenario: AUT-Component-001 | new: 3 | updated: 0

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
