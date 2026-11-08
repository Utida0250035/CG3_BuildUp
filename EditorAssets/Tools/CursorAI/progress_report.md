# 半自律稼働 progress report

**最新サマリー:** 2026-11-08 — ループ停止・未マージ tick を `f6c92c2` へロールバック

**更新日時:** 2026-11-08（BGM 一時停止・再開完了）  
**ブランチ:** `feature/SemiAutoDevelopFlow`  
**稼働指定:** 「Next」1 サブタスク完了で一旦停止

## 1. 現在の進捗と完了したこと

- **Audio:** シーン BGM の一時停止・再開（`AudioManager::PauseSceneBgm` / `ResumeSceneBgm`、`PlaySe.h` の `PauseBgmLoop` / `ResumeBgmLoop` / `IsBgmLoopPaused`）
- **Audio:** 一時停止中は `Update()` でストリーミング補填をスキップ。`IsPlaying` は一時停止中もスロット占有として true（`MaintainSceneBgmIfNeeded` が再発火しない）
- **roadmap.md:** 「シーン BGM の一時停止・再開」を `[x]`
- **buildDevelopment.cmd:** `%REPO_ROOT%\Project\...` にバックスラッシュ修正
- **Development | x64** ビルド成功

## 2. 発生した課題または設計上の選択肢

| 項目 | 内容 |
|------|------|
| シーン側未配線 | API のみ追加。ポーズメニュー等から呼ぶのは別タスク |
| buildDevelopment.cmd | 従来パス結合バグで失敗していたため本タスクで修正 |
| 作業ツリー | 未コミット。コミットは未許可 |
| Git 運用 | ユーザー指示: develop マージ／develop push／作業ブランチ削除はリスク例外時以外確認不要（`cursorInstructions.md` §5 反映済み） |

## 3. 次に実行すべきステップ

1. **実装:** roadmap 次の `- [ ]` → **Audio: 再生地点の記録／指定**（または BGM ピッチ等ステータス）
2. 必要なら `SceneGame` 等で `PauseBgmLoop` の呼び出し試験
3. コミット許可時: 本ブランチを整理して push

**状態:** 指示待ち（区切り停止）

## 実装済みと判断した項目（コード確認・即時 [x]）

| roadmap の文言 | 根拠 | 判定 |
|----------------|------|------|
| Bgmストリーミング | `Audio.cpp` `SetupStreaming` / `PlaySceneBgm` | 万全 |
| **シーン BGM 一時停止・再開** | `Audio.cpp` Pause/Resume、`PlaySe.h` | 万全（API） |
| Component ImGui 追加削除 | `EntityEditorUi.cpp` | 万全 |
| Json入出力 | `EditorSceneDocument` / `EntityEditor` | 万全 |
| CmpHitSphere 球 | `CmpHitSphere` | 万全 |
| Quaternion Slerp | `Quaternion::Slerp` | 万全 |

## 保留・着手見送り（§6.3）

（前回区切りと同様。変更なし）

| roadmap の文言 | 種別 | 理由 | 再開条件 |
|----------------|------|------|----------|
| 純粋 ECS | 悪影響 | スコープ外 | 方針変更時 |
| FNV-1a registry | 依存 | json 型選択の設計未確定 | 仕様決定後 |
| Collider AABB・Update 順 | 悪影響 | 判定パイプライン設計 | 設計合意後 |
| factory コンポーネント | 依存 | プレハブ／json | プレハブ2項完了後 |
| Lua／動的スクリプト全体 | 悪影響 | API 未安定 | エンティティ API 安定後 |
| ステージギミック／統合エディタ | 悪影響 | 範囲巨大 | json ワークフロー後 |
| 別スレッド読み込み | 悪影響 | 同期・ライフサイクル | ローダ設計後 |
| Organize 名前空間 | 悪影響 | 全庫一括は別計画 | スコープ明示後 |

---

## セッション 2026-11-08 18:15（/loop 初回・蓄積形式開始）

**ブランチ:** `feature/SemiAutoDevelopFlow`  
**トリガー:** `/loop 1h30m` — roadmap に沿った開発

