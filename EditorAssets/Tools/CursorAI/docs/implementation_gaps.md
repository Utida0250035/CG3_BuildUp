# 実装ギャップ一覧（用途別・正本）

人間が読む正本。機械による重複判定・マージの正本は **`implementation_gaps_index.json`**。

<!-- gaps-schema: 2 -->

## メタ

- **schemaVersion:** 2
- **lastFullScanAt:** （未実施）
- **lastIncrementalScanAt:** 2026-11-09T23:40:39.226713+09:00
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

#### UC-Play

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


---

## Archive

なし

---

## スキャン履歴（直近）

- scanId: SCAN-20261109-232947 | at: 2026-11-09T23:30:26.570595+09:00 | scenario: PLY-Audio-001 | new: 3 | updated: 0
- scanId: SCAN-20261109-234011 | at: 2026-11-09T23:40:39.226713+09:00 | scenario: AUT-AssetLoop-001 | new: 3 | updated: 0

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