### 1. 完了したこと

- **運用:** `cursorInstructions.md` §3 に progress_report **追記（上書き禁止）** を明記
- **ループ:** 90 分間隔 `AGENT_LOOP_TICK_roadmap-dev`（旧ループは置換）
- **Audio:** `GetBgmLoopPlaybackSeconds` / `SetBgmLoopPlaybackSeconds`（`PlaySe.h`、ストリーミング＋短尺ループ）
- **roadmap:** 再生地点行を実装内容に合わせて更新
- **Development | x64** ビルド成功

### 2. 課題・メモ

| 項目 | 内容 |
|------|------|
| ストリーミング位置 | 取得は `startTime100ns`＋`SamplesPlayed` 近似（ループ巻き戻し時はジャンプしうる） |
| ランタイム | 未実施 |

### 3. 次ステップ

1. roadmap 次 `- [ ]` → **ピッチ等ステータス API（BGM／SE）**
2. 各 tick: roadmap 先頭未完了 1 件 → ビルド → 本ファイル末尾にセクション追記

**状態:** レビュー可（次 tick は約 90 分後）

### 実装済みと判断した項目（§6.2）

| roadmap の文言 | 根拠 | 判定 |
|----------------|------|------|
| 再生地点の記録／指定 | `Audio.cpp` Get/Set、`PlaySe.h` | 万全（API） |

### 保留（§6.3）

変更なし（前セッション表を参照）

---

## セッション 2026-11-08 18:22（ループ運用ルール更新）

**ブランチ:** `feature/SemiAutoDevelopFlow`

### 1. 完了したこと

- `cursorInstructions.md` §3・**§7.1** — ループ中の報告追記、200kB 超過でループ自動停止
- `loopReportGuard.ps1` / `loopRoadmapDev.ps1` 追加
- `README.md`・`semi-autonomous-agent.mdc` を蓄積方針に合わせて更新

### 2. 課題・メモ

| 項目 | 内容 |
|------|------|
| ループ | ユーザー指示により手動停止済み。次回は `loopRoadmapDev.ps1` から起動 |

### 3. 次ステップ

1. roadmap 次タスク（例: ピッチ API）
2. `/loop` 再開時は `loopRoadmapDev.ps1 -IntervalMinutes 90`

**状態:** 指示待ち

---

## セッション 2026-11-08 18:26（/loop 1m 初回 tick）

**ブランチ:** `feature/SemiAutoDevelopFlow`  
**ループ:** `loopRoadmapDev.ps1 -IntervalMinutes 1`（PID 31068、200kB ガード付き）

### 1. 完了したこと

- **Audio:** ピッチ・音量 API（`SetHandleFrequencyRatio` / `SetHandleVolume`、`SetBgmLoopPitch` / `SetSeVolume`）
- **roadmap:** ピッチ等ステータスを `[x]`
- **Development | x64** ビルド成功

### 2. 課題・メモ

| 項目 | 内容 |
|------|------|
| ランタイム | ピッチ変更の耳検証は未実施 |
| 次 tick | 約 1 分後（`AGENT_LOOP_TICK_roadmap-dev`） |

### 3. 次ステップ

1. roadmap **System** 先頭の未完了から 1 件
2. tick ごと本ファイル末尾にセクション追記

**状態:** ループ稼働中

---

## セッション 2026-11-08 18:28（/loop tick・プレハブ json）

**ブランチ:** `feature/SemiAutoDevelopFlow`

### 1. 完了したこと

- **プレハブ:** `SaveSelectedInstanceToPrefabPath`、UI「選択エンティティをプレハブ JSON へ出力」、`MakeUniquePrefabFilePath`
- **roadmap:** プレハブ json 出力・読み込みを `[x]`（既存 `AddInstanceFromPrefabPath` と整合）
- **Development | x64** ビルド成功

### 2. 課題・メモ

| 項目 | 内容 |
|------|------|
| エディタ | 出力先は `EditorAssets/Entities`（相対パス） |

### 3. 次ステップ

1. roadmap: コンポーネント型の文字列記録 等
2. コミットは明示指示まで保留

**状態:** tick 処理後、ユーザー指示でループ停止待ち

---

## セッション 2026-11-08 18:29（ループ手動停止・区切り）

**理由:** ユーザー「キリのいいところでループ停止」（プレハブ区切り＋ビルド成功後）

- **停止:** `loopRoadmapDev.ps1` プロセス（PID 31068）終了
- **累積:** 本ファイルの過去セクションは削除していない

**状態:** 指示待ち（半自律・手動開発どちらも可）

---

## セッション 2026-11-08 18:38（ループ運用・ビルド失敗／Git 自動化）

**ブランチ:** `feature/SemiAutoDevelopFlow`

### 1. 完了したこと

- **方針:** 同一タスクでビルド **3 回連続失敗** → `lastGoodCommit` へロールバック → 依存のない別 `- [ ]` へ（ループ継続）
- **報告:** 中断時は `progress_report` に **MSBuild のファイルパス・行番号付き** ログ抜粋を必須（`loopRegisterBuildFailure.ps1`）
- **Git（ループ中）:** 成功 tick は `loopMergeTickToDevelop.ps1` で **develop 自動マージ・push**（ユーザー確認不要）
- **ドキュメント:** `cursorInstructions.md` §7.1.1〜7.1.3、`README.md`、半自律ルール更新
- **補助:** `loop_state.json`, `loopMergeTickToDevelop.ps1`, `loopRegisterBuildFailure.ps1`, `loopRollbackToLastGood.ps1`, `loopRecordLastGood.ps1`

### 2. 課題・メモ

| 項目 | 内容 |
|------|------|
| `lastGoodCommit` | 初回ループ前に 1 回成功マージするか、既存の良好コミット SHA を `loop_state.json` に入れるとロールバックが明確になる |
| 未コミット作業 | 現状ツリーはまだ手動コミット前。次回 `/loop` 最初の成功で `loopMergeTickToDevelop` が baseline になる |

### 3. 次ステップ

1. コミット許可があれば現状差分を 1 回マージして `lastGoodCommit` を実体化
2. `/loop` 再開時は新デフォルト `TickPrompt` を使用

**状態:** 指示待ち

---

## セッション 2026-11-08 19:25（/loop 20m 開始）

**ブランチ:** `feature/SemiAutoDevelopFlow`  
**ループ:** `loopRoadmapDev.ps1 -IntervalMinutes 20`（PID 25492）

### 1. 完了したこと

- **初回 tick（即時）:** ビルド成功、§6.2 で「コンポーネント型を文字列／選択式で記録」を `[x]`（既存 `typeName`＋Combo）
- **Git baseline:** コミット `52c2481` → **develop へ fast-forward マージ・push 済み**
- **`loop_state.json`:** `lastGoodCommit=52c2481`
- **`loopMergeTickToDevelop.ps1`:** PowerShell `$Args` 衝突を `GitArgs` に修正（次 tick からスクリプト経由マージ可）

### 2. 課題・メモ

| 項目 | 内容 |
|------|------|
| 未コミット | 本セクション追記・roadmap `[x]` 1 行・ps1 修正（次 tick でマージ） |
| リモート | CG3_BuildUp 移転通知（push は成功） |

### 3. 次ステップ

1. roadmap 次: `AddComponent` 関数化 等
2. 約 **20 分後** に次 `AGENT_LOOP_TICK_roadmap-dev`

**状態:** ループ稼働中

---

## セッション 2026-11-08 19:45（/loop 20m tick・SortComponents）

**トリガー:** `AGENT_LOOP_TICK_roadmap-dev`（1 回目の周期 tick）

### 1. 完了したこと

- **Entity:** `SortComponents` / `SetUpdCmpPriority` / `SetDrawCmpPriority`、`AttachUpdComponent`・`AttachDrawComponent`
- **エディタ:** `AppendKnownComponentSlot`
- **roadmap:** SortComponents 節 2 項を `[x]`
- **Git:** `c149e9c` → develop マージ・push（`lastGoodCommit` 更新）

### 2. 課題・メモ

| 項目 | 内容 |
|------|------|
| エディタ UI | 実行順の列表示は未実装（別項） |

### 3. 次ステップ

1. roadmap: Entity 配列化／実行順 UI
2. 次 tick 約 20 分後

**状態:** ループ稼働中

---

## セッション 2026-11-08 20:05（/loop 20m tick・executionOrder）

**トリガー:** `AGENT_LOOP_TICK_roadmap-dev`（2 回目の周期 tick）

### 1. 完了したこと

- **`ComponentSlot::executionOrder`**（JSON）とインスペクタ「実行順 | 型」、`InputInt` による priority 編集
- **プレビュー:** `ApplySlotExecutionOrder` 後 `SortComponents`（`EditorSceneRuntime.cpp`）
- **ビルド:** 初回は `SortComponents`／const 周りで失敗 → 修正後 **Development | x64 成功**
- **roadmap:** Entity 配列化関連 3 項を `[x]`
- **Git:** `10431b0` → **develop マージ・push 済み**（作業ブランチ維持）

### 2. 課題・メモ

| 項目 | 内容 |
|------|------|
| `loop_state.json` | コミット内 `lastGoodCommit` が `c149e9c` のまま。作業ツリーは `10431b0` に更新済み（次マージで同期） |
| D&D | priority のドラッグ＆ドロップは未 |

### 3. 次ステップ

1. roadmap 次: 「方針に沿ったコンポーネント分割」または Loader D&D 等
2. 次 tick 約 20 分後

**状態:** ループ稼働中

---

## セッション 2026-11-08 20:25（/loop 20m tick・ファイルドロップ）

**トリガー:** `AGENT_LOOP_TICK_roadmap-dev`（3 回目の周期 tick）

### 1. 完了したこと

- **`EditorImportedAsset`:** SDL ドロップイベント用キュー、`EditorAssets/Imported` へコピー
- **エディタ:** プロジェクトパネルにドロップ案内・直近パス表示
- **roadmap:** Loader ドロップ 1 項、任意フォルダ D&D の Assets コピー 1 項を `[x]`
- **ビルド:** Development | x64 成功

### 2. 課題・メモ

| 項目 | 内容 |
|------|------|
| ランタイム | エディタ未起動でのドロップ動作は未手動確認 |
| 続き | ModelStorage への即時ロード・リソースコンポーネント連携は別項 |

### 3. 次ステップ

1. リソースコンポーネントのパス記録・読み込み（roadmap 残）
2. 次 tick 約 20 分後

## 実装済みと判断した項目（コード確認・即時 [x]）

- Loader: ドロップでパス保持・Imported コピー（`AtrumEngine` + `EditorImportedAsset`）
- System: GUI 専用で Assets（Imported）へコピー

## 保留・着手見送り（§6.3）

- リソースコンポーネントがパス記録・読み込み — 本 tick スコープ外

**状態:** ループ稼働中

---

## セッション 2026-11-08 20:45（/loop 20m tick・CmpEditorAssetPath）

**トリガー:** `AGENT_LOOP_TICK_roadmap-dev`（4 回目の周期 tick）

### 1. 完了したこと

- **`CmpEditorAssetPath`（Data）:** JSON `assetPath`、インスペクタ編集、直近ドロップ適用ボタン
- **プレビュー:** obj パスから `ModelStorage` 経由で `CmpMesh` 描画
- **roadmap:** リソースコンポーネント項目を `[x]`
- **ビルド:** Development | x64 成功

### 2. 課題・メモ

| 項目 | 内容 |
|------|------|
| mtl | 同名 mtl が無い Imported obj はロード失敗しうる |
| png | テクスチャ読み込みは未（別タスク） |

### 3. 次ステップ

1. コンポーネント分割継続または Loader 変換ツール
2. 次 tick 約 20 分後

## 実装済みと判断した項目（コード確認・即時 [x]）

- リソースコンポーネントのパス記録・obj プレビュー読み込み

## 保留・着手見送り（§6.3）

- png 等テクスチャのプレビュー連携 — スコープ外

**状態:** ループ稼働中

---

## セッション 2026-11-08 21:05（/loop 20m tick・png プレビュー）

**トリガー:** `AGENT_LOOP_TICK_roadmap-dev`（5 回目の周期 tick）

### 1. 完了したこと

- **`CmpEditorTextureBillboard`:** png／jpg をビルボード表示
- **`CmpEditorAssetPath`:** `loadImageAsBillboard`・板サイズを JSON 化
- **ビルド:** Development | x64 成功

### 2. 課題・メモ

| 項目 | 内容 |
|------|------|
| 手動確認 | エディタ上のドロップ→プレビューは未実施 |

### 3. 次ステップ

1. Unity 風方針メモまたは BinaryConverter 等
2. 次 tick 約 20 分後

## 実装済みと判断した項目（コード確認・即時 [x]）

- png 画像のエディタプレビュー読み込み（前 tick 保留分）

## 保留・着手見送り（§6.3）

- なし

**状態:** ループ稼働中

---

## セッション 2026-11-08 21:24（ループ停止・ユーザー指示）

### 1. 完了したこと

- **`/loop 20m` 停止:** `loopRoadmapDev.ps1` PID 27144 を終了（tick 実行中のビルドなし）
- **残骸停止:** 旧 `IntervalMinutes 1` のループ PID 32364 も終了

### 2. 課題・メモ

| 項目 | 内容 |
|------|------|
| 最後の成功 tick | `91bb8df`（png ビルボードプレビュー） |
| `progress_report` 追記 | 本セクション（未コミット） |

### 3. 次ステップ

1. 次回は通常モードまたは `/loop` 再開時に roadmap 上位の `- [ ]` から着手

**状態:** 停止（指示待ち）

---

## セッション 2026-11-08 22:05（/loop 10m 開始・Unity 方針メモ）

**ブランチ:** `chore/UnityStyleEntityGuidelines`（develop から新規）

### 1. 完了したこと

- **`unity_style_entity_guidelines.md`** 追加、roadmap 該当行を `[x]`
- **`startTaskBranchFromDevelop.ps1`** 出力取得と develop 上の未コミット引き継ぎを修正
- **ビルド:** Development | x64 成功（`buildDevelopment.cmd` は MSBuild 解決失敗のため MSBuild 直叩き）

### 2. 課題・メモ

| 項目 | 内容 |
|------|------|
| buildDevelopment.cmd | 一部環境で MSBuild 自動解決失敗（要調査） |

### 3. 次ステップ

1. 次 `- [ ]`: 方針に沿ったコンポーネント分割 → `chore/ComponentSplitGuideline`
2. `/loop 10m` 稼働中

## 実装済みと判断した項目（コード確認・即時 [x]）

- Unity っぽいオブジェクト指向の整理（方針メモ）

## 保留・着手見送り（§6.3）

- なし

## 用途不明で確認待ち（§6.4）

- なし

## 推測した用途（§6.4）

- **Unity 風方針メモ**
  - 用途: エディタ／ランタイムで共通の用語と責務を制作者向けに固定する
  - 使用場面: 制作（エディタ）、将来のランタイムシーンロード設計
  - 仕様: ドキュメントのみ。コード変更なし

**状態:** 停止（のちロールバック）

---

## セッション 2026-11-08 22:21（ループ停止・ロールバック）

### 1. 完了したこと

- **`/loop 15m`:** 起動前に中断（マージ未完了）
- **ロールバック:** `loopRollbackToLastGood.ps1` → **`f6c92c2`**（Unity 方針メモまで）
- **破棄した作業:** コンポーネント分割ガイド／pre-commit サイレント化／roadmap `[x]` 等（未コミット分）
- **ブランチ:** `develop` に切替、`chore/ComponentSplitGuideline` ローカル削除

### 2. 課題・メモ

| 項目 | 内容 |
|------|------|
| リモート | `origin/chore/ComponentSplitGuideline` が残っている場合は手動削除可 |

### 3. 次ステップ

1. 再開時は roadmap 先頭 `- [ ]` から `startTaskBranchFromDevelop.ps1` でブランチ作成

**状態:** 停止（指示待ち）
