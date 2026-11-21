# 半自律稼働 progress report

**最新サマリー:** 2026-11-18 — ストリーム PCM は音源のサンプルレート。次は `SetupStreaming が 44.1kHz 固定`

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

---

## セッション 2026-11-08 22:45（/loop 15m Cloud・コンポーネント分割ガイド）

**ブランチ:** `cursor/cloud-agent-1794144340853-5xzcq`

### 1. 完了したこと

- **`component_split_guidelines.md`**、roadmap 該当行を `[x]`
- **ビルド:** Cloud Linux のため `buildDevelopment.cmd` は未実施（Win 専用）

### 3. 次ステップ

1. 次 `- [ ]`: BinaryConverter

## 実装済みと判断した項目（コード確認・即時 [x]）

- 方針に沿ったコンポーネント分割（ガイド第1版）

## 推測した用途（§6.4）

- **コンポーネント分割ガイド:** 制作（エディタでコンポーネント追加・分割時の判断基準）

**状態:** ループ稼働中（subscription timer 15m）

---

## セッション 2026-11-08 13:45（/loop 15m Cloud・BinaryConverter）

**ブランチ:** `cursor/cloud-agent-1794144340853-5xzcq`（着手行: Loader / BinaryConverter）

### 1. 完了したこと

- **`.amesh` v1 仕様** … `atr_mesh_binary_format.md`
- **変換ツール** … `EditorAssets/Tools/BinaryConverter/obj_to_amesh.py`（OBJ 三角形 → バイナリ）
- **スモークテスト** … Python で 3 頂点の `.amesh` 生成を確認
- **ビルド:** 未実施（ツールは Python・ランタイムロードは未接続）

### 2. 課題・メモ

| 項目 | 内容 |
|------|------|
| ランタイム | `ModelStorage` で `.amesh` 読み込みは別タスク |

### 3. 次ステップ

1. 次 `- [ ]`: DdsConverter

## 実装済みと判断した項目（コード確認・即時 [x]）

- BinaryConverter（エンジン外 Python ツール + 形式仕様）

## 保留・着手見送り（§6.3）

- `ModelStorage` への `.amesh` ローダー … 本 tick は変換ツールのみ（依存: 形式 v1 確定済み）

## 用途不明で確認待ち（§6.4）

- なし

## 推測した用途（§6.4）

- **BinaryConverter:** ビルド・配布前に OBJ を固定レイアウトバイナリへ変換し、読み込みコストとフォーマットを安定化（制作パイプライン）
- **`.amesh`:** 将来ランタイムの高速メッシュ読み込み用（現状はオフライン変換のみ）

**状態:** ループ稼働中（subscription timer 15m）

---

## セッション 2026-11-08 14:00（/loop 15m Cloud・DdsConverter）

**ブランチ:** `cursor/cloud-agent-1794144340853-5xzcq`（着手行: Loader / DdsConverter）

### 1. 完了したこと

- **`image_to_dds.py`** … PNG/JPEG → レガシー RGBA8 DDS（任意ミップチェーン）
- **README / requirements.txt**（Pillow）
- **スモークテスト** … 4×4 画像から DDS 生成
- **ビルド:** 未実施

### 2. 課題・メモ

| 項目 | 内容 |
|------|------|
| ランタイム | `TextureStorage` は現状 WIC のみ。DDS 直読みは別タスク |
| BC 圧縮 | 本ツールは非圧縮のみ（texconv 案内を README に記載） |

### 3. 次ステップ

1. 次 `- [ ]`: Camera「設計＋調整ツール化」など roadmap 続行

## 実装済みと判断した項目（コード確認・即時 [x]）

- DdsConverter

## 保留・着手見送り（§6.3）

- TextureStorage への DDS ロードパス追加

## 用途不明で確認待ち（§6.4）

- なし

## 推測した用途（§6.4）

- **DdsConverter:** ビルド前のアセット DDS 化・ミップ同梱（配布サイズとロード経路の整理）
- **制作:** エンジン外 Python で Linux/CI でも実行可能

**状態:** ループ稼働中（subscription timer 15m）

---

## セッション 2026-11-08 14:15（/loop 15m Cloud・カメラ設計＋調整ツール）

**ブランチ:** `cursor/cloud-agent-1794144340853-5xzcq`（着手行: Camera / 設計＋調整ツール化）

### 1. 完了したこと

- **`odyssey_style_stage_camera.md`** … 方針・既存 API 対応表
- **`EditorAssets/Camera/stage_view_camera_tuning.json`** … 現行 `StageViewCamera` 定数と同等の調整セット
- **`StageViewCameraTuning/preview_poses.py`** … battle / focus の eye・pivot CLI プレビュー
- **ビルド:** 未実施（ランタイム JSON ホットリードは未接続）

### 3. 次ステップ

1. roadmap 次行: イージング式のカスタム
2. DEVELOPMENT ImGui スライダー（任意）

## 実装済みと判断した項目（コード確認・即時 [x]）

- オデッセイ的カメラ「設計＋調整ツール化」（ドキュメント + JSON + プレビュー CLI）

## 保留・着手見送り（§6.3）

- エンジン内 JSON 読み込み・ImGui 調整パネル

## 用途不明で確認待ち（§6.4）

- なし

## 推測した用途（§6.4）

- **調整ツール:** レベル／カメラ担当が数値を試す制作フロー（デバッグ用 DebugCamera と本番体験の分離）
- **ランタイム:** 現状は既存 C++ 定数のまま（JSON は調整のたたき台）

**状態:** ループ稼働中（subscription timer 15m）

---

## セッション 2026-11-08 14:30（/loop 15m Cloud・カメライージング）

**ブランチ:** `cursor/cloud-agent-1794144340853-5xzcq`

### 1. 完了したこと

- **`StageViewCameraEasing.h`** … PullBackReveal / FocusPush / Legacy
- **`CameraPoseBlendController`** … ease 種別引数（既定 PullBackReveal）
- **`CameraFocusPushController`** … FocusPush 曲線
- **メモ / `plot_easing.py`**、tuning.json `easing` 節
- **ビルド:** Cloud 未実施（C++ 変更あり）

### 3. 次ステップ

1. roadmap: ユーザーに操作を丸投げしない

## 実装済みと判断した項目（コード確認・即時 [x]）

- イージング式のカスタム

## 推測した用途（§6.4）

- **PullBackReveal:** ボスイントロ後プレイヤーが状況を把握してから戦闘画へ戻す演出
- **FocusPush:** 撃破・注目演出で素早く寄せつつ酔いにくい終端

**状態:** ループ稼働中（subscription timer 15m）

---

## セッション 2026-11-08 14:45（/loop 15m Cloud・カメラ丸投げ禁止）

**ブランチ:** `cursor/cloud-agent-1794144340853-5xzcq`

### 1. 完了したこと

- **`player_friendly_camera_policy.md`** … 原則・ガード表・シーン一覧・チェックリスト
- **`verify_camera_policy.py`** … Game/BuildUp/Title/Result=locked、Editor系=unlocked
- **`Scene.h` / `GameOrder.h`** コメント追記
- **roadmap** Camera 節 3 行目を `[x]`
- **ビルド:** 未実施

## 実装済みと判断した項目（コード確認・即時 [x]）

- ユーザーに操作を丸投げしない（既存 `IsStageViewCameraLocked` 体系の文書化＋静的検証）

## 推測した用途（§6.4）

- **ランタイム:** 横 STG 本編で視点操作を求めず、演出カメラに集中させる UX
- **制作:** 新シーン追加時の CI/手元チェック（verify スクリプト）

**状態:** ループ稼働中（subscription timer 15m）

---

## セッション 2026-11-08 15:00（/loop 15m Cloud・ムービーカメラ）

**ブランチ:** `cursor/cloud-agent-1794144340853-5xzcq`

### 1. 完了したこと

- **`MovieCameraController`** キーフレーム再生・ ease 区間補間
- **`LoadMovieCameraTrackFromJson`**、`sample_movie_track.json`
- **vcxproj 登録**、メモ `movie_camera_controller.md`
- **ビルド:** Cloud 未実施

### 3. 次ステップ

1. ムービー用シーン

## 実装済みと判断した項目（コード確認・即時 [x]）

- ムービー用カメラコントローラ

## 推測した用途（§6.4）

- **ランタイム:** カットシーン再生（シーン接続は次タスク）
- **制作:** JSON トラック編集

**状態:** ループ稼働中（subscription timer 15m）

---

## セッション 2026-11-08 15:15（/loop 15m Cloud・SceneMovie）

**ブランチ:** `cursor/cloud-agent-1794144340853-5xzcq`

### 1. 完了したこと

- **`SceneMovie` + `SceneMovieRegistry`** … JSON 再生・終了/ Esc で returnScene へフェード
- **メモ** `scene_movie.md`、`verify_camera_policy` に SceneMovie 追加
- **ビルド:** Cloud 未実施

## 実装済みと判断した項目（コード確認・即時 [x]）

- ムービー用シーン

## 推測した用途（§6.4）

- **ランタイム:** イベントカットの差し込み（`Launch` + `CommandChangeScene`）
- **デバッグ:** サンプルトラックでカメラのみ確認

**状態:** ループ稼働中（subscription timer 15m）

---

## セッション 2026-11-08 15:30（/loop 15m Cloud・粒々透過）

**ブランチ:** `cursor/cloud-agent-1794144340853-5xzcq`

### 1. 完了したこと

- **`BlendMode::DITHERED_ALPHA`** + Bayer clip PS + PSO
- **`CmpMesh`** 半透明時にディザ透過を使用
- **メモ** `object_dithered_transparency.md`
- **ビルド:** Cloud 未実施（シェーダ再コンパイル要）

## 実装済みと判断した項目（コード確認・即時 [x]）

- オブジェクトの粒々透過

## 推測した用途（§6.4）

- **ランタイム:** 半透明メッシュを深度バッファ付きで描く（ソートコスト回避）
- **制作:** エンティティの `CmpMesh` 色アルファで粒々表現

**状態:** ループ稼働中（subscription timer 15m）

---

## セッション 2026-11-08 15:45（/loop 15m Cloud・地形越しシルエット）

**ブランチ:** `cursor/cloud-agent-1794144340853-5xzcq`

### 1. 完了したこと

- **`BlendMode::OCCLUDED_SILHOUETTE`**（Depth Greater）
- **`PlayerTerrainSilhouette`** + `SceneGame` 描画順接続
- **メモ** `player_terrain_silhouette.md`
- **ビルド:** Cloud 未実施

## 実装済みと判断した項目（コード確認・即時 [x]）

- 地形越しシルエット表示

## 推測した用途（§6.4）

- **ランタイム:** 横 STG で地形・手前雲に隠れても自機の位置把握を支援

**状態:** ループ稼働中（subscription timer 15m）

---

## セッション 2026-11-08 16:00（/loop 15m Cloud・モーションブレンディング）

**ブランチ:** `cursor/cloud-agent-1794144340853-5xzcq`

### 1. 完了したこと

- **Motion クリップ JSON**、サンプル 2 本
- **`MotionBlender` / `CmpMotionBlend`**
- **メモ** `motion_blending.md`
- **ビルド:** Cloud 未実施

## 実装済みと判断した項目（コード確認・即時 [x]）

- モーションブレンディング

## 推測した用途（§6.4）

- **ランタイム:** ボス／プロップの簡易モーション合成（スケルタル未導入時の Transform 駆動）
- **制作:** JSON キーフレームでブレンド比を試す

**状態:** ループ稼働中（subscription timer 15m）

---

## セッション 2026-11-08 16:15（/loop 15m Cloud・モーション間補間）

**ブランチ:** `cursor/cloud-agent-1794144340853-5xzcq`

### 1. 完了したこと

- **`MotionCrossfade`** + **`CmpMotionCrossfade`**
- **メモ** `motion_interpolation.md`
- **ビルド:** Cloud 未実施

## 実装済みと判断した項目（コード確認・即時 [x]）

- モーション間の補間

## 推測した用途（§6.4）

- **ランタイム:** 連続モーションの切り替え（待機→行動）を途切れなく再生

**状態:** ループ稼働中（subscription timer 15m）

---

## セッション 2026-11-08 16:30（/loop 15m Cloud・テストプレイエリア）

**ブランチ:** `cursor/cloud-agent-1794144340853-5xzcq`

### 1. 完了したこと

- **シーンテンプレ** + combat_bounds 例
- **`scaffold_play_area.py`**、README、メモ
- **ビルド:** 不要（データ＋Python）

## 実装済みと判断した項目（コード確認・即時 [x]）

- デバッグ用テストプレイエリア・生成ツール／テンプレート

## 推測した用途（§6.4）

- **デバッグ:** 本編外サンドボックスで Entity 試験

**状態:** ループ稼働中（subscription timer 15m）

---

## セッション 2026-11-08 16:45（/loop 15m Cloud・ストリーミングメモリ ImGui）

**ブランチ:** `cursor/cloud-agent-1794144340853-5xzcq`

### 1. 完了したこと

- **`CollectStreamingMemoryDebug`**、ImGui ウィンドウ、エディタメニュー
- **メモ** `streaming_debug_imgui.md`
- **ビルド:** Cloud 未実施（DEVELOPMENT + USE_IMGUI）

## 実装済みと判断した項目（コード確認・即時 [x]）

- メモリ使用量表示

## 推測した用途（§6.4）

- **デバッグ・プロファイリング:** ストリーム BGM のバッファ使用量監視

**状態:** ループ稼働中（subscription timer 15m）

---

## セッション 2026-11-08 17:00（/loop 15m Cloud・ストリーミング進捗）

**ブランチ:** `cursor/cloud-agent-1794144340853-5xzcq`

### 1. 完了したこと

- **進捗フィールド**（再生秒・キュー・デコーダ backlog）と ImGui プログレスバー
- **roadmap** Debug ストリーミング2行とも `[x]`

## 実装済みと判断した項目（コード確認・即時 [x]）

- ストリーミング進捗表示

## 推測した用途（§6.4）

- **デバッグ:** ストリーム途切れ・バッファ饥饿の切り分け

**状態:** ループ稼働中（subscription timer 15m）

---

## セッション 2026-11-08 17:15（/loop 15m Cloud・リムライト）

**ブランチ:** `cursor/cloud-agent-1794144340853-5xzcq`

### 1. 完了したこと

- **リムライト**（Fresnel 縁取り）を平行光 CB + `Object3d.PS` に追加
- VS で `worldPosition`、エンジンでカメラ座標を毎フレーム同期
- **メモ** `rim_light.md`、`SceneGame` で弱リムの既定例
- **ビルド:** Cloud 未実施（シェーダ再コンパイル要）

## 実装済みと判断した項目（コード確認・即時 [x]）

- リムライト

## 推測した用途（§6.4）

- **ランタイム:** 3D キャラのシルエット強調で視認性向上
- **レベル制作:** ImGui で rim 調整（`DirectionalLightData::ImGui`）

**状態:** ループ稼働中（subscription timer 15m）

---

## セッション 2026-11-08 17:30（/loop 15m Cloud・VolumetricFog）

**ブランチ:** `cursor/cloud-agent-1794144340853-5xzcq`

### 1. 完了したこと

- **`VolumetricFog` / `VolumetricFogSettings`** — `Lighting` と `AtrumEngine` で共有
- **`Object3d.PS`** — 距離×高さフォグと太陽方向インスキャッター
- **メモ** `volumetric_fog.md`、`SceneGame` で弱フォグ既定
- **ビルド:** Cloud 未実施

## 実装済みと判断した項目（コード確認・即時 [x]）

- ボリュームフォグ／VolumetricFog（統合）

## 推測した用途（§6.4）

- **ランタイム:** 屋外ステージの奥行きと視認性の調整
- **デバッグ:** DEVELOPMENT で ImGui から密度・色をチューニング

**状態:** ループ稼働中（subscription timer 15m）

---

## セッション 2026-11-08 17:45（/loop 15m Cloud・ゴッドレイ）

**ブランチ:** `cursor/cloud-agent-1794144340853-5xzcq`

### 1. 完了したこと

- **`GodRays` / `GodRaySettings`** — `Lighting`・`AtrumEngine` 統合
- **`Object3d.PS`** — 太陽方向シャフト（`VolumetricFog` と `fogCoupling`）
- **メモ** `god_rays.md`、`SceneGame` 既定
- **ビルド:** Cloud 未実施

## 実装済みと判断した項目（コード確認・即時 [x]）

- ゴッドレイ

## 推測した用途（§6.4）

- **ランタイム:** 屋外シーンの陽光・演出強調
- **将来:** スクリーンスペースブラーへの差し替え用に設定 API を分離済み

**状態:** ループ稼働中（subscription timer 15m）

---

## セッション 2026-11-08 18:00（/loop 15m Cloud・シェード／シャドウ）

**ブランチ:** `cursor/cloud-agent-1794144340853-5xzcq`

### 1. 完了したこと

- **`DirectionalShadow`** — 接触シェード（PS）と平面影描画
- **`CmpMesh`** — `radius_` がある場合に平面影
- **メモ** `directional_shadow.md`、`SceneGame` 既定
- **ビルド:** Cloud 未実施

## 実装済みと判断した項目（コード確認・即時 [x]）

- シェード／シャドウ

## 推測した用途（§6.4）

- **ランタイム:** キャラ・障害物の足元の読み取りやすさ向上
- **将来:** `shadowMapIndex` 付きシャドウマップパスへ接続

**状態:** ループ稼働中（subscription timer 15m）

---

## セッション 2026-11-08 18:15（/loop 15m Cloud・vcxproj 穴チェック）

**ブランチ:** `cursor/cloud-agent-1794144340853-5xzcq`

### 1. 完了したこと

- **`verify_vcxproj_coverage.py`** — 未登録 `.cpp`、古い ClCompile、欠落 hlsl を検出
- **`vcxproj_coverage_allowlist.txt`**（Lua 2 ファイル）
- **GitHub Actions** — `BuildCoverageCheck.yml`、各ビルド workflow に前置ジョブ
- **メモ** `cloud_build_vcxproj_check.md`

## 実装済みと判断した項目（コード確認・即時 [x]）

- 除外パスによるビルド穴の自動確認

## 推測した用途（§6.4）

- **CI:** クラウドビルド前にリンク漏れを遮断
- **半自律エージェント:** Windows ビルドなし環境での vcxproj 登録漏れ検知

**状態:** ループ稼働中（subscription timer 15m）

---

## セッション 2026-11-08 18:30（/loop 15m Cloud・コードテンプレファイル）

**ブランチ:** `cursor/cloud-agent-1794144340853-5xzcq`

### 1. 完了したこと

- **`EditorAssets/Templates/Code/`** — ComponentDraw／Upd／Beh 用 `.template` と `manifest.json`
- **`Templates/README.md`**、`text_code_templates.md`
- **ビルド:** 不要（データのみ）

## 実装済みと判断した項目（コード確認・即時 [x]）

- テンプレファイル

## 推測した用途（§6.4）

- **レベル制作:** コンポーネント・行動クラスの雛形を `%ClassName%` で量産
- **エディタ:** 次タスクで manifest から生成パイプラインへ接続

**状態:** ループ稼働中（subscription timer 15m）

---

## セッション 2026-11-08 18:45（/loop 15m Cloud・テンプレ置換ツール）

**ブランチ:** `cursor/cloud-agent-1794144340853-5xzcq`

### 1. 完了したこと

- **`scaffold_from_template.py`** — list / generate、%Token% 置換、衝突時は非上書き
- **README**、`text_code_templates.md` 更新
- **ビルド:** 不要

## 実装済みと判断した項目（コード確認・即時 [x]）

- 作成・コピー・置換コード

## 推測した用途（§6.4）

- **制作:** CLI からコンポーネント雛形を安全に追加
- **CI／エージェント:** dry-run でパス確認後に実生成

**状態:** ループ稼働中（subscription timer 15m）

---

## セッション 2026-11-08 19:00（/loop 15m Cloud・コードテンプレ UI）

**ブランチ:** `cursor/cloud-agent-1794144340853-5xzcq`

### 1. 完了したこと

- **`EditorCodeTemplateScaffold`** — manifest 読込・置換・書き出し
- **エンティティエディタ** ドック「コードテンプレ」＋メニュー
- **vcxproj** 登録
- **ビルド:** Cloud 未実施（DEVELOPMENT 必須）

## 実装済みと判断した項目（コード確認・即時 [x]）

- エディタ組み込み（テキスト系 %テンプレ 一式完了）

## 推測した用途（§6.4）

- **エディタ制作:** コンポーネント追加を GUI から完結（vcxproj は手動／次改善）

**状態:** ループ稼働中（subscription timer 15m）

---

## セッション 2026-11-08 19:15（/loop 15m Cloud・案内吹き出し）

**ブランチ:** `cursor/cloud-agent-1794144340853-5xzcq`

### 1. 完了したこと

- **`OperatorGuidance::System`** + `operator_guidance.json`
- **SceneGame** / **CmpMoveByInput（parry）** 連携、吹き出しスプライト描画
- **メモ** `operator_guidance_bubble.md`
- **ビルド:** Cloud 未実施

## 実装済みと判断した項目（コード確認・即時 [x]）

- 案内吹き出しシステム

## 推測した用途（§6.4）

- **ランタイム UX:** 操作忘れの再案内（howToPlay 画像を吹き出し内に再利用）

**状態:** ループ稼働中（subscription timer 15m）

---

## セッション 2026-11-08 19:30（/loop 15m Cloud・ナビキャラ／チュートリアル置換）

**ブランチ:** `cursor/cloud-agent-1794144340853-5xzcq`

### 1. 完了したこと

- **`navigator` / `tutorialReplacement`** — JSON・ポートレート表示・welcome ヒント
- **SceneGame** — イントロ howTo スキップ、`OnBattleReady` 連携
- **roadmap** 全 `- [ ]` 消化 → **ループ停止**

## 実装済みと判断した項目（コード確認・即時 [x]）

- チュートリアル置換・ナビキャラ

## 推測した用途（§6.4）

- **UX:** 固定チュートリアル UI の代わりにナビ付き吹き出しで操作案内

## ループ自動停止

- **理由:** `roadmap.md` に未完了 `- [ ]` がなくなったため `loop-roadmap-dev-15m` を unsubscribe

**状態:** ループ停止（timer 解除済み）

---

## セッション 2026-11-09 23:30（用途別・実装ギャップ正本の新規作成）

**ブランチ:** `cursor/cloud-agent-1794144340853-5xzcq`

### 1. 完了したこと

- `docs/implementation_gaps.md`（schema 2・運用ルール・空一覧）
- `docs/implementation_gaps_index.json`（空 index）
- `docs/use_case_scenarios.md`（UC 各 5 シナリオのシード）
- `cursorInstructions.md` §6.6 追記

### 2. 課題・選択

- ギャップスキャンループ本体（スクリプト／自動マージ）は未実装。正本ファイルと手順のみ。

### 3. 次ステップ

- 初回 tick: `UC-Debug` + `DBG-Streaming-001` などでスキャンし index にマージ

## 実装済みと判断した項目（コード確認・即時 [x]）

なし

## 保留・着手見送り（§6.3）

なし

## 用途不明で確認待ち（§6.4）

なし

## 推測した用途（§6.4）

- **CursorAI ドキュメント（ギャップ在庫）**
  - 用途: デバッグ・制作・出荷の観点で不足を ID 付きで蓄積し、半自律ループで重複なく更新する
  - 使用場面: デバッグ／制作（エディタ）／ビルド・配布（シナリオ定義）
  - 用途から決めた仕様: 用途不明は GAP 化せず PENDING のみ。`Codes/` の変更はギャップ tick とは分離

---

## セッション 2026-11-09 23:35（ギャップスキャン tick 機構）

**ブランチ:** `cursor/cloud-agent-1794144340853-5xzcq`

### 1. 完了したこと

- キーワード「ギャップスキャンを実行して」用 §6.6 拡張・`.cursor/rules/gap-scan.mdc`
- `planGapScanTick.py` / `recordGapScanTick.py` / `syncImplementationGapsMd.py`
- `loopGapScanDev.ps1`（§7.1.4）

### 3. 次ステップ

- チャットまたは `loopGapScanDev.ps1` で初回 tick 実行

---

## ギャップスキャン tick（SCAN-20261109-232947）

- **useCase:** UC-Play / **scenario:** PLY-Audio-001
- **newIds:** 3（ChangeSceneFade 遷移 SE 未配線、CommandScene BGM 瞬断、SceneMovie オーディオ方針）
- **loop:** `loop-gap-scan` 10m タイマー登録済み

## ギャップスキャン tick（SCAN-20261109-234011）

- **useCase:** UC-Authoring / **scenario:** AUT-AssetLoop-001
- **newIds:** 3（エンティティ複製 UI なし、複数選択なし、コンポーネント CP なし）

## ギャップスキャン tick（SCAN-20261109-235016）

- **useCase:** UC-Debug / **scenario:** DBG-FeatureScene-001
- **newIds:** 3（SceneBuildUp 未配線、シーン実行がプレビュー限定、FeatureTest 既定シーンなし）

## ギャップスキャン tick（SCAN-20261110-000016）

- **useCase:** UC-Play / **scenario:** PLY-Camera-001
- **newIds:** 3（BuildUp シルエットなし、verify_camera_policy 未 CI、Scene 基底 unlock 既定）

## ギャップスキャン tick（SCAN-20261110-001015）

- **useCase:** UC-Authoring / **scenario:** AUT-Component-001
- **newIds:** 3（kind 不一致スキップ、登録表ドリフト、component verify 未整備）

## ギャップスキャン tick（SCAN-20261110-002012）

- **useCase:** UC-Play / **scenario:** PLY-Movie-001
- **newIds:** 3（Movie 未配線、トラック失敗即復帰、sample JSON 未 CI）

## ギャップスキャン tick（SCAN-20261110-003022）

- **useCase:** UC-Authoring / **scenario:** AUT-EditorPlace-001
- **newIds:** 3（未保存でシーン切替、読込失敗が空シーン化、ビューポート配置なし）

## ギャップスキャン tick（SCAN-20261110-004013）

- **useCase:** UC-Play / **scenario:** PLY-SceneLoad-001
- **newIds:** 3（Dev 起動がエディタ、UX JSON 未検知、BuildUp 読込失敗置換）

## ギャップスキャン tick（SCAN-20261110-005019）

- **useCase:** UC-Authoring / **scenario:** AUT-Guidance-001
- **newIds:** 3（message 未描画、UX JSON エディタなし、actionId parry のみ）

## ギャップスキャン tick（SCAN-20261110-010019）

- **useCase:** UC-Play / **scenario:** PLY-Visual-001
- **newIds:** 3（ライト直書き、BuildUp フォグ等なし、SceneMovie リムなし）

## ギャップスキャン tick（SCAN-20261110-011015）

- **useCase:** UC-Authoring / **scenario:** AUT-VisualTune-001
- **newIds:** 3（カメラ tuning エディタ未実装、JSON 手動同期、ポスト UI なし）

## ギャップスキャン tick（SCAN-20261110-012008）

- **useCase:** UC-Ship / **scenario:** SHP-AssetPack-001
- **newIds:** 3（PostBuild は Assets のみ、amesh CI 無し、アセットパス検証無し）

## ギャップスキャン tick（SCAN-20261110-013008）

- **useCase:** UC-Ship / **scenario:** SHP-Config-001
- **newIds:** 3（Debug も DEVELOPMENT、Editor TU が Release に残る、構成マトリクス CI 無し）

## ギャップスキャン tick（SCAN-20261110-014010）

- **useCase:** UC-Ship / **scenario:** SHP-DevBuild-001
- **newIds:** 3（buildDevelopment と CI 差分、NuGet 無し、Cloud Linux 未定義）

## ギャップスキャン tick（SCAN-20261110-015009）

- **useCase:** UC-Ship / **scenario:** SHP-Release-001
- **newIds:** 3（Release PDB、JsonTest 同梱、ForDebug TU 未除外）

## ギャップスキャン tick（SCAN-20261110-020007）

- **useCase:** UC-Ship / **scenario:** SHP-Smoke-001
- **newIds:** 3（Release CI スモーク無し、手順書無し、main にスモーク CLI 無し）

## ギャップスキャン tick（SCAN-20261110-021019）

- **useCase:** UC-Debug / **scenario:** DBG-PlayArea-001
- **newIds:** 3（DebugPlayArea パス未定義、シーン一覧対象外、scaffold 未配線）

## ギャップスキャン tick（SCAN-20261110-022017）

- **useCase:** UC-Debug / **scenario:** DBG-Profiler-001
- **newIds:** 3（区間プロファイラ無し、Δt UI 無し、Release 相当計測手順無し）

## ギャップスキャン tick（SCAN-20261110-023023）

- **useCase:** UC-Debug / **scenario:** DBG-RenderDiag-001
- **newIds:** 3（Lighting ImGui 未配線、Editor 診断パネル無し、影の可視化無し）

## ギャップスキャン tick（SCAN-20261110-024011）

- **useCase:** UC-Debug / **scenario:** DBG-Streaming-001
- **newIds:** 3（remaining クランプ、ログ無し、BuildUp でメニュー無し）

## ギャップスキャン tick（SCAN-20261110-025019）

- **useCase:** UC-Play / **scenario:** PLY-Audio-001（2回目）
- **newIds:** 3（Title BGM 維持無し、BuildUp 無音、PauseBgm 未使用）

## ギャップスキャン tick（SCAN-20261110-030010）

- **useCase:** UC-Authoring / **scenario:** AUT-AssetLoop-001（2回目）
- **newIds:** 3（複製ショートカット無し、プレハブ一括配置無し、ヒエラルキー DnD 無し）

## ギャップスキャン tick（SCAN-20261110-031009）

- **useCase:** UC-Debug / **scenario:** DBG-FeatureScene-001（2回目）
- **newIds:** 3（機能検証メニュー無し、実行ビューはプレビューのみ、LoopTick13Demo 未登録）

## ギャップスキャン tick（SCAN-20261110-032016）

- **useCase:** UC-Play / **scenario:** PLY-Camera-001（2回目）
- **newIds:** 3（StageBounds 固定、BuildUp が境界未反映、ParticleEditor の Debug カメラ制限）

## ギャップスキャン tick（SCAN-20261110-033008）

- **useCase:** UC-Authoring / **scenario:** AUT-Component-001（2回目）
- **newIds:** 3（CmpMesh 未登録、未知 type が Draw 既定、次分割候補未整備）

## ギャップスキャン tick（SCAN-20261110-034006）

- **useCase:** UC-Play / **scenario:** PLY-Movie-001（2回目）
- **newIds:** 3（returnScene null ハング、SkyDome／カメラ順序、トラックパス未解決）

## ギャップスキャン tick（SCAN-20261110-035010）

- **useCase:** UC-Authoring / **scenario:** AUT-EditorPlace-001（2回目）
- **newIds:** 3（配置変更は手動保存、ビューポート配置 UI 無し、再入場でディスク再読込）

## ギャップスキャン tick（SCAN-20261110-040022）

- **useCase:** UC-Play / **scenario:** PLY-SceneLoad-001（2回目）
- **newIds:** 3（遷移中 Set 無視、Game 入場カメラ順序、BuildUp パス未解決）

## ギャップスキャン tick（SCAN-20261110-041018）

- **useCase:** UC-Authoring / **scenario:** AUT-Guidance-001（2回目）
- **newIds:** 3（hints 必須ロード、howTo 二重表示、displayName 未描画）

## ギャップスキャン tick（SCAN-20261110-042012）

- **useCase:** UC-Play / **scenario:** PLY-Visual-001（2回目）
- **newIds:** 3（毎フレーム lighting 上書き、二重 VolumetricFog、Title にリム無し）

## ギャップスキャン tick（SCAN-20261110-043016）

- **useCase:** UC-Authoring / **scenario:** AUT-VisualTune-001（2回目）
- **newIds:** 3（プレビューにポスト無し、調整メニュー無し、lighting 用 JSON 無し）

## ギャップスキャン tick（SCAN-20261110-044018）

- **useCase:** UC-Debug / **scenario:** DBG-FeatureScene-001（3回目）
- **newIds:** 3（PlayRequest 未配線、sandbox JSON 未参照、BuildUp 復帰不可）

## ギャップスキャン tick（SCAN-20261110-045017）

- **useCase:** UC-Play / **scenario:** PLY-Audio-001（3回目）
- **newIds:** 3（Result BGM 未維持、撃破即 BGM 停止、StopBgm 全停止）

## ギャップスキャン tick（SCAN-20261110-050024）

- **useCase:** UC-Authoring / **scenario:** AUT-AssetLoop-001（3回目）
- **newIds:** 3（インポート適用 UI、ドロップがコピー止まり、タグでツリー絞込無し）

## ギャップスキャン tick（SCAN-20261110-051012）

- **useCase:** UC-Play / **scenario:** PLY-Camera-001（3回目）
- **newIds:** 3（AllowDebug 既定、verify が Allow 未検査、StageViewCamera アスペクト固定）

## ギャップスキャン tick（SCAN-20261110-052013）

- **useCase:** UC-Authoring / **scenario:** AUT-Component-001（3回目）
- **newIds:** 3（Mesh 存在検出、AssetPath が Draw 内包、未対応 params UI）

## ギャップスキャン tick（SCAN-20261110-053016）

- **useCase:** UC-Play / **scenario:** PLY-Movie-001（3回目）
- **newIds:** 3（失敗時 StageView、JSON 全体拒否、遷移予約で更新停止）

## ギャップスキャン tick（SCAN-20261110-054019）

- **useCase:** UC-Authoring / **scenario:** AUT-EditorPlace-001（3回目）
- **newIds:** 3（プリファブ配置、JSON パス編集、表示名／entityName 分離）

## ギャップスキャン tick（SCAN-20261110-055015）

- **useCase:** UC-Play / **scenario:** PLY-SceneLoad-001（3回目）
- **newIds:** 3（遷移中 Update 停止、子 Entity 未 Init、フェード中間で ChangeScene）

## ギャップスキャン tick（SCAN-20261110-060016）

- **useCase:** UC-Authoring / **scenario:** AUT-Guidance-001（3回目）
- **newIds:** 3（session 未配線、テクスチャ失敗時空枠、JSON 参照 verify 無し）

## ギャップスキャン tick（SCAN-20261110-061024）

- **useCase:** UC-Play / **scenario:** PLY-Visual-001（3回目）
- **newIds:** 3（Fog Flush 順、EnterScene 未ライティング、Result 簡易光のみ）

## ギャップスキャン tick（SCAN-20261110-062012）

- **useCase:** UC-Authoring / **scenario:** AUT-VisualTune-001（3回目）
- **newIds:** 3（FOV 固定、プレビューカメラ、tuning 解像度未参照）

## ギャップスキャン tick（SCAN-20261110-063013）

- **useCase:** UC-Debug / **scenario:** DBG-FeatureScene-001（4回目）
- **newIds:** 3（PlayRequest Path 未使用、BuildUp が Runtime 未参照、起動が EntityEditor 固定）

## ギャップスキャン tick（SCAN-20261110-064013）

- **useCase:** UC-Play / **scenario:** PLY-Audio-001（4回目）
- **newIds:** 3（PlaySceneBgm 全停止、PlaySeLoop 未使用、撃破演出中 BGM 維持無効）

## ギャップスキャン tick（SCAN-20261110-065006）

- **useCase:** UC-Authoring / **scenario:** AUT-AssetLoop-001（4回目）
- **newIds:** 3（空追加のみ、Imported 一覧無し、トランスフォーム一括無し）

## ギャップスキャン tick（SCAN-20261110-070015）

- **useCase:** UC-Debug / **scenario:** DBG-FeatureScene-001（5回目）
- **newIds:** 3（BuildUp が EditorFrame 依存、F2 検証無し、PlayRequest Clear 未使用）

## ギャップスキャン tick（SCAN-20261110-071014）

- **useCase:** UC-Play / **scenario:** PLY-Audio-001（5回目）
- **newIds:** 3（Resume 未使用、音量フェード未使用、撃墜／撃破 BGM 非対称）

## ギャップスキャン tick（SCAN-20261110-072023）

- **useCase:** UC-Authoring / **scenario:** AUT-AssetLoop-001（5回目）
- **newIds:** 3（単体削除のみ、タグ一括解除無し、プレハブが Definition のみ）

## ギャップスキャン tick（SCAN-20261110-073017）

- **useCase:** UC-Debug / **scenario:** DBG-FeatureScene-001（6回目）
- **newIds:** 3（Launch が Enter 無し、実行＝タブのみ、README 手順のみ）

## ギャップスキャン tick（SCAN-20261110-074024）

- **useCase:** UC-Play / **scenario:** PLY-Audio-001（6回目）
- **newIds:** 3（BGM 再試行、イントロ中 SE 停止、タイトル Exit 即 BGM 停止）

## 実装済みと判断した項目（コード確認・即時 [x]）

なし

## 保留・着手見送り（§6.3）

なし

## 用途不明で確認待ち（§6.4）

なし

## 推測した用途（§6.4）

- **ギャップスキャン自動化（CursorAI ツール）**
  - 用途: デバッグ・制作・出荷観点の不足を優先順 1 シナリオ／tick で蓄積
  - 使用場面: デバッグ（半自律 `/loop`）
  - 用途から決めた仕様: roadmap ループと分離、ドキュメントのみ tick はビルド・ブランチ不要

## ギャップスキャン tick（SCAN-20261110-075021）

- **useCase:** UC-Authoring / **scenario:** AUT-AssetLoop-001（6回目）
- **newIds:** 3（コンポ 1 件削除、プレハブ 1 体追加、実行順 1 スロットのみ）

## 実装済みと判断した項目（コード確認・即時 [x]）

なし

## 保留・着手見送り（§6.3）

なし

## 用途不明で確認待ち（§6.4）

なし

---

## エンティティカメラ要件ドキュメント（P0）

- **実施:** `entity_camera_components.md` 作成、`roadmap.md` リンク、`scene_movie`→`movie_camera_controller` 統合スタブ、`roadmap_rough_old` 重複本文削除
- **手動確認:** 未実施（ドキュメントのみ）
- **未検証:** なし（ビルド対象外）
- **次:** P1 実装（`cursor/cmp-camera-entity-0f04`）

## エンティティカメラ Phase P1 完了

- **実施:** `CmpCamera` / `SceneCameraRig` / `ActiveCameraSystem` / `MainCamera.json` / `EntityFactory` プレハブ読み込み / エディタ登録 / `GameOrder` 描画直前 Prepare
- **手動確認:** **未実施**（タイトル・ゲーム・エディタでのカメラ挙動）
- **段階末・未検証:** **あり**（MSVC ビルド・実機確認はローカル待ち）
- **次 Phase:** **P2** — エンジン側の複数カメラ登録・アクティブ index 切替、primary 契約明文化

## 推測した用途（§6.4）

- **entity_camera_components.md**
  - 用途: エンティティカメラの要件・段階チェックの正本（他エージェント引き継ぎ）
  - 使用場面: 半自律・Cloud Agent の着手前 Read
  - 仕様: progress_report とチェック更新を同期
- **CmpCamera / SceneCameraRig（P1）**
  - 用途: 全シーンで主カメラをエンティティ化し `GetCamera()` を統一
  - 使用場面: ランタイム描画、StageView 適用、将来の複数カメラ
  - 仕様: 自動スポーン、`camera.main` タグ、描画直前行列更新

## Release スタートアップシーン（launch_config.json）

- **実装:** `EditorAssets/launch_config.json`・`ResolveStartupScenePath`・Release `CommandScene` → `SceneBuildUp`・エディタ「起動シーンに設定」・Post-build で `EditorAssets` 同梱
- **ビルド:** Development / Release x64 成功（エージェント環境）
- **手動確認:** Release exe 起動で JSON シーン表示、F1 でタイトル復帰は未確認

## ギャップスキャン tick（SCAN-20261110-230538）

- **useCase:** UC-Debug / **scenario:** DBG-FeatureScene-001（7回目）
- **newIds:** 3（Draw 内 camera Update、RT SetCamera、BuildPreviewScene のみ）
- **備考:** ブランチ `cursor/cmp-camera-entity-0f04`（カメラ PR と同ブランチに doc 追記）

## ギャップスキャン tick（SCAN-20261110-231005）

- **useCase:** UC-Play / **scenario:** PLY-Audio-001（7回目）
- **newIds:** 3（IsBgmLoopPaused 未使用、再生秒 API 未使用、遷移中 BGM 維持スキップ）

## ギャップスキャン tick（SCAN-20261110-232021）

- **useCase:** UC-Authoring / **scenario:** AUT-AssetLoop-001（7回目）
- **newIds:** 3（未知 Cmp UI、単一ヒエラルキー選択、型変更 1 スロット）

## ギャップスキャン tick（SCAN-20261110-233005）

- **useCase:** UC-Debug / **scenario:** DBG-FeatureScene-001（8回目）
- **newIds:** 3（BuildUp entities と cameraRig 分離、RT で StageView 再上書き、Runtime タブ差し替え）

## ギャップスキャン tick（SCAN-20261110-234025）

- **useCase:** UC-Play / **scenario:** PLY-Audio-001（8回目）
- **newIds:** 3（BGM シーク未配線、ピッチ未配線、撃破演出中の維持スキップ）

## ギャップスキャン tick（SCAN-20261110-235013）

- **useCase:** UC-Authoring / **scenario:** AUT-AssetLoop-001（8回目）
- **newIds:** 3（インスタンス複製 API 無し、空追加のみ、HitSphere 単一球編集）

## ギャップスキャン tick（SCAN-20261111-000032）

- **useCase:** UC-Debug / **scenario:** DBG-FeatureScene-001（9回目）
- **newIds:** 3（SceneCameraRig 走査範囲、プレビュー rig カメラ、PlayRequest 起動未配線）

## ギャップスキャン tick（SCAN-20261111-001015）

- **useCase:** UC-Play / **scenario:** PLY-Audio-001（9回目）
- **newIds:** 3（GetSeVolume 未配線、GetSeFrequencyRatio 未配線、リザルト Exit 即 BGM 停止）

## ギャップスキャン tick（SCAN-20261111-002014）

- **useCase:** UC-Authoring / **scenario:** AUT-AssetLoop-001（9回目）
- **newIds:** 3（CmpCamera UI 無し、コンポ単体削除、タグ一括付与無し）

## ギャップスキャン tick（SCAN-20261111-003024）

- **useCase:** UC-Debug / **scenario:** DBG-FeatureScene-001（10回目）
- **newIds:** 3（Cmp 正射影既定、BuildUp Update 漏れ、stageView ロック固定）

## ギャップスキャン tick（SCAN-20261111-004014）

- **useCase:** UC-Play / **scenario:** PLY-Audio-001（10回目）
- **newIds:** 3（Game Exit 即 BGM 停止、StopSe 未使用、PlaySe キャッシュ無期限）

## ギャップスキャン tick（SCAN-20261111-005013）

- **useCase:** UC-Authoring / **scenario:** AUT-AssetLoop-001（10回目）
- **newIds:** 3（削除後シーン先頭固定、ロードで選択 0、実行順 1 件ずつ）

## ギャップスキャン tick（SCAN-20261111-010025）

- **useCase:** UC-Debug / **scenario:** DBG-FeatureScene-001（11回目）
- **newIds:** 3（BuildUp ロード失敗ダミー、起動パス試行 UI 無し、BuildEntities に MainCamera 無し）

## ギャップスキャン tick（SCAN-20261111-011012）

- **useCase:** UC-Play / **scenario:** PLY-Audio-001（11回目）
- **newIds:** 3（SetSeVolume 未使用、IsPlaying+Paused、フェード中盤で次 BGM）

## ギャップスキャン tick（SCAN-20261111-012017）

- **useCase:** UC-Authoring / **scenario:** AUT-AssetLoop-001（11回目）
- **newIds:** 3（シーン一覧二段、プレハブ常時新規、Cmp 型変更で params リセット）

## ギャップスキャン tick（SCAN-20261111-013013）

- **useCase:** UC-Debug / **scenario:** DBG-FeatureScene-001（12回目）
- **newIds:** 3（startup 空で Title、launch_config が Scene_0、Enter で SetupStageViewCamera）

## ギャップスキャン tick（SCAN-20261111-014014）

- **useCase:** UC-Play / **scenario:** PLY-Audio-001（12回目）
- **newIds:** 3（SetSeFrequency 未使用、GameOrder 音声未管理、イントロ中 Maintain）

## ギャップスキャン tick（SCAN-20261111-015018）

- **useCase:** UC-Authoring / **scenario:** AUT-AssetLoop-001（12回目）
- **newIds:** 3（Dock 初回固定、直近ドロップ 1 Cmp、新規即保存）

## ギャップスキャン tick（SCAN-20261111-020020）

- **useCase:** UC-Debug / **scenario:** DBG-FeatureScene-001（13回目）
- **newIds:** 3（Exit でタブ未復帰、JSON パス非表示、フェード中 F1 無効）

## ギャップスキャン tick（SCAN-20261111-021016）

- **useCase:** UC-Play / **scenario:** PLY-Audio-001（13回目）
- **newIds:** 3（PlaySe ハンドル破棄、ワンショット一括停止なし、Title Submit+フェード）

## ギャップスキャン tick（SCAN-20261111-022018）

- **useCase:** UC-Authoring / **scenario:** AUT-AssetLoop-001（13回目）
- **newIds:** 3（保存後プレビュー無し、複数ドロップ上書き、シーン切替でプレハブ選択残り）

## ギャップスキャン tick（SCAN-20261111-023031）

- **useCase:** UC-Debug / **scenario:** DBG-FeatureScene-001（14回目）
- **newIds:** 3（BuildUp タグカタログ未同期、AllowDebugCamera 未上書き、scaffold 完了案内が Launch のみ）

## ギャップスキャン tick（SCAN-20261111-024014）

- **useCase:** UC-Play / **scenario:** PLY-Audio-001（14回目）
- **newIds:** 3（ストリームプール奪取、Resume が Start のみ、5MB 閾値のみでストリーム判定）

## ギャップスキャン tick（SCAN-20261111-025013）

- **useCase:** UC-Authoring / **scenario:** AUT-AssetLoop-001（14回目）
- **newIds:** 3（削除時子の親付替え無し、CmpCamera 既定 params 無し、Cmp 1 件ずつ削除のみ）

## ギャップスキャン tick（SCAN-20261111-030016）

- **useCase:** UC-Debug / **scenario:** DBG-FeatureScene-001（15回目）
- **newIds:** 3（TryLoad タグ未同期、ビュー毎ライト再設定、Registry Launch 蓄積）

## ギャップスキャン tick（SCAN-20261111-031017）

- **useCase:** UC-Play / **scenario:** PLY-Audio-001（15回目）
- **newIds:** 3（Paused 中 Update スキップ、OnStreamEnd 瞬断、BGM 秒数 44.1kHz 固定）

## ギャップスキャン tick（SCAN-20261111-032014）

- **useCase:** UC-Authoring / **scenario:** AUT-AssetLoop-001（15回目）
- **newIds:** 3（型変更で params 全置換、同名インポート連番、起動設定が暗黙 Save）

## ギャップスキャン tick（SCAN-20261111-033012）

- **useCase:** UC-Debug / **scenario:** DBG-FeatureScene-001（16回目）
- **newIds:** 3（launch 失敗が空文字、起動パス未正規化、BuildUp ルートのみ Draw）

## ギャップスキャン tick（SCAN-20261111-034021）

- **useCase:** UC-Play / **scenario:** PLY-Audio-001（16回目）
- **newIds:** 3（失敗時 playId 不整合、PCM 端数退避、BGM フォールバックで短音解放）

## ギャップスキャン tick（SCAN-20261111-035014）

- **useCase:** UC-Authoring / **scenario:** AUT-AssetLoop-001（16回目）
- **newIds:** 3（プレイモードのみトグル、タグ改名 1 語ずつ、JSON パス自由編集）

## ギャップスキャン tick（SCAN-20261111-040020）

- **useCase:** UC-Debug / **scenario:** DBG-FeatureScene-001（17回目）
- **newIds:** 3（launch 1 キー保存、起動前 JSON 未検証、BuildUp 表示名が ctor 仮値）

## ギャップスキャン tick（SCAN-20261111-041012）

- **useCase:** UC-Play / **scenario:** PLY-Audio-001（17回目）
- **newIds:** 3（SE index 永久キャッシュ、初期 Refill 1 回、OnBufferEnd が Streaming のみ）

## ギャップスキャン tick（SCAN-20261111-042015）

- **useCase:** UC-Authoring / **scenario:** AUT-AssetLoop-001（17回目）
- **newIds:** 3（D&D 後プレビュー未更新、from_json params 未マージ、衝突リネーム連番のみ）

## ギャップスキャン tick（SCAN-20261111-043006）

- **useCase:** UC-Debug / **scenario:** DBG-FeatureScene-001（18回目）
- **newIds:** 3（ChangeScene 常時描画、Enter 無条件、PlayRequest 未 Clear）

## ギャップスキャン tick（SCAN-20261111-044011）

- **useCase:** UC-Play / **scenario:** PLY-Audio-001（18回目）
- **newIds:** 3（BGM Pause が Stop のみ、Resume に Refill なし、PlaySceneBgm 失敗 playId）

## ギャップスキャン tick（SCAN-20261111-045006）

- **useCase:** UC-Authoring / **scenario:** AUT-AssetLoop-001（18回目）
- **newIds:** 3（プレハブ先頭 1 件、Drop キュー不可視、インスタンス複製 UI なし）

## ギャップスキャン tick（SCAN-20261111-050017）

- **useCase:** UC-Debug / **scenario:** DBG-FeatureScene-001（19回目）
- **newIds:** 3（子 Entity 未 Initialize、カメラ無し無言 return、Launch は Enter なし）

## ギャップスキャン tick（SCAN-20261111-051011）

- **useCase:** UC-Play / **scenario:** PLY-Audio-001（19回目）
- **newIds:** 3（Decoder EOF、Waiting 非ループ即停止、ストリーム wfEx 固定）

## ギャップスキャン tick（SCAN-20261111-052010）

- **useCase:** UC-Authoring / **scenario:** AUT-AssetLoop-001（19回目）
- **newIds:** 3（プレハブ連番、一覧フルパス、AssetPath プレビュー1回）

## ギャップスキャン tick（SCAN-20261111-053014）

- **useCase:** UC-Debug / **scenario:** DBG-FeatureScene-001（20回目）
- **newIds:** 3（実行はタブのみ、固定ライト上書き、Enter で Runtime 強制）

## ギャップスキャン tick（SCAN-20261111-054008）

- **useCase:** UC-Play / **scenario:** PLY-Audio-001（20回目）
- **newIds:** 3（MF PCM 固定、バッファ 0 埋め、SE 枯渇 assert）

## ギャップスキャン tick（SCAN-20261111-055013）

- **useCase:** UC-Authoring / **scenario:** AUT-AssetLoop-001（20回目）
- **newIds:** 3（タグ 1 件付与、実行順単体、型差替 params 全置換）

## ギャップスキャン tick（SCAN-20261111-060023）

- **useCase:** UC-Debug / **scenario:** DBG-FeatureScene-001（21回目）
- **newIds:** 3（シーン一覧 Scenes のみ、launch 空 path、Exit で Ui 残存）
- **fix:** `EntityEditorUi-029` と `-019` の scopeKey 衝突（:745）を :746 に分離

## ギャップスキャン tick（SCAN-20261111-061024）

- **useCase:** UC-Play / **scenario:** PLY-Audio-001（21回目）
- **newIds:** 3（Load index キャッシュ、PlayStreaming playId、5MB 閾値固定）

## ギャップスキャン tick（SCAN-20261111-062020）

- **useCase:** UC-Authoring / **scenario:** AUT-AssetLoop-001（21回目）
- **newIds:** 3（プレハブ定義のみ、タグ全解除 1 体、カタログ改名 1 件）

## ギャップスキャン tick（SCAN-20261111-063020）

- **useCase:** UC-Debug / **scenario:** DBG-FeatureScene-001（22回目）
- **newIds:** 3（instanceName 未反映、プレビュー親のみ、scaffold は名前のみ）

## ギャップスキャンループ停止（ユーザー指示）

- **日時:** 2026-11-11（JST）
- **操作:** `loop-gap-scan`（600s）を `unsubscribe`（`sub_dbb157f3-f608-4454-966b-142597f448d8`）
- **直前状態:** オープン GAP 289、完了スキャン 96 回（tick 97 まで）

## 推測した用途（§6.4）

- **launch_config / SceneBuildUp 起動**
  - 用途: 配布ビルドでエディタ作成シーンを本番同等パスで試す
  - 使用場面: 制作物のスモークテスト、デモ配布、CI 用プレイ可能ビルド
  - 仕様: パスは exe 隣 `../EditorAssets` 基準、欠損時はタイトルフォールバック

## パーティクル能力サンプル JSON（ユーザー依頼）

- **追加:** `EditorAssets/Particles/SAMPLES.md` と `samples/01`〜`15`（発射モード・形状・運動・描画・キル領域・多レイヤー等を1ファイル1テーマ）
- **用途:** パーティクルエディタで「今何が表現できるか」をすぐ見比べるカタログデータ
- **使用場面:** エフェクト仕様の検討、新規演出のたたき、プログラマ／アーティスト間の共通言語

## パーティクルデモ（エンティティ＋コンポーネント化）

- **撤回:** 専用 `SceneParticleDemo` シーンクラスは削除
- **追加コンポーネント:** `CmpParticleEmitter`（JSON 再生・カルーセル）、`CmpBounceBall`（壁反射＋衝撃波ワンショット）
- **シーン:** `EditorAssets/Scenes/ParticleDemo.json`（Showreel / Ball / 壁エンティティ）
- **起動:** F3 → 既存 `SceneBuildUp` で上記 JSON を実行
- **推測した用途:** レベル制作データとしてパーティクル演出を試す、コンポーネント再利用、エディタで配置・調整

## ビルボード100枚インスタンス＋Releaseシーン巡回

- **CmpBillboardInstancedGrid** … 10×10=100 枚を `DrawBillboardParticlesInstanced` で描画
- **シーン:** `EditorAssets/Scenes/BillboardInstancing100.json`
- **Release:** `release_scene_carousel.json` + `SceneReleaseCarousel`（Space / パッド A で BuildUp JSON のみ巡回。Title/Game/Result は対象外）
- **推測した用途:** GPU インスタンシング確認、配布ビルドのシーン一覧スモーク、デモ・検証ループ

## セッション 2026-11-11（ギャップ在庫 → roadmap 手入れ）

- **実施:** `implementation_gaps_index.json`（未解消 289）を `roadmap.md` に反映
- **方針:** P0/P1 を各 `##` 節末の「ギャップスキャン追補」に 43 行（gap ID 付き）。P2/P3 はテーマ別在庫行＋正本参照
- **追加:** ヘッダのギャップ在庫リンク、`## GapScan（索引）`
- **再実行用:** `EditorAssets/Tools/CursorAI/syncGapScanToRoadmap.py`（index 更新後に再生成可）

### roadmap 変更理由（§6.1）

| 種別 | 内容 |
|------|------|
| 追加 | Audio / System / Camera / Scene / Render / Debug / Repository / Editor / UX にギャップ追補節 |
| 統合 | P2/P3 246 件は節ごとに束ね（個別行は `implementation_gaps.md` 正本） |
| 即時完了 | なし（実装タスクの新設のみ） |

---

## セッション 2026-11-11 14:33（/loop 15m tick1・ロードマップ実行）

**ブランチ:** `fix/StreamingDebugDecoderBacklog`（`loopMergeTickToDevelop.ps1` はユーザー skip のため **未マージ**）  
**タスク:** roadmap 先頭 `- [ ]` — decoderBacklog01 のクランプで remaining 肥大が見えない（`GAP-Audio-StreamingDebug-001`）

### 1. 完了したこと

- `StreamingVoiceMemoryRow::decoderBacklogRatio` を追加し、ImGui で 1 超を赤バー＋ `Nx !` 表示
- `streaming_debug_imgui.md`・`roadmap.md` 該当行を `[x]`
- **Development | x64** ビルド成功

### 2. 課題・メモ

| 項目 | 内容 |
|------|------|
| develop マージ | **完了**（ユーザー許可後 `loopMergeTickToDevelop`、commit `5701d19`） |
| 次タスク候補 | シーン遷移 BGM／SE（在庫行・範囲大。着手前に細分化推奨） |

### 3. 次ステップ

1. コミット／マージ許可 → `loopMergeTickToDevelop.ps1` または `finishTaskGit.ps1`
2. 次 tick: 上から次の `- [ ]`（BGM 遷移は §6.1 細分化を検討）

## 実装済みと判断した項目（コード確認・即時 [x]）

なし（本 tick は実装あり）

## 保留・着手見送り（§6.3）

なし

## 用途不明で確認待ち（§6.4）

なし

## 推測した用途（§6.4）

- **decoderBacklog 可視化（GAP-Audio-StreamingDebug-001）**
  - 用途: BGM ストリームの remaining 肥大をデバッグメニューで即判別
  - 使用場面: デバッグ（DEVELOPMENT + ImGui）
  - 仕様: バーは 0〜1、比率 1 超は数値＋警告色（ランタイムコストはスナップショット取得のみ）

---

## セッション 2026-11-11 15:25（ループ占有宣言 lease.md の導入）

**ブランチ:** `feature/agentCollisionAvoidance`（roadmap 外・ユーザー指示タスク）

### 1. 進捗・完了したこと

- `loopLease.ps1` / `loopLeaseCommon.ps1` を新規作成。`Control/lease.md` に各ループの占有パスを宣言（Begin / Claim / End / Release / Test / Show）。他ループと重なれば exit 4 で tick を中止し、`progress_report.md` に「占有衝突でスキップ」を追記
- `lease.md` は Git 管理外（`.gitignore`）。メインワークツリー側に 1 つだけ置き、`git worktree` 間でも共有。占有は `IntervalMinutes × 2`（最短 60 分）で失効。読み書きはロックファイルで直列化
- 既存スクリプトに任意引数 `-LoopId` を追加（未指定時の挙動は従来どおり）
  - `loopMergeTickToDevelop.ps1`: tick 中の他ループの占有パスを `git add` から除外
  - `loopRollbackToLastGood.ps1` / `loopRegisterBuildFailure.ps1`: 自ループの占有パスだけをロールバック（`reset --hard` / `clean -fd` を使わない）
  - `loopRoadmapDev.ps1` / `loopGapScanDev.ps1`: ループ ID（既定 `roadmap-dev` / `gap-scan`）を tick プロンプトに記載、自動停止時に占有を解放
- 手順: `cursorInstructions.md` §7.1.5、`.cursor/rules/loop-lease.mdc`（常時適用）、`gap-scan.mdc`・`roadmap-execute.mdc`・`README.md` に参照を追記
- ユーザールール変更: 「既存実装の修正禁止」→「既存実装の修正よりも新たな実装など細分化での解決を優先」（`roadmap-execute.mdc` も同趣旨に変更）
- 検証: PowerShell 5.1 で全 `.ps1` の構文確認、lease CLI の各操作、一時リポジトリでスコープ付きロールバック・コミット除外・スキップ報告を確認（ビルド対象のコード変更なし）

### 2. 課題・設計上の選択肢

- **同じ作業ツリーではブランチが全ループ共通。** roadmap ループの `git checkout` は他ループの作業ツリーにも及ぶ。roadmap ループを複数並行させる場合は `git worktree` で分ける必要がある（lease は同一ファイル編集の衝突を防ぐのみ）
- **gap-scan の変更は roadmap ループのコミットに相乗りする。** gap-scan が tick 中でなければ、その変更は roadmap 側の `loopMergeTickToDevelop.ps1` でコミットされる（従来と同じ）
- **既存実装の課題点:** `loopTaskCheckpoint.ps1` は手順書から参照されていない一方、`Invoke-Git` の引数名が自動変数 `$Args` と衝突しており（`loopMergeTickToDevelop.ps1` で修正済みの不具合と同種）、Rollback は `reset --hard` / `clean -fd` で作業ツリー全体を戻す。使う場合は修正、使わないなら削除を検討
- **既存実装の課題点:** `loopGapScanDev.ps1` に BOM が無く、Windows PowerShell 5.1 で日本語が誤読される恐れがあった（今回 BOM を付与）

### 3. 次ステップ

1. コミット／マージ許可 → `finishTaskGit.ps1`
2. 実ループ（roadmap-dev と gap-scan の同時稼働）で Begin / End の運用を確認

## 実装済みと判断した項目（コード確認・即時 [x]）

なし

## 保留・着手見送り（§6.3）

なし

## 用途不明で確認待ち（§6.4）

なし

## 推測した用途（§6.4）

- **ループ占有宣言（lease.md）**
  - 用途: エンジン・エディタ開発で、roadmap 実装ループとギャップスキャンループなど複数の自律エージェントを並行稼働させ、同じソースやドキュメントを同時編集して作業を壊し合うのを防ぐ
  - 使用場面: ビルド・配布（開発ワークフロー／ツール）。ランタイム・出荷ビルドへの影響なし
  - 用途から決めた仕様: 衝突時は待たずに tick ごとスキップ（ループ間隔ごとに再試行）、異常終了に備えて期限で失効、ロールバックとコミットは自分の占有範囲に限定、制作者の手編集を前提にしない自動生成ファイル

## セッション 2026-11-13 18:31（Release シーン巡回を ParticleDemo ⇔ BillboardInstancing100 に変更）

### 1. 実施内容

- Release ビルドのコンパイルエラー修正: `SceneReleaseCarousel.h`（`<vector>` 不足・`Entry` 型の宣言と定義の不一致 → `ReleaseCarouselEntry` に統一）、`SceneParticleServices.h`（`<string>` 不足）、`CmpParticleEmitter.cpp`（const 関数から `RefOwner()` 呼び出し → `GetOwner()`）、`GameOrder.h`（Release 分岐で `SceneReleaseCarousel.h` 未 include）
- `EditorAssets/release_scene_carousel.json` と `SceneReleaseCarousel.cpp` の既定リストを `ParticleDemo.json` → `BillboardInstancing100.json` の 2 件に変更（起動シーンは `launch_config.json` の `BillboardInstancing100`）
- Development|x64・Release|x64 ともビルド成功

### 2. 課題・設計上の選択肢

- **既存実装の課題点:** Development 構成でしかビルド確認しておらず、Release 専用コード（`!DEVELOPMENT && !USE_IMGUI`）の include 漏れが見逃されていた。`buildDevelopment.cmd` と同様の Release 用ビルドスクリプトがあるとよい
- **既存実装の課題点:** `SceneReleaseCarousel.cpp` の既定リストは `release_scene_carousel.json` と内容が重複しており、変更時に二重管理になる

## 推測した用途（§6.4）

- **Release シーン巡回（ParticleDemo ⇔ BillboardInstancing100）**
  - 用途: 配布ビルドでのデモ・展示。パーティクル表現とビルボードのインスタンシング描画を、Space／パッド A で交互に見せる
  - 使用場面: ビルド・配布（Release のみ）。Development／エディタの遷移には影響なし
  - 用途から決めた仕様: 巡回順はコード変更不要の JSON マニフェストで指定し、マニフェストが無い場合も同じ 2 シーンで巡回する

## セッション 2026-11-13 18:41（複数エミッタ／複数レイヤーのパーティクル描画修正）

ブランチ: `fix/particle-instance-buffer-per-draw`

### 1. 実施内容

- 原因: `Draw::Draw*ParticlesInstanced` が毎回インスタンスバッファの先頭へ `memcpy` し、`instanceBufferBase` も常に 0、`particleFrameBuffer_` も単一 CBV だったため、GPU 実行時にはフレーム内で最後に書いたデータ（後から追加される衝撃波エミッタ等）を全パーティクルドローが参照していた
- `Draw::WriteParticleInstancesToFrameBuffer` を新設し、フレーム内で書き込み位置を前詰めで進める方式に変更（`particleInstanceWriteCount_`、`AtrumEngine` のフレーム末でリセット）。各ドローに `instanceBufferBase` を渡す（シェーダは対応済みのため変更なし）
- `particleFrameBuffer_` を `MultiConstantBuffer`（`kMaxDrawCount` スロット）にし、`constantBufferCount_` 番目を各ドローの CBV に使用
- 上限確認: `kMaxParticleInstances` = 16384（フレーム合計）。ParticleDemo は最大でも数百個、samples 各 JSON は 16〜128、`editorTest.json` 522 で **上限には到達していない**。上限超過時は超過分を描画しない（切り詰め）
- Development|x64・Release|x64 ビルド成功（実機での見た目確認は未実施）

### 2. 課題・設計上の選択肢

- **既存実装の課題点:** インスタンス上限超過・`kMaxDrawCount` 超過はどちらも無言で描画をスキップする。Development ではログや ImGui で超過を通知できるとよい
- **既存実装の課題点:** `Draw.cpp` に `AgentDebugLogDraw`（`./.cursor/debug-db6cac.log` へ書き込むデバッグ用コード）が残っている

## 推測した用途（§6.4）

- **パーティクルのフレーム内インスタンス割り当て**
  - 用途: ランタイムのゲームプレイ演出（ヒット・衝突エフェクトの one-shot と常駐エフェクトの同時表示）、エディタでの複数エミッタ・多レイヤーのプレビュー
  - 使用場面: ランタイムのゲームプレイ、レベル／アセット制作
  - 用途から決めた仕様: 毎フレーム呼ばれるためメモリ確保なしの前詰め書き込み（既存アップロードバッファを再利用）、上限はフレーム合計で管理し超過時はクラッシュさせず切り詰め

## セッション 2026-11-13 19:12（Release シーン巡回の左上に howToNextScene.png を表示）

ブランチ: `feature/release-carousel-next-scene-hint`

### 1. 実施内容

- `SceneScreenUi` に `PreloadReleaseCarouselUiTextures` / `DrawReleaseCarouselHowToNextSceneUi` を追加（`howToNextScene.png` を左上、余白 16px に等倍スプライト描画）
- `SceneReleaseCarousel::HasBuildUpEntry` を追加（巡回リストに登録された BuildUp シーン JSON か判定）
- `SceneBuildUp`: Release 構成（`!DEVELOPMENT && !USE_IMGUI`）でのみ、`EnterScene` で巡回対象か判定してテクスチャを事前読み込みし、`DrawScene` のパーティクル描画後に描画。フェードより前に描くため遷移時は一緒に暗転する
- 対象は `release_scene_carousel.json` に載っている ParticleDemo と BillboardInstancing100（両シーンで同じ位置）
- Release|x64・Development|x64 ビルド成功（実機での見た目確認は未実施）

### 2. 課題・設計上の選択肢

- **既存実装の課題点:** `SceneScreenUi` の UI 座標は `Window::kDefaultClientWidth/Height` 固定で、ウィンドウサイズ変更や解像度差に追従しない（左上配置の今回は影響なし）

## 推測した用途（§6.4）

- **シーン巡回の操作案内スプライト**
  - 用途: 配布ビルドでのデモ・展示時に、初見のプレイヤーへ「Space で次のシーンへ進める」ことを常時示す
  - 使用場面: ビルド・配布（Release のみ）。エディタ／Development の表示には出さない
  - 用途から決めた仕様: 巡回リストに載ったシーンだけに出す（リストを変えれば表示対象も追従）、テクスチャは描画中でなく EnterScene で読み込み、案内画像は差し替えやすいよう PNG ファイル名で指定

## セッション 2026-11-13 20:05（Release でも使う JSON を Assets へ移動）

### 1. 実施内容

- Release が読むシーン `ParticleDemo.json` / `BillboardInstancing100.json` を `Project/Assets/Scenes/` へ移動
- それらが参照するパーティクル `samples/01`〜`15` と `demos/ball_wall_shockwave.json` を `Project/Assets/Particles/` へ移動
- `launch_config.json`・`release_scene_carousel.json`・本編の `operator_guidance.json` も `Project/Assets/` へ移動し、パスを `./Assets/...` に更新
- 相対パーティクルパスの解決先を `GameAssets::kParticlesDirectory`（`./Assets/Particles`）に変更
- エディタのシーン一覧は `Assets/Scenes` と `EditorAssets/Scenes` の両方を表示。新規シーンの作成先はエディタ作業用の `EditorAssets/Scenes` のまま
- `editorTest.json` と `Scene_0.json` などエディタ専用ファイルは `EditorAssets` に残置

### 2. 課題・設計上の選択肢

- **既存実装の課題点:** `feature-memos/operator_guidance_bubble.md` とギャップスキャン資料は、案内 JSON の場所を `EditorAssets/UX/operator_guidance.json` のまま記載している

## 推測した用途（§6.4）

- **Release 同梱のシーン・パーティクル JSON**
  - 用途: 配布ビルドでのデモシーン再生、本編のパーティクル演出とオペレータ案内。制作中の下書きシーンはエディタ専用のまま分ける
  - 使用場面: ビルド・配布、ランタイムのゲームプレイ、レベル／アセット制作（エディタから同じ Assets を開いて調整）
  - 用途から決めた仕様: ゲームが読むパスは cwd（Project）からの `./Assets/...`。エディタ専用の試験 JSON と作業シーンは `../EditorAssets` のまま

## セッション 2026-11-13 20:15（ParticleDemo のサンプルを画面内の別位置で同時再生）

### 1. 実施内容

- `ParticleSampleCarouselSlots` を追加。`autoCarousel` かつ sample が複数のとき、ステージカメラに収まる格子へインデックスをずらして同時再生し、`carouselIntervalSeconds` ごとに窓を1つ進める
- 同時数はセルの短辺が 5 以上になる最大の矩形（15 件なら 5 列×2 行の 10 枠）
- `CmpBounceBall` の衝撃波は変更していない

### 2. 課題・設計上の選択肢

- **既存実装の課題点:** 噴水や半径の大きいバーストはセルをはみ出して隣と重なることがある。サンプル側の速度を変えない限り、15 件全部を画面内で完全に分離はできない

## 推測した用途（§6.4）

- **パーティクルサンプルの複数同時プレビュー**
  - 用途: デモ・展示で、表現カタログをカメラ操作なしに見比べる。転がるボールの演出は残す
  - 使用場面: ビルド・配布、レベル／アセット制作の確認
  - 用途から決めた仕様: 位置は既定ステージカメラの視野内に固定し、再生順は従来の sample 列をスロット分ずらす

## セッション 2026-11-13 21:31（複数レイヤーの衝撃パーティクル）

### 1. 実施内容

- `Project/Assets/Particles/samples/16_multilayer_impact_stack.json` を追加。1 ファイルの配列で 5 レイヤー（立ち上る煙、地面の輪、重力のかかる火花、長く残る残り火、短い閃光）
- `ParticleDemo.json` の `samplePaths` と `SAMPLES.md` に追加

### 2. 課題・設計上の選択肢

- なし

## 推測した用途（§6.4）

- **複数レイヤーの衝撃演出**
  - 用途: ヒットや着弾で、閃光・火花・煙を別々の寿命と見た目で同時に出す
  - 使用場面: ランタイムのゲームプレイ、レベル／アセット制作（パーティクルエディタと ParticleDemo で見比べる）
  - 用途から決めた仕様: 広がりはデモの枠に収まる程度に抑え、層ごとに `BURST`/`CONTINUOUS` と `BUILTIN_SPHERE`/`BILLBOARD` を分ける


---

## セッション 2026-11-18 07:06

ギャップスキャン 1 tick。scanId: SCAN-20261118-070405 / source: goal / scenarioId: UG-G-a668342c / newIds: 3

- 対象: 完成形の説明「ファイル内部の編集とは別の、ファイルの新規作成や削除、名前変更などが行えるようになっている」
- 追加: GAP-Editor-EditorSceneAsset-004（シーン JSON の改名が無い）、GAP-Editor-PrefabAsset-007（プレハブ JSON の削除と改名が無い）、GAP-Editor-EditorImportedAsset-005（取り込みファイルの削除と改名が無い）


---

## セッション 2026-11-18 07:15

ギャップスキャン 1 tick。scanId: SCAN-20261118-071408 / source: goal / scenarioId: UG-G-274394f0 / newIds: 2 / updatedIds: 1

- 対象: 完成形の説明「jsonなどインスペクタで簡易的に内部の編集が可能なファイルには自作エディタのインスペクタを使用する」
- 追加: GAP-Editor-EntityTagRegistry-001（tagRegistry.json は読込専用）、GAP-UX-OperatorGuidanceSystem-007（operator_guidance.json にインスペクタが無い）
- 更新: GAP-Editor-StageViewCameraTuning-001（カメラ tuning JSON もインスペクタ未接続のまま）


---

## セッション 2026-11-18 07:25

ギャップスキャン 1 tick。scanId: SCAN-20261118-072411 / source: goal / scenarioId: UG-G-9c2c9771 / newIds: 1

- 対象: 完成形の説明「png・obj・gltf のファイル内部の編集は外部ツールを使う」
- インスペクタはパスとプレビューだけで、画素や頂点は編集していない
- 追加: GAP-Editor-EntityEditorUi-032（png・obj・gltf を外部ツールで開く操作が無い）


---

## セッション 2026-11-18 07:35

ギャップスキャン 1 tick。scanId: SCAN-20261118-073408 / source: goal / scenarioId: UG-G-1fe965f0 / newIds: 1

- 対象: 完成形の説明「md や Lua などコード・テキストの本文は VS Code などの外部エディタで編集する」
- 自作エディタに複数行の本文編集は無い。Lua はテンプレート生成のみ
- 追加: GAP-Editor-EditorLuaScriptScaffold-001（生成した Lua や md を外部テキストエディタで開く操作が無い）


---

## セッション 2026-11-18 07:45

ギャップスキャン 1 tick。scanId: SCAN-20261118-074422 / source: goal / scenarioId: UG-G-088fd113 / newIds: 1

- 対象: 完成形の説明「シーン、オーディオ、入力、描画、物理（形状データを除く）が基本機能として独立している」
- CommandScene、AudioManager、PlayInput、AtrumEngine は独立した型がある
- 追加: GAP-Runtime-CmpLhRigidBody-001（物理演算が CmpLhRigidBody::Update に閉じ、共通の物理エンジンが無い）


---

## セッション 2026-11-18 07:55

ギャップスキャン 1 tick。scanId: SCAN-20261118-075408 / source: goal / scenarioId: UG-G-3f038476 / newIds: 1

- 対象: 完成形の説明「基本機能に外部クラスからアクセスできるようになっている」
- 時間・入力・オーディオ・テクスチャと CommandScene は外部から届く
- 追加: GAP-Engine-EngineAccess-001（描画は EngineInstance 経由で、EngineAccess に入り口が無い）


---

## セッション 2026-11-18 08:05

ギャップスキャン 1 tick。scanId: SCAN-20261118-080410 / source: goal / scenarioId: UG-G-b9f501e7 / newIds: 1

- 対象: 完成形の説明「機能として成り立つ最小単位の部品がそれぞれクラスに分けられ整理されている」
- 追加: GAP-Runtime-CmpBounceBall-001（運動・壁床バウンス・衝突演出が CmpBounceBall 1クラスに同居）
---

## セッション 2026-11-18 12:05

ギャップスキャンの未掲載分を roadmap_rough.md へ追記（重複する小見出しは追加しない）。

- 追加: 146（Authoring 51 / Debug 47 / Play 48 / Ship 0）
- 既存の小見出しと文言が一致したため未追加: 153
- 既存の中見出しへ 86 件、新しい中見出し 29 個
- 正本: implementation_gaps_index.json の open / investigating
- ループ: gap-to-rough、10 分間隔。次 tick も同じ基準

## セッション 2026-11-18 12:07

ループ手動停止。gap-to-rough（10 分間隔でギャップを roadmap_rough.md へ追記）を終了。

## セッション 2026-11-18 12:24

roadmap_rough.md を解釈して roadmap.md へマージ（重複する小見出しは追加しない）。取り込み後、下書きは roadmap_rough_old.md へ退避し、テンプレで roadmap_rough.md を再生成した。

- 追加: 256 件
- 既存と一致のため未追加: 42 件
- 在庫のまとめ行（`- [ ]` かつ「在庫」）は個別項目へ展開済みの注記に変えた

## 実装済みと判断した項目（コード確認・即時 [x]）

| roadmap の文言 | 根拠（パス・シーン・API） | 判定（万全／一部要確認） |
|----------------|------------------------------|--------------------------|
| PostBuild が Assets のみコピーしランタイム参照の EditorAssets が同梱されない | `Project/CG2_BuildUp.vcxproj` の Debug / Release / Development の PostBuild が EditorAssets を xcopy | 万全 |
| EditorScenePlayRequest がどこからも Set されない | `EditorSceneAsset.cpp` `TrySetRepositoryStartupScene` が `EditorScenePlayRequest::Set` を呼ぶ | 万全 |
| decoderBacklog01 のクランプで remaining 肥大が見えない | 既存の `- [x]` のまま。診断表が remaining バイトと 1 超の倍率を表示（`AudioStreamingDebugImGui.cpp`） | 万全 |

## 追加した節

- Camera: 6
- Editor: 78
- UX: 9
- System: 7
- Lighting: 2
- Debug: 59
- Repository: 6
- Scene: 41
- Audio: 47
- Loader: 1

配置は記号の役割で既存の大見出しへ寄せた。Authoring の制作作業は Editor、プレイ中のシーン挙動は Scene、BGM／SE は Audio、ライトは Lighting、ビルドと配布は Repository。

---

## セッション 2026-11-18 12:56（/loop 15m・roadmap tick）

### 1. 実施内容

- **着手ブランチ:** `fix/ChangeSceneFadeTransitionSe`
- **roadmap:** Audio › ChangeSceneFade — `GAP-Scene-ChangeSceneFade-001`（フェードイン開始時の遷移 SE）
- `PlaySceneTransitionSe()` を追加（`seSceneTransition` → 無ければ `seSubmit` にフォールバック）
- `ChangeSceneFade::Update` でフェードイン開始時に1回だけ再生
- **ビルド:** Cloud VM（Linux）のため `buildDevelopment.cmd` は未実行（Windows/MSBuild 前提）

### 2. 課題・設計上の選択肢

- BGM クロスフェード／継続は未着手（親トピック「シーン遷移時の BGM／SE…」は展開済み行のまま）

### 3. 次に実行すべきステップ

- roadmap Audio › ChangeSceneFade の次行: `timeRatio 満了で即 isSceneChange…`（`GAP-Scene-ChangeSceneFade-002` 相当）
- Windows 環境で Development ビルド確認後、`loopMergeTickToDevelop.ps1` で develop 統合

### 4. 実装済みと判断した項目（コード確認・即時 [x]）

- ChangeSceneFade フェードイン開始時の遷移 SE 再生（`GAP-Scene-ChangeSceneFade-001`）

### 5. 保留・着手見送り（§6.3）

- なし

### 6. 用途不明で確認待ち（§6.4）

- なし

### 7. 推測した用途（§6.4）

- **遷移フェード SE**
  - 用途: ランタイムのシーン切替で音の手応えを付け、無音フェードの違和感を減らす
  - 使用場面: プレイ（Title/Game/Result/BuildUp 間の `ChangeSceneFade` 経由遷移すべて）
  - 仕様: 専用 `seSceneTransition` が無くても既存 submit SE でフォールバックし、欠損時は `PlaySeCandidates` 同様に無音

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|------|------|------|
| ChangeSceneFade 遷移 SE | 有効 | 本編プレイの遷移体感向上。コア API 破壊なし |

---

## セッション 2026-11-18 13:01（/loop 15m・roadmap tick 2）

### 1. 実施内容

- **着手ブランチ:** `fix/ChangeSceneFadeFullBlackHold`（前 tick の `fix/ChangeSceneFadeTransitionSe` は develop へ fast-forward 済み）
- **roadmap:** Audio › ChangeSceneFade — `GAP-Scene-ChangeSceneFade-002`
- フェードアウト完了後 **0.08s** 真っ黒を維持してから `isSceneChange_` を立て、`EnterScene` を中間フレーム寄りからずらす
- フェード時間を名前付き定数に整理（挙動は従来の 0.5s / 0.25s 待ち / 0.25s イン）
- **ビルド:** Cloud VM（Linux）未実行

### 2. 課題・設計上の選択項

- 保持 0.08s は体感調整用。長くするとロード感が増える

### 3. 次に実行すべきステップ

- roadmap 次行: `フェード中盤で次 BGM が始まる`
- Windows で Development ビルド後に develop 統合

### 4. 実装済みと判断した項目（コード確認・即時 [x]）

- ChangeSceneFade のシーン入替タイミング（`GAP-Scene-ChangeSceneFade-002`）

### 5. 保留・着手見送り（§6.3）

- なし

### 6. 用途不明で確認待ち（§6.4）

- なし

### 7. 推測した用途（§6.4）

- **暗転ホールド**
  - 用途: プレイ中シーン遷移で、画面が完全に暗くなってから次シーンへ切り替え、チラつきを減らす
  - 使用場面: 全 `ChangeSceneFade` 遷移
  - 仕様: フェードアウト比率 1.0 到達だけでは切替せず短いホールドを挟む

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|------|------|------|
| ChangeSceneFade 入替タイミング | 有効 | プレイ UX。リスク低 |

---

## セッション 2026-11-18 13:16（/loop 15m・roadmap tick 3）

### 1. 実施内容

- **着手ブランチ:** `fix/ChangeSceneFadeBgmEarlyStart`
- **roadmap:** Audio › ChangeSceneFade — `GAP-Scene-ChangeSceneFade-003`
- `CommandChangeScene` + `PlayBgmLoop` で、演出付き遷移時は EnterScene の BGM を登録のみ行い、フェードイン完了（演出破棄）後に `PlaySceneBgm` を開始
- Title / Result / Game で `AssignPendingFadeSyncedSceneBgm` によりハンドルを同期
- **ビルド:** Cloud VM（Linux）未実行

### 2. 課題・設計上の選択項

- 演出なし即時遷移は従来どおり EnterScene で即 BGM 開始

### 3. 次に実行すべきステップ

- roadmap 次行: `Pause/Resume/Volume 付き BGM API がシーンから未使用`

### 4. 実装済みと判断した項目（コード確認・即時 [x]）

- ChangeSceneFade 遷移中の BGM 開始タイミング（`GAP-Scene-ChangeSceneFade-003`）

### 5. 保留・着手見送り（§6.3）

- なし

### 6. 用途不明で確認待ち（§6.4）

- なし

### 7. 推測した用途（§6.4）

- **フェード同期 BGM**
  - 用途: 本編シーン遷移で、暗転フェードインが明けるタイミングと次 BGM の頭を揃え、暗転中の不自然な先行再生を避ける
  - 使用場面: `ChangeSceneFade` 経由の Title↔Game↔Result
  - 仕様: 遷移演出終了時に保留パスへ `PlaySceneBgm`

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|------|------|------|
| ChangeSceneFade BGM 同期 | 有効 | PLY-Audio-001。音声レイヤのみ |

---

## セッション 2026-11-18 13:15（/loop 15m・roadmap tick 4）

### 1. 実施内容

- **着手ブランチ:** `fix/SceneBgmStatusApiUnused`
- **roadmap:** Audio › PauseBgmLoop — `GAP-Audio-PauseBgmLoop-001`
- `SyncSceneBgmForChangeSceneFade`（`PlaySe.h`）を追加し、演出中は `SetBgmLoopVolume` + `PauseBgmLoop`、通常時は `ResumeBgmLoop` + フル音量復帰
- Title / Result / Game の `Update` から呼び出し（Game は遷移早期 return 前）
- **ビルド:** Cloud VM（Linux）未実行

### 2. 課題・設計上の選択項

- ダック音量 0.12 は遷移 SE を前面に出す暫定値。クロスフェード本格化は別タスク

### 3. 次に実行すべきステップ

- roadmap 次行: `StopBgmLoop がシーン BGM 以外のストリーミングも停止`

### 4. 実装済みと判断した項目（コード確認・即時 [x]）

- Pause/Resume/Volume BGM API のシーン配線（`GAP-Audio-PauseBgmLoop-001`）

### 5. 保留・着手見送り（§6.3）

- なし

### 6. 用途不明で確認待ち（§6.4）

- なし

### 7. 推測した用途（§6.4）

- **遷移中 BGM ダック**
  - 用途: `ChangeSceneFade` 中に旧シーン BGM を下げつつ停止し、遷移 SE と次 BGM 開始がぶつかりにくくする
  - 使用場面: Title・Game・Result の演出付きシーン切替
  - 仕様: 演出指令 `GetIsExecute` と同期

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|------|------|------|
| BGM Pause/Resume/Volume 配線 | 有効 | プレイ中オーディオ管理。エンジン基本機能のシーン側利用 |

---

## セッション 2026-11-18 13:30（/loop 15m・roadmap tick 5）

### 1. 実施内容

- **着手ブランチ:** `fix/StopBgmLoopStreamingScope`
- **roadmap:** Audio › StopBgmLoop — `GAP-Audio-StopBgmLoop-001`
- `StopBgmLoop` から `StopAllStreamingVoices` / `StopAllLoopingShortVoices` を除去し、ハンドル指定の `StopSe` のみ。ハンドル空のときは `StopDedicatedSceneBgmVoice`（ボイス 0 のみ）
- **ビルド:** Cloud VM（Linux）未実行

### 2. 課題・設計上の選択項

- `PlaySceneBgm` 開始時の全停止は別 GAP（roadmap 次項）

### 3. 次に実行すべきステップ

- roadmap 次行: `PlaySceneBgm 開始時に全ボイスを一括停止`

### 4. 実装済みと判断した項目（コード確認・即時 [x]）

- StopBgmLoop の停止範囲（`GAP-Audio-StopBgmLoop-001`）

### 5. 保留・着手見送り（§6.3）

- なし

### 6. 用途不明で確認待ち（§6.4）

- なし

### 7. 推測した用途（§6.4）

- **選択的 BGM 停止**
  - 用途: シーン Exit で BGM だけ止め、同時再生中のストリーミング SE／ボイスは維持
  - 使用場面: Title/Game/Result の `ExitScene`
  - 仕様: 専用ボイス 0 のみフォールバック解放

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|------|------|------|
| StopBgmLoop スコープ縮小 | 有効 | オーディオマネージャー利用の本編プレイ |

---

## セッション 2026-11-18 13:45（/loop 15m・roadmap tick 6）

### 1. 実施内容

- **着手ブランチ:** `fix/PlaySceneBgmStopAllVoices`
- **roadmap:** Audio › PlaySceneBgm — `GAP-Audio-PlaySceneBgm-001`
- `PlaySceneBgm` 先頭の `StopAllStreamingVoices` / `StopAllLoopingShortVoices` をやめ、専用スロットの `StopDedicatedSceneBgmVoice` のみ（tick5 と同方針）
- ストリーミング BGM は従来どおりボイス 0 を `ResetStreamingVoice` してから開始
- **ビルド:** Cloud VM（Linux）未実行

### 2. 課題・設計上の選択項

- 短尺 BGM フォールバック失敗時の `StopAllLoopingShortVoices` は維持（プール逼迫時のみ）

### 3. 次に実行すべきステップ

- roadmap 次行: `Init 失敗で voice playId だけ巻き戻し`

### 4. 実装済みと判断した項目（コード確認・即時 [x]）

- PlaySceneBgm 開始時の停止範囲（`GAP-Audio-PlaySceneBgm-001`）

### 5. 保留・着手見送り（§6.3）

- なし

### 6. 用途不明で確認待ち（§6.4）

- なし

### 7. 推測した用途（§6.4）

- **BGM 再開と SE 共存**
  - 用途: `MaintainSceneBgmIfNeeded` 等で BGM だけ差し替え／再開し、弾幕・ヒット等のストリーミング SE を切らない
  - 使用場面: 本編 `SceneGame` プレイ中
  - 仕様: シーン BGM スロットのみ事前解放

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|------|------|------|
| PlaySceneBgm 停止範囲縮小 | 有効 | 基本機能オーディオの本編利用 |

---

## セッション 2026-11-18 14:00（/loop 15m・roadmap tick 7）

### 1. 実施内容

- **着手ブランチ:** `fix/PlaySceneBgmInitPlayIdRollback`
- **roadmap:** Audio › PlaySceneBgm — `GAP-Audio-PlaySceneBgm-002`
- `InitializeStreaming` 失敗時は `ResetStreamingVoice` し、返却 `AudioHandle.playId` をボイス側（65536）と一致させる
- **ビルド:** Cloud VM（Linux）未実行

### 2. 課題・設計上の選択項

- `PlayStreaming` 側の同種不整合は別 GAP（roadmap 76 行付近）

### 3. 次に実行すべきステップ

- roadmap 次行: `PlayBgmLoop 失敗時に PlaySceneBgm を再実行`

### 4. 実装済みと判断した項目（コード確認・即時 [x]）

- PlaySceneBgm Init 失敗時ハンドル整合（`GAP-Audio-PlaySceneBgm-002`）

### 5. 保留・着手見送り（§6.3）

- なし

### 6. 用途不明で確認待ち（§6.4）

- なし

### 7. 推測した用途（§6.4）

- **BGM 開始失敗の復帰**
  - 用途: デコーダ不調時に `PlayBgmLoop` 再試行・`StopBgmLoop` が正しい playId で動く
  - 使用場面: シーン入場・MaintainSceneBgmIfNeeded
  - 仕様: 失敗ハンドルは無効 playId と一致

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|------|------|------|
| PlaySceneBgm 失敗ハンドル | 有効 | プレイ中 BGM 継続。矛盾なし |

---

## セッション 2026-11-18 14:15（/loop 15m・roadmap tick 8）

### 1. 実施内容

- **着手ブランチ:** `fix/PlayBgmLoopRetrySceneBgm`
- **roadmap:** Audio › PlayBgmLoop — `GAP-Audio-PlayBgmLoop-001`
- `PlayBgmLoopCore` から二重 `PlaySceneBgm` を削除。再試行は `PlaySceneBgm` 内で `InitializeStreaming` を最大 2 回（専用ボイス再セットのみ）
- **ビルド:** Cloud VM（Linux）未実行

### 2. 課題・設計上の選択項

- 二重呼び出し時の `StopDedicatedSceneBgmVoice` 連打を避け、他 SE との共存を改善

### 3. 次に実行すべきステップ

- roadmap 次行: `ResumeBgmLoop がシーンから呼ばれない`（tick4 で Sync に配線済みか要確認）

### 4. 実装済みと判断した項目（コード確認・即時 [x]）

- PlayBgmLoop の二重 PlaySceneBgm（`GAP-Audio-PlayBgmLoop-001`）

### 5. 保留・着手見送り（§6.3）

- なし

### 6. 用途不明で確認待ち（§6.4）

- なし

### 7. 推測した用途（§6.4）

- **BGM 再開試行**
  - 用途: MaintainSceneBgmIfNeeded 経由の再開で戦闘ストリーム SE を二重切断しない
  - 使用場面: SceneGame プレイ中
  - 仕様: 1 回の PlaySceneBgm 呼び出し内で完結する再試行

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|------|------|------|
| PlayBgmLoop 再試行整理 | 有効 | オーディオ本編プレイ |

---

## セッション 2026-11-18 14:30（/loop 15m・roadmap tick 9）

### 1. 実施内容

- **着手ブランチ:** `fix/ResumeBgmLoopSceneCall`
- **roadmap:** Audio › ResumeBgmLoop — `GAP-Audio-ResumeBgmLoop-001`
- **§6.2 コード確認:** tick 4 で追加した `SyncSceneBgmForChangeSceneFade` が `ResumeBgmLoop` を呼び、`SceneTitle` / `SceneGame` / `SceneResult` の `Update` から参照済み。新規 diff なし
- **ビルド:** 不要（ドキュメントのみ）

### 2. 課題・設計上の選択項

- なし

### 3. 次に実行すべきステップ

- roadmap 次行: `SetBgmLoopVolume が未使用`（同様に Sync 経由で確認）

### 4. 実装済みと判断した項目（コード確認・即時 [x]）

- ResumeBgmLoop のシーン経由配線（`GAP-Audio-ResumeBgmLoop-001`）

### 5. 保留・着手見送り（§6.3）

- なし

### 6. 用途不明で確認待ち（§6.4）

- なし

### 7. 推測した用途（§6.4）

- **フェード後 BGM 復帰**（tick 4 と同旨）
  - 使用場面: ChangeSceneFade 終了後、通常 Update で音量・再生再開

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|------|------|------|
| ResumeBgmLoop 配線確認 | 有効 | 本編シーン遷移オーディオ |

---

## セッション 2026-11-18 14:45（/loop 15m・roadmap tick 10）

### 1. 実施内容

- **着手ブランチ:** `fix/SetBgmLoopVolumeUnused`
- **roadmap:** Audio › SetBgmLoopVolume — `GAP-Audio-SetBgmLoopVolume-001`
- **§6.2 コード確認:** `SyncSceneBgmForChangeSceneFade` が遷移中に `SetBgmLoopVolume(0.12)`、通常時に `1.0f` を設定。Title / Game / Result の `Update` から呼び出し済み。新規 diff なし
- **ビルド:** 不要

### 2. 課題・設計上の選択項

- なし

### 3. 次に実行すべきステップ

- roadmap 次行: `PlaySeLoop がコードベースで未呼び出し`

### 4. 実装済みと判断した項目（コード確認・即時 [x]）

- SetBgmLoopVolume のシーン経由利用（`GAP-Audio-SetBgmLoopVolume-001`）

### 5. 保留・着手見送り（§6.3）

- なし

### 6. 用途不明で確認待ち（§6.4）

- なし

### 7. 推測した用途（§6.4）

- **フェード中 BGM ダック音量**（tick 4 と同旨）

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|------|------|------|
| SetBgmLoopVolume 配線確認 | 有効 | 遷移オーディオ |

---

## セッション 2026-11-18 15:00（/loop 15m・roadmap tick 11）

### 1. 実施内容

- **着手ブランチ:** `fix/PlaySeLoopUnwired`
- **roadmap:** Audio › PlaySeLoop — `GAP-Audio-PlaySeLoop-001`
- `PlaySeLoopCandidates` を追加。`SceneTitle::EnterScene` でタイトル環境ループ SE（`seTitleAmbientLoop.mp3` 候補）、`ExitScene` で `StopSe`
- **ビルド:** Cloud VM（Linux）未実行

### 2. 課題・設計上の選択項

- 音源ファイル未同梱時は無音（他 SE と同様の存在チェック）

### 3. 次に実行すべきステップ

- roadmap 次行: `空き無しで全汎用ストリームをリセット`

### 4. 実装済みと判断した項目（コード確認・即時 [x]）

- PlaySeLoop のシーン呼び出し（`GAP-Audio-PlaySeLoop-001`）

### 5. 保留・着手見送り（§6.3）

- なし

### 6. 用途不明で確認待ち（§6.4）

- なし

### 7. 推測した用途（§6.4）

- **タイトル環境ループ層**
  - 用途: BGM とは別ボイスで微量の環境音をループ（風・基地アンビエンス等）
  - 使用場面: タイトル画面
  - 仕様: 候補パスが無ければ再生しない

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|------|------|------|
| PlaySeLoop 配線 | 有効 | プレイオーディオ層の拡充 |

---

## セッション 2026-11-18 15:15（/loop 15m・roadmap tick 12）

### 1. 実施内容

- **着手ブランチ:** `fix/StreamingVoicePoolReset`
- **roadmap:** Audio › AudioManager — `GAP-Audio-AudioManager-003`
- `AcquireStreamingVoiceIndex` の枯渇処理を、汎用ボイス全リセットから **最古 playId 1 本退避** に変更（voice 0 は従来どおり除外）
- **ビルド:** Cloud VM（Linux）未実行

### 2. 課題・設計上の選択項

- 優先度は playId のみ。カテゴリ別優先度は未実装

### 3. 次に実行すべきステップ

- roadmap 次行: `BGM 一時停止中は復帰判定が誤る`

### 4. 実装済みと判断した項目（コード確認・即時 [x]）

- ストリーミング SE プール枯渇時の単一退避（`GAP-Audio-AudioManager-003`）

### 5. 保留・着手見送り（§6.3）

- なし

### 6. 用途不明で確認待ち（§6.4）

- なし

### 7. 推測した用途（§6.4）

- **同時ストリーム SE**
  - 用途: 弾幕等でプールが埋まっても他 SE を一斉切断せず、最古 1 本だけ切る
  - 使用場面: 本編プレイの長尺ストリーム SE
  - 仕様: BGM 専用 slot 0 非奪取

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|------|------|------|
| ストリーム退避方針 | 有効 | オーディオ基本機能 |

---

## セッション 2026-11-18 15:30（/loop 15m・roadmap tick 13）

### 1. 実施内容

- **着手ブランチ:** `fix/BgmPausedResumeJudgement`
- **roadmap:** Audio › AudioManager — `GAP-Audio-AudioManager-001`
- `IsSceneBgmAudible` / `IsSceneBgmSlotHeld` を追加。`MaintainSceneBgmIfNeeded` で一時停止を検知し、遷移演出外なら `ResumeBgmLoop` + 音量復帰
- `IsSceneBgmPaused` で `state.load()` を使用
- **ビルド:** Cloud VM（Linux）未実行

### 2. 課題・設計上の選択項

- 遷移中の一時停止は `CommandChangeScene::GetIsExecute` で Resume を抑制（`Sync` と役割分担）

### 3. 次に実行すべきステップ

- roadmap 次行: `Resume は Start のみでデコード再開なし`（`GAP-Audio-AudioManager-004`）

### 4. 実装済みと判断した項目（コード確認・即時 [x]）

- BGM 一時停止時の維持判定（`GAP-Audio-AudioManager-001`）

### 5. 保留・着手見送り（§6.3）

- なし

### 6. 用途不明で確認待ち（§6.4）

- なし

### 7. 推測した用途（§6.4）

- **BGM 復帰**
  - 用途: フェード以外で Pause された BGM を本編 Update で再開
  - 使用場面: SceneGame
  - 仕様: 無音のまま `IsPlaying` だけ true になる状態を解消

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|------|------|------|
| BGM 復帰判定 | 有効 | 本編プレイ音声 |

---

## セッション 2026-11-18 15:45（/loop 15m・roadmap tick 14）

### 1. 実施内容

- **着手ブランチ:** `fix/ResumeSceneBgmRefill`
- **roadmap:** Audio › AudioManager — `GAP-Audio-AudioManager-004`
- ストリーム BGM の `ResumeSceneBgm` で `FlushSourceBuffers` → `SubmitInitialBuffer` → `Start` → `RefillBuffer`（シーク復帰と同系）
- **ビルド:** Cloud VM（Linux）未実行

### 2. 課題・設計上の選択項

- 短尺 BGM（非ストリーム）は従来どおり Start のみ

### 3. 次に実行すべきステップ

- roadmap 次行: `StopAll は BGM 系ループのみ`

### 4. 実装済みと判断した項目（コード確認・即時 [x]）

- ResumeSceneBgm のデコード再開（`GAP-Audio-AudioManager-004`）

### 5. 保留・着手見送り（§6.3）

- なし

### 6. 用途不明で確認待ち（§6.4）

- なし

### 7. 推測した用途（§6.4）

- **フェード／ポーズ後 BGM 再開**
  - 用途: Pause 後の Resume でストリームキューを再構築し無音化を防ぐ
  - 使用場面: ChangeSceneFade 同期・Maintain 復帰

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|------|------|------|
| ResumeSceneBgm Refill | 有効 | オーディオ基本機能 |

---

## セッション 2026-11-18 16:00（/loop 15m・roadmap tick 15）

### 1. 実施内容

- **着手ブランチ:** `fix/StopAllBgmLoopsOnly`
- **roadmap:** Audio › AudioManager — `GAP-Audio-AudioManager-002`
- `SourceVoice::isSceneBgmSlot` を追加。`PlaySceneBgm` 短尺経路のみフラグ。`StopAllLoopingShortVoices` は当該スロットのみ解放
- **ビルド:** Cloud VM（Linux）未実行

### 2. 課題・設計上の選択項

- シーン Exit で戦闘ワンショット SE を一括停止する API は未追加（別 GAP）

### 3. 次に実行すべきステップ

- roadmap 次行: `Paused ストリームは Update をスキップ`

### 4. 実装済みと判断した項目（コード確認・即時 [x]）

- StopAll 相当の BGM 限定化（`GAP-Audio-AudioManager-002`）

### 5. 保留・着手見送り（§6.3）

- なし

### 6. 用途不明で確認待ち（§6.4）

- なし

### 7. 推測した用途（§6.4）

- **短尺 BGM と環境ループ SE 分離**
  - 用途: PlaySceneBgm 逼迫時にタイトル環境 `PlaySeLoop` を止めない
  - 使用場面: 短尺 BGM フォールバック・プール解放

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|------|------|------|
| StopAll BGM 限定 | 有効 | プレイオーディオ |

---

## セッション 2026-11-18 16:15（/loop 15m・roadmap tick 16）

### 1. 実施内容

- **着手ブランチ:** `fix/StreamingPausedUpdateSkip`
- **roadmap:** Audio › AudioManager — `GAP-Audio-AudioManager-005`
- 一時停止中のループストリームでも `AudioManager::Update` が `RefillBuffer` を実行しキューを枯らさない（再生／Waiting 処理は従来どおりスキップ）
- **ビルド:** Cloud VM（Linux）未実行

### 2. 課題・設計上の選択項

- 非ループ一時停止は対象外（短尺ワンショット）

### 3. 次に実行すべきステップ

- roadmap 次行: `Handle と voice の playId がずれる`

### 4. 実装済みと判断した項目（コード確認・即時 [x]）

- Paused ストリームのバッファ補充（`GAP-Audio-AudioManager-005`）

### 5. 保留・着手見送り（§6.3）

- なし

### 6. 用途不明で確認待ち（§6.4）

- なし

### 7. 推測した用途（§6.4）

- **BGM ポーズ中キュー維持**
  - 用途: フェード Pause 中もデコード先を溜め、Resume と組み合わせて無音化を防ぐ
  - 使用場面: シーン BGM ストリーミング

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|------|------|------|
| Paused 時 Refill | 有効 | オーディオ基本機能 |

---

## セッション 2026-11-18 16:30（/loop 15m・roadmap tick 17）

### 1. 実施内容

- **着手ブランチ:** `fix/AudioHandlePlayIdMismatch`
- **roadmap:** Audio › AudioManager — `GAP-Audio-AudioManager-007`
- `PlayStreaming` の Init 失敗時に `ResetStreamingVoice` し、返却 `playId` をボイスと一致（`PlaySceneBgm-002` と同方針）
- **ビルド:** Cloud VM（Linux）未実行

### 2. 課題・設計上の選択項

- なし

### 3. 次に実行すべきステップ

- roadmap 次行: `Start 後に Refill を 1 回だけ`

### 4. 実装済みと判断した項目（コード確認・即時 [x]）

- PlayStreaming 失敗ハンドル（`GAP-Audio-AudioManager-007`）

### 5. 保留・着手見送り（§6.3）

- なし

### 6. 用途不明で確認待ち（§6.4）

- なし

### 7. 推測した用途（§6.4）

- **長尺ストリーム SE**
  - 用途: Init 失敗後の Stop/IsPlaying が空振りにならない
  - 使用場面: 戦闘等の `PlayStreaming`

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|------|------|------|
| playId 契約 | 有効 | オーディオ API |

---

## セッション 2026-11-18 16:45（/loop 15m・roadmap tick 18）

### 1. 実施内容

- **着手ブランチ:** `fix/StreamingStartSingleRefill`
- **roadmap:** Audio › AudioManager — `GAP-Audio-AudioManager-009`（併せ roadmap 72 行 §6.2）
- `PrefillStreamingQueueAfterStart` で `kBufferCount-1` 回 Refill。`InitializeStreaming` / `ResumeSceneBgm` / シーク復帰で使用
- **ビルド:** Cloud VM（Linux）未実行

### 2. 課題・設計上の選択項

- Refill 失敗時は途中までのプリロードで継続

### 3. 次に実行すべきステップ

- roadmap 次行: `BGM フォールバックで短音ボイス解放`

### 4. 実装済みと判断した項目（コード確認・即時 [x]）

- ストリーム開始プリロード（`GAP-Audio-AudioManager-009`）
- Resume 後 Refill（§6.2）

### 5. 保留・着手見送り（§6.3）

- なし

### 6. 用途不明で確認待ち（§6.4）

- なし

### 7. 推測した用途（§6.4）

- **ストリーム BGM/SE 開始**
  - 用途: 再生開始直後のアンダーランを減らす
  - 使用場面: トリプルバッファ前提のストリーミング

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|------|------|------|
| 開始時プリロード | 有効 | オーディオ |

---

## セッション 2026-11-18 17:00（/loop 15m・roadmap tick 19）

### 1. 実施内容

- **着手ブランチ:** `fix/SceneBgmShortVoiceRelease`
- **roadmap:** Audio › AudioManager — `GAP-Audio-AudioManager-008`
- `PlaySceneBgm` 短尺フォールバック再試行から `ReleaseWaitingShortVoices` を除去。シーン BGM スロットのみ `StopAllLoopingShortVoices`
- **ビルド:** Cloud VM（Linux）未実行

### 2. 課題・設計上の選択項

- プールが完全に埋まった場合の奪取は従来の `PlayShort` 探索に委ねる

### 3. 次に実行すべきステップ

- roadmap 次行: `ストリーム BGM 停止は XAudio Stop`

### 4. 実装済みと判断した項目（コード確認・即時 [x]）

- BGM 短尺フォールバック時の解放範囲（`GAP-Audio-AudioManager-008`）

### 5. 保留・着手見送り（§6.3）

- なし

### 6. 用途不明で確認待ち（§6.4）

- なし

### 7. 推測した用途（§6.4）

- **シーン遷移直後の SE 保護**
  - 用途: 短尺 BGM 開始失敗時の再試行で Waiting ワンショット SE を切らない
  - 使用場面: 遷移直後の戦闘 SE

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|------|------|------|
| 短音解放範囲 | 有効 | プレイ SE 優先 |

---

## セッション 2026-11-18 17:15（/loop 15m・roadmap tick 20）

### 1. 実施内容

- **着手ブランチ:** `fix/StreamBgmXaudioStop`
- **roadmap:** Audio › AudioManager — `GAP-Audio-AudioManager-010`
- `PauseSceneBgm` でストリーム BGM 一時停止時に `isStreaming` を落とし、`Stop(0, XAUDIO2_COMMIT_NOW)` で即時停止。デコーダは維持（tick16 Refill と Resume 再構築と連携）
- **ビルド:** Cloud VM（Linux）未実行

### 2. 課題・設計上の選択項

- 完全停止は従来 `ResetStreamingVoice` / `Stop(handle)` 経路

### 3. 次に実行すべきステップ

- roadmap 次行: `再生位置計算が 44100 固定`

### 4. 実装済みと判断した項目（コード確認・即時 [x]）

- ストリーム BGM ソフトポーズ（`GAP-Audio-AudioManager-010`）

### 5. 保留・着手見送り（§6.3）

- なし

### 6. 用途不明で確認待ち（§6.4）

- なし

### 7. 推測した用途（§6.4）

- **フェード用 Pause**
  - 用途: XAudio 停止のみでスロットを保持し、復帰時のクリック低減
  - 使用場面: ChangeSceneFade 同期

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|------|------|------|
| BGM ソフト Pause | 有効 | 本編オーディオ |

---

## セッション 2026-11-18 17:18（ループ停止・ユーザー指示）

- **ループ:** `loop-roadmap-execute`（15 分間隔）を **停止**（timer `sub_b2a62ebd-a8f8-431f-a93f-ae669a992400` unsubscribe 済み）
- **直前:** roadmap tick 20 完了（`develop` `dbff9f9`）。Audio セクションの未完了 `- [ ]` は継続あり
- **次の再開時:** roadmap 先頭の `- [ ]`（例: `再生位置計算が 44100 固定`）。作業ブランチ `fix/StreamBgmSampleRateFix` は空のまま push 済み
- **状態:** 半自律ロードマップループ終了。新規 tick は行わない

---

## セッション 2026-11-18 17:45（/loop 15m・roadmap tick）

### 1. 現在の進捗状況と完了したこと

- ユーザー指示で 15 分間隔のロードマップループを再開（`loopRoadmapDev.ps1`、ループ ID `roadmap-dev`）
- **着手ブランチ:** `fix/StreamPlaybackSampleRate`
- **roadmap:** Audio › AudioManager — `再生位置計算が 44100 固定`
- 再生秒換算を `TryPlaybackSecondsFromSamples` に分離。ストリーミング BGM とストリーミング診断は `SoundData::wfEx.nSamplesPerSec`（SourceVoice の PCM レート）で秒にする。レート 0 は失敗とし、44100 へ置き換えない
- **ビルド:** `buildDevelopment.cmd`（Development | x64）成功。ランタイム再生確認は未実施

### 2. 発生した課題または設計上の選択肢

- デコーダ出力と `SetupStreaming` の 44.1kHz 固定は別の `- [ ]`。`wfEx` だけをファイル本来のレートに変えると、`SamplesPlayed` の単位とずれて再生秒が壊れる
- 短尺ボイスの再生秒は従来どおり `SamplesPlayed / nSamplesPerSec`（開始時刻は足さない）

### 3. 次に実行すべきステップの提案

- roadmap 次行: `Waiting 非ループは Stop で終了`（次ブランチ `fix/WaitingNonLoopStop`）
- 人間のレビュー可。ループは 15 分後に次 tick

### 4. 実装済みと判断した項目（コード確認・即時 [x]）

- なし

### 5. 保留・着手見送り（§6.3）

- なし

### 6. 用途不明で確認待ち（§6.4）

- なし

### 7. 推測した用途（§6.4）

- **再生位置計算が 44100 固定**
  - 用途: シーン BGM の一時停止・復帰・シークで使う再生秒を、鳴っている PCM のレートに合わせる
  - 使用場面: ランタイム（ストリーミング BGM）。開発用 GUI のストリーミング診断も同じ換算
  - 用途から決めた仕様: サンプルレートは `SoundData::wfEx`（ボイス作成時の PCM）。0 なら秒を作らず失敗にする

### 8. 最終目標との査定（§6.7）

| タスク | 判定（有効／矛盾／寄与なし／査定不能） | 根拠（完成形の説明の・行） |
|--------|----------------------------------------|------------------------------|
| 再生位置計算が 44100 固定 | 有効 | ・bgmとしてのループ再生ができる / ・サイズが大きい音源ファイルについてストリーミング再生ができる |

- §6.1: 固定 44100 による割り算をやめ、ボイス PCM レートで再生秒を出す。デコーダの出力レート変更は対象外

---

## セッション 2026-11-18 18:50（/loop 15m・roadmap tick）

### 1. 現在の進捗状況と完了したこと

- **着手ブランチ:** `fix/WaitingNonLoopStop`（前 tick のマージで develop から作成済み。未コミットは `lease.md` と `loop_state.json` のみだったため再作成はしていない）
- **roadmap:** Audio › AudioManager — `Waiting 非ループは Stop で終了`
- 非ループのストリームが Waiting でも、キューが残っている間と、空キューの再読込が `kNonLoopEmptyRefillStopMisses`（3）未満の間は Stop しない。失敗が続いたときだけ従来どおり停止する
- 短尺ボイスの Waiting 即停止と、ループの `TryRecoverLoopingStreamingVoice` はそのまま
- **ビルド:** `buildDevelopment.cmd`（Development | x64）成功。ランタイム再生確認は未実施

### 2. 発生した課題または設計上の選択肢

- 再読込の失敗回数はボイスに持つ。真の終端と一時的な読み失敗は `ReadNextChunk` の成否だけでは区別できない
- 3 フレームで止めるので、ディスク待ちがそれより長いと長尺 SE はまだ切れる

### 3. 次に実行すべきステップの提案

- roadmap 次行: `ボイス枯渇で assert`（次ブランチ `fix/VoicePoolExhaustAssert`）
- ループは 15 分間隔のまま

### 4. 実装済みと判断した項目（コード確認・即時 [x]）

- なし

### 5. 保留・着手見送り（§6.3）

- なし

### 6. 用途不明で確認待ち（§6.4）

- なし

### 7. 推測した用途（§6.4）

- **Waiting 非ループは Stop で終了**
  - 用途: 長い効果音のストリーミングが、読み込みの一過性の失敗で途中から無音にならない
  - 使用場面: ランタイム
  - 用途から決めた仕様: キュー残は再生継続。空キューの再読込は 3 回まで。その後はボイスを Stop してプールへ返す

### 8. 最終目標との査定（§6.7）

| タスク | 判定（有効／矛盾／寄与なし／査定不能） | 根拠（完成形の説明の・行） |
|--------|----------------------------------------|------------------------------|
| Waiting 非ループは Stop で終了 | 有効 | ・サイズが大きい音源ファイルについてストリーミング再生ができる |

- §6.1: 非ループ Waiting の即 Stop をやめ、空キューの再読込失敗が続いたときだけ終了する

---

## セッション 2026-11-18 19:00（/loop 15m・roadmap tick）

### 1. 現在の進捗状況と完了したこと

- **着手ブランチ:** `fix/VoicePoolExhaustAssert`（前 tick のマージで develop から作成済み）
- **roadmap:** Audio › AudioManager — `ボイス枯渇で assert`
- `PlayShort` は Waiting 解放の再試行後も空きが無いとき、`MakeUnavailableShortVoiceHandle` で空の Handle を返す。Development の `assert` は外した
- ストリーミングプール満杯の `assert` は別行のまま
- **ビルド:** `buildDevelopment.cmd`（Development | x64）成功。ランタイムでの SE 枯渇確認は未実施

### 2. 発生した課題または設計上の選択肢

- 枯渇時は音を欠落させる。どの SE を止めて空きを作るかは今回決めていない
- 空 Handle は成功時と区別できる（`isSuccess == false`）。呼び出し側の無音は従来の Release と同じ

### 3. 次に実行すべきステップの提案

- roadmap 次行: `Init 失敗で playId 不一致`（次ブランチ `fix/StreamInitPlayIdMismatch`）
- ループは 15 分間隔のまま

### 4. 実装済みと判断した項目（コード確認・即時 [x]）

- なし

### 5. 保留・着手見送り（§6.3）

- なし

### 6. 用途不明で確認待ち（§6.4）

- なし

### 7. 推測した用途（§6.4）

- **ボイス枯渇で assert**
  - 用途: シーン遷移直後など SE が重なって短尺ボイスが足りなくても、プレイと既に鳴っている BGM を止めない
  - 使用場面: ランタイム（Development を含む）
  - 用途から決めた仕様: プール枯渇は失敗 Handle。プロセスは落とさない

### 8. 最終目標との査定（§6.7）

| タスク | 判定（有効／矛盾／寄与なし／査定不能） | 根拠（完成形の説明の・行） |
|--------|----------------------------------------|------------------------------|
| ボイス枯渇で assert | 有効 | ・bgmとしてのループ再生ができる |

- §6.1: 短尺ボイス枯渇の assert をやめ、空 Handle でその SE だけ欠落させる

---

## セッション 2026-11-18 19:55（/loop 15m・roadmap tick）

### 1. 現在の進捗状況と完了したこと

- **着手ブランチ:** `fix/StreamInitPlayIdMismatch`（前 tick のマージで develop から作成済み）
- **roadmap:** Audio › AudioManager — `Init 失敗で playId 不一致`
- §6.2: `PlayStreaming` の失敗 Handle は `ResetStreamingVoice` のあと `streamingVoice->playId` を入れる。ボイス側も同じ値（65536）なので、`Stop` は voiceIndex と playId の両方で同じスロットだけを見る
- コード変更なし。ビルドはしていない

### 2. 発生した課題または設計上の選択肢

- 失敗後の playId は全リセットボイス共通の 65536。再生中のボイスには付かない。別スロットの再生を止めることはない
- ギャップ本文の「Handle は currentPlayId のまま」は現行コードと違う

### 3. 次に実行すべきステップの提案

- roadmap 次行: `ストリーム PCM を 44.1k 固定`（次ブランチ `fix/StreamPcmSampleRate`）
- ループは 15 分間隔のまま

### 4. 実装済みと判断した項目（コード確認・即時 [x]）

| roadmap の文言 | 根拠（パス・シーン・API） | 判定（万全／一部要確認） |
|----------------|---------------------------|--------------------------|
| Init 失敗で playId 不一致 | `Audio.cpp` `PlayStreaming` 失敗時は `ResetStreamingVoice` 後の `playId` を Handle にコピー。`Stop` は同一 `voiceIndex` かつ同一 `playId` | 万全 |

### 5. 保留・着手見送り（§6.3）

- なし

### 6. 用途不明で確認待ち（§6.4）

- なし

### 7. 推測した用途（§6.4）

- なし

### 8. 最終目標との査定（§6.7）

| タスク | 判定（有効／矛盾／寄与なし／査定不能） | 根拠（完成形の説明の・行） |
|--------|----------------------------------------|------------------------------|
| Init 失敗で playId 不一致 | 有効 | ・サイズが大きい音源ファイルについてストリーミング再生ができる |

- §6.1: 失敗 Handle とボイスの playId は既に一致しているため実装せず `[x]`

---

## セッション 2026-11-18 20:15（/loop 15m・roadmap tick）

### 1. 現在の進捗状況と完了したこと

- **着手ブランチ:** `fix/StreamPcmSampleRate`（前 tick のマージで develop から作成済み）
- **roadmap:** Audio › AudioManager — `ストリーム PCM を 44.1k 固定`
- `BuildStreamPcmFormat` が音源の `MF_MT_AUDIO_SAMPLES_PER_SECOND` でステレオ 16bit PCM を組む。`PrepareStreamingDecoder` はそのレートでデコードし、`RecreateStreamingSourceVoice` で SourceVoice を合わせる。`SoundData::wfEx` もその PCM に更新する
- チャンネル数とビット深度はステレオ 16bit のまま。レートが取れない音源はストリーム開始を失敗にする
- **ビルド:** `buildDevelopment.cmd`（Development | x64）成功。ランタイムでの 48kHz 再生確認は未実施

### 2. 発生した課題または設計上の選択肢

- `SetupStreaming` が最初に置く 44.1kHz は、デコーダを開いたときに上書きする。初期値そのものは次の行
- 短尺 `AudioDecoder::LoadAudio` の 44.1kHz 固定はこの行の対象外

### 3. 次に実行すべきステップの提案

- roadmap 次行: `SetupStreaming が 44.1kHz 固定`（次ブランチ `fix/SetupStreamingSampleRate`）
- ループは 15 分間隔のまま

### 4. 実装済みと判断した項目（コード確認・即時 [x]）

- なし

### 5. 保留・着手見送り（§6.3）

- なし

### 6. 用途不明で確認待ち（§6.4）

- なし

### 7. 推測した用途（§6.4）

- **ストリーム PCM を 44.1k 固定**
  - 用途: 44.1kHz 以外の BGM や長い効果音を、音程を変えずにストリーミング再生する
  - 使用場面: ランタイム
  - 用途から決めた仕様: サンプルレートだけ音源に合わせ、ボイスも同じ PCM で作り直す。形はステレオ 16bit

### 8. 最終目標との査定（§6.7）

| タスク | 判定（有効／矛盾／寄与なし／査定不能） | 根拠（完成形の説明の・行） |
|--------|----------------------------------------|------------------------------|
| ストリーム PCM を 44.1k 固定 | 有効 | ・サイズが大きい音源ファイルについてストリーミング再生ができる / ・mp4やwavなど代表的な拡張子の音源ファイルを読み込める |

- §6.1: デコーダ出力と SourceVoice のサンプルレートを音源の値にする。SetupStreaming の初期代入は別行
---

## セッション 2026-11-19 18:43（/loop 15m・roadmap tick）

### 1. 現在の進捗状況と完了したこと

- **ループ:** `loop-roadmap-execute`（15m、`sub_3bea8bd0-499d-4cd6-8d1c-390c91ed130b`）継続
- **着手ブランチ:** `fix/SetupStreamingSampleRate`（`develop` から新規）
- **roadmap:** Audio › AudioManager — `SetupStreaming が 44.1kHz 固定`
- `ProbeStreamPcmFormatFromUrl` で MF のネイティブサンプルレートを読み、`SetupStreaming` の `SoundData::wfEx` に反映。取得失敗時のみ `StandardWaveFormatEx()` にフォールバック（`PrepareStreamingDecoder` で再更新される）
- **ビルド:** Cloud VM 上では `buildDevelopment.cmd` 未実行（Windows ツールチェーン必須）。CI で確認待ち

### 2. 発生した課題または設計上の選択肢

- プローブは `Load` 時の1回のみ。キャッシュヒットで再取得しない件は次行 `Load ヒットで再判定なし`
- プローブ失敗時の 44.1kHz は大容量ファイルのロード遅延を避けるためのプレースホルダとして残す

### 3. 次に実行すべきステップの提案

- roadmap 次行: `Load ヒットで再判定なし`（次ブランチ `fix/LoadHitNoWfReprobe`）
- Development ビルドと 48kHz ストリーム BGM の手動確認（任意）

### 4. 実装済みと判断した項目（コード確認・即時 [x]）

- なし

### 5. 保留・着手見送り（§6.3）

- なし

### 6. 用途不明で確認待ち（§6.4）

- なし

### 7. 推測した用途（§6.4）

- **SetupStreaming が 44.1kHz 固定**
  - 用途: 大きい BGM を `Load` 直後から、再生前の参照（インデックス・デバッグ表示・再生秒の仮レート）に実ファイルのレートを載せる
  - 使用場面: ランタイムのストリーミングロード、シーン BGM の事前登録
  - 用途から決めた仕様: MF を開いてネイティブレートだけ読む。フルデコーダは `Play` 時の `PrepareStreamingDecoder` に任せる

### 8. 最終目標との査定（§6.7）

| タスク | 判定（有効／矛盾／寄与なし／査定不能） | 根拠（完成形の説明の・行） |
|--------|----------------------------------------|------------------------------|
| SetupStreaming が 44.1kHz 固定 | 有効 | ・サイズが大きい音源ファイルについてストリーミング再生ができる / ・mp4やwavなど代表的な拡張子の音源ファイルを読み込める |

---

## セッション 2026-11-19 18:45（/loop 15m・roadmap tick）

### 1. 現在の進捗状況と完了したこと

- **ループ:** `loop-roadmap-execute`（15m）継続
- **着手ブランチ:** `fix/LoadHitNoWfReprobe`
- **roadmap:** Audio › AudioManager — `Load ヒットで再判定なし`
- `Load` の `soundIndexMap_` 命中時に `RefreshSoundDataOnLoadCacheHit` を呼ぶ。5MB 閾値で短尺／ストリームの経路を再判定し、切り替え時は同一 index の `SoundData` を置き換える。ストリームは `wfEx` を再プローブ、短尺はデコードし直して PCM を更新
- `SetupStreaming` は `AssignStreamingSoundInPlace` と共通化
- **ビルド:** Cloud VM 上では未実行（Windows ツールチェーン必須）

### 2. 発生した課題または設計上の選択肢

- 短尺のキャッシュヒットは毎回フルデコード。ホットリロード寄りで、初回のみ Load のケースよりコストは増える
- 再デコード失敗時は従来データを残したまま index を返す（再生継続優先）

### 3. 次に実行すべきステップの提案

- roadmap 次行: `不足バイトを 0 でパディング`（次ブランチ `fix/DecoderShortBufferZeroPad`）
- Development ビルドと、同じパスを連続 `Load` したときの BGM 再現性の手動確認（任意）

### 4. 実装済みと判断した項目（コード確認・即時 [x]）

- なし

### 5. 保留・着手見送り（§6.3）

- なし

### 6. 用途不明で確認待ち（§6.4）

- なし

### 7. 推測した用途（§6.4）

- **Load ヒットで再判定なし**
  - 用途: シーン再読込や `PlaySeDetail` 経由の二度目 `Load` で、差し替えた音源や閾値変更後も正しいストリーム／短尺経路とレートを使う
  - 使用場面: ランタイム（BGM・SE の繰り返し Load）、開発中のアセット差し替え
  - 用途から決めた仕様: index は安定、caches ヒットでもメタデータと短尺 PCM だけ更新する

### 8. 最終目標との査定（§6.7）

| タスク | 判定（有効／矛盾／寄与なし／査定不能） | 根拠（完成形の説明の・行） |
|--------|----------------------------------------|------------------------------|
| Load ヒットで再判定なし | 有効 | ・mp4やwavなど代表的な拡張子の音源ファイルを読み込める / ・bgmとしてのループ再生ができる / ・サイズが大きい音源ファイルについてストリーミング再生ができる |

---

## セッション 2026-11-19 19:00（/loop 15m・roadmap tick）

### 1. 現在の進捗状況と完了したこと

- **ループ:** `loop-roadmap-execute`（15m）継続
- **着手ブランチ:** `fix/DecoderShortBufferZeroPad`
- **roadmap:** Audio › AudioManager — `不足バイトを 0 でパディング`
- `RefillBuffer` で `bytesRead < kBufferSize` のとき尾を `std::fill` していた処理を削除。`XAUDIO2_BUFFER::AudioBytes` は読み込み長のまま Submit する
- **ビルド:** Cloud VM 上では未実行（Windows ツールチェーン必須）

### 2. 発生した課題または設計上の選択肢

- EOF 直前の短いチャンクは無音パディングではなく実サンプル長のみ再生。ループ時は既存の `Seek` + 再 `ReadNextChunk` が続く
- バッファ未使用領域の内容は未初期化のままだが XAudio には渡さない

### 3. 次に実行すべきステップの提案

- roadmap 次行: `PlaySe が Play の Handle を捨てる`（次ブランチ `fix/PlaySeDiscardsPlayHandle`）
- BGM ループ境界のクリック有無を手動確認（任意）

### 4. 実装済みと判断した項目（コード確認・即時 [x]）

- なし

### 5. 保留・着手見送り（§6.3）

- なし

### 6. 用途不明で確認待ち（§6.4）

- なし

### 7. 推測した用途（§6.4）

- **不足バイトを 0 でパディング**
  - 用途: ストリーム BGM／長尺 SE のチャンク末尾で意図しない無音を挟まず、途切れなくループする
  - 使用場面: ランタイム（ストリーミング再生、シーン BGM ループ）
  - 用途から決めた仕様: Submit 長は実読み込みバイトのみ。不足分を 0 埋めしない

### 8. 最終目標との査定（§6.7）

| タスク | 判定（有効／矛盾／寄与なし／査定不能） | 根拠（完成形の説明の・行） |
|--------|----------------------------------------|------------------------------|
| 不足バイトを 0 でパディング | 有効 | ・サイズが大きい音源ファイルについてストリーミング再生ができる / ・bgmとしてのループ再生ができる |

---

## セッション 2026-11-19 19:15（/loop 15m・roadmap tick）

### 1. 現在の進捗状況と完了したこと

- **ループ:** `loop-roadmap-execute`（15m）継続
- **着手ブランチ:** `fix/PlaySeDiscardsPlayHandle`
- **roadmap:** Audio › PlaySe — `PlaySe が Play の Handle を捨てる`
- `PlaySe` / `PlaySeCandidates` の戻り値を `Audio::Handle` に変更し、`AudioManager::Play` の結果を返す。読み込み失敗・ファイル無しは空 Handle
- 既存の void 呼び出し（`PlaySubmitSe` 等）は戻り値未使用のままコンパイル可能
- **ビルド:** Cloud VM 上では未実行

### 2. 発生した課題または設計上の選択肢

- シーン Exit でワンショット SE を止める処理は別項目（`StopSe-001`）。本 tick は Handle を呼び出し側で得られる状態にしたのみ
- ストリーミング SE が `Play` 経由になる場合も Handle で追跡可能

### 3. 次に実行すべきステップの提案

- roadmap 次行: `OnStreamEnd が Waiting のみセット`（次ブランチ `fix/StreamingCallbackOnStreamEnd`）
- シーン遷移時に `StopSe` で One-shot を止める統合（任意・別ギャップ）

### 4. 実装済みと判断した項目（コード確認・即時 [x]）

- なし

### 5. 保留・着手見送り（§6.3）

- なし

### 6. 用途不明で確認待ち（§6.4）

- なし

### 7. 推測した用途（§6.4）

- **PlaySe が Play の Handle を捨てる**
  - 用途: ジャンプ・被弾・UI 等のワンショット SE を、シーン切替や上書き再生時に `StopSe` で個別停止する
  - 使用場面: ランタイムゲームプレイ、シーン遷移（`ChangeSceneFade`）
  - 用途から決めた仕様: ヘルパは fire-and-forget も可能だが、必要なら Handle を返して既存の `StopSe`／音量 API を使える

### 8. 最終目標との査定（§6.7）

| タスク | 判定（有効／矛盾／寄与なし／査定不能） | 根拠（完成形の説明の・行） |
|--------|----------------------------------------|------------------------------|
| PlaySe が Play の Handle を捨てる | 有効 | ・bgmとしてのループ再生ができる（効果音・BGM の音量割合をプレイヤーが設定し保存反映 — Handle 経由の制御の前提） |

---

## セッション 2026-11-19 19:30（/loop 15m・roadmap tick）

### 1. 現在の進捗状況と完了したこと

- **ループ:** `loop-roadmap-execute`（15m）継続
- **着手ブランチ:** `fix/StreamingCallbackOnStreamEnd`
- **roadmap:** Audio › StreamingVoiceCallback — `OnStreamEnd が Waiting のみセット`
- `OnStreamEnd` で `VoiceState::Waiting` 固定をやめ、`Streaming` に遷移。`Update` の `RefillBuffer`（ループは Seek 再読込）を同 tick で走らせ、ループ BGM の 1 フレーム途切れを軽減。非ループは従来どおり Refill 失敗後 `Waiting` → `TryContinueNonLoopStreamingVoice` → 停止
- **ビルド:** Cloud VM 上では未実行

### 2. 発生した課題または設計上の選択肢

- コールバック内では Refill せず state のみ（次行 `OnBufferEnd` と線を分ける）
- `nullptr` 親ボイスは無視

### 3. 次に実行すべきステップの提案

- roadmap 次行: `OnBufferEnd が Streaming へ遷移`（次ブランチ `fix/StreamingCallbackOnBufferEnd`）
- ループ BGM 終端の手動確認（任意）

### 4. 実装済みと判断した項目（コード確認・即時 [x]）

- なし

### 5. 保留・着手見送り（§6.3）

- なし

### 6. 用途不明で確認待ち（§6.4）

- なし

### 7. 推測した用途（§6.4）

- **OnStreamEnd が Waiting のみセット**
  - 用途: ストリーム BGM ループと長尺 SE の終端で、無音フレームなく次チャンクまたは自然終了へつなぐ
  - 使用場面: ランタイム（シーン BGM、ストリーミング SE）
  - 用途から決めた仕様: 終端通知は「補充が必要」の `Streaming` とし、`Update` の既存 Refill／非ループ継続ロジックに委譲

### 8. 最終目標との査定（§6.7）

| タスク | 判定（有効／矛盾／寄与なし／査定不能） | 根拠（完成形の説明の・行） |
|--------|----------------------------------------|------------------------------|
| OnStreamEnd が Waiting のみセット | 有効 | ・bgmとしてのループ再生ができる / ・サイズが大きい音源ファイルについてストリーミング再生ができる |

---

## セッション 2026-11-19 19:45（/loop 15m・roadmap tick）

### 1. 現在の進捗状況と完了したこと

- **ループ:** `loop-roadmap-execute`（15m）継続
- **着手ブランチ:** `fix/StreamingCallbackOnBufferEnd`
- **roadmap:** Audio › StreamingVoiceCallback — `OnBufferEnd が Streaming へ遷移`
- `OnBufferEnd` で `VoiceState::Streaming` のみ設定していたのをやめ、`AudioManager::RefillStreamingVoiceAfterBufferEnd` で `RefillBuffer` を即 Submit。コールバックに `AudioManager*` を渡す（`CreateVoicePool`）
- 失敗時は `Streaming` に戻し、従来どおり `Update` が再試行。一時停止・停止・Waiting はスキップ
- **ビルド:** Cloud VM 上では未実行

### 2. 発生した課題または設計上の選択肢

- XAudio コールバックスレッドから `RefillBuffer` を呼ぶため、`Update` と同時実行の可能性はある（現行は state が atomic）
- 高負荷時の完全保証は未検証

### 3. 次に実行すべきステップの提案

- roadmap 次行: `EOF で break のみ`（次ブランチ `fix/AudioDecoderEofBreak`）
- ストリーム BGM の連続再生手動確認（任意）

### 4. 実装済みと判断した項目（コード確認・即時 [x]）

- なし

### 5. 保留・着手見送り（§6.3）

- なし

### 6. 用途不明で確認待ち（§6.4）

- なし

### 7. 推測した用途（§6.4）

- **OnBufferEnd が Streaming へ遷移**
  - 用途: バッファ再生終了直後にキューを補充し、BGM／長尺 SE のクリックや無音を防ぐ
  - 使用場面: ランタイム（高負荷フレームでもストリーミング再生）
  - 用途から決めた仕様: state 更新だけにせず、コールバック内で `RefillBuffer` まで実行

### 8. 最終目標との査定（§6.7）

| タスク | 判定（有効／矛盾／寄与なし／査定不能） | 根拠（完成形の説明の・行） |
|--------|----------------------------------------|------------------------------|
| OnBufferEnd が Streaming へ遷移 | 有効 | ・サイズが大きい音源ファイルについてストリーミング再生ができる / ・bgmとしてのループ再生ができる |

---

## セッション 2026-11-19 20:00（/loop 15m・roadmap tick）

### 1. 現在の進捗状況と完了したこと

- **ループ:** `loop-roadmap-execute`（15m）継続
- **着手ブランチ:** `fix/AudioDecoderEofBreak`
- **roadmap:** Audio › AudioDecoder — `EOF で break のみ`
- `ReadNextChunk` で `ENDOFSTREAM` 時、ループ再生なら `Seek` してバッファ未満なら読み続ける。非ループは `atEndOfStream` を立て、単に break するだけにしない
- `TryContinueNonLoopStreamingVoice` は終端かつキュー空なら即停止へ。`Seek` / `ResetStreamingVoice` でフラグクリア
- **ビルド:** Cloud VM 上では未実行

### 2. 発生した課題または設計上の選択肢

- ループはデコーダ内巻き戻し + `RefillBuffer` 側の Seek の二重経路は残す（失敗時のフォールバック）
- 端数バイト退避は次行 `4 バイト揃えで端数を次回へ`

### 3. 次に実行すべきステップの提案

- roadmap 次行: `4 バイト揃えで端数を次回へ`（次ブランチ `fix/AudioDecoderAlignRemainder`）
- ループ BGM 境界の手動確認（任意）

### 4. 実装済みと判断した項目（コード確認・即時 [x]）

- なし

### 5. 保留・着手見送り（§6.3）

- なし

### 6. 用途不明で確認待ち（§6.4）

- なし

### 7. 推測した用途（§6.4）

- **EOF で break のみ**
  - 用途: ストリーム BGM ループはファイル終端で巻き戻し、非ループ長尺 SE は自然終了を正しく打ち切る
  - 使用場面: ランタイム再生
  - 用途から決めた仕様: EOF はループ／非ループで分岐。非ループは `atEndOfStream` で Update 停止を早める

### 8. 最終目標との査定（§6.7）

| タスク | 判定（有効／矛盾／寄与なし／査定不能） | 根拠（完成形の説明の・行） |
|--------|----------------------------------------|------------------------------|
| EOF で break のみ | 有効 | ・bgmとしてのループ再生ができる / ・サイズが大きい音源ファイルについてストリーミング再生ができる |

---

## セッション 2026-11-19 20:15（/loop 15m・roadmap tick）

### 1. 現在の進捗状況と完了したこと

- **ループ:** `loop-roadmap-execute`（15m）継続
- **着手ブランチ:** `fix/AudioDecoderAlignRemainder`
- **roadmap:** Audio › AudioDecoder — `4 バイト揃えで端数を次回へ`
- 端数バイトの `remainingData` 結合順を時系列どおり先頭へ（旧: MF オーバーフローの後に付けていた）。`pcmBlockAlign` をボイスへ保持（`PrepareStreamingDecoder` で設定）
- ストリーム終端の短いチャンクはフレーム境界までゼロパッドして Submit。`atEndOfStream` かつ `remainingData` のみ残りはフラッシュ読み出し
- **ビルド:** Cloud VM 上では未実行

### 2. 発生した課題または設計上の選択肢

- EOF 時のパディングは最終チャンクのみ（途中チャンクは従来どおり次回へ退避）
- 短尺 Load デコーダは 44.1kHz 固定のまま（別行）

### 3. 次に実行すべきステップの提案

- roadmap 次行: `IsBgmLoopPaused がシーンから未使用`（次ブランチ `fix/IsBgmLoopPausedUnused`）
- 長尺ストリーム SE の途切れ手動確認（任意）

### 4. 実装済みと判断した項目（コード確認・即時 [x]）

- なし

### 5. 保留・着手見送り（§6.3）

- なし

### 6. 用途不明で確認待ち（§6.4）

- なし

### 7. 推測した用途（§6.4）

- **4 バイト揃えで端数を次回へ**
  - 用途: ステレオ 16bit PCM のフレーム境界を崩さず、チャンク境界のクリックを防ぐ
  - 使用場面: ランタイムのストリーミング BGM／長尺 SE
  - 用途から決めた仕様: フルチャンクは端数を次読みの先頭へ。終端だけ最小限パッド

### 8. 最終目標との査定（§6.7）

| タスク | 判定（有効／矛盾／寄与なし／査定不能） | 根拠（完成形の説明の・行） |
|--------|----------------------------------------|------------------------------|
| 4 バイト揃えで端数を次回へ | 有効 | ・サイズが大きい音源ファイルについてストリーミング再生ができる |

---

## セッション 2026-11-19 20:30（/loop 15m・roadmap tick）

### 1. 現在の進捗状況と完了したこと

- **ループ:** `loop-roadmap-execute`（15m）継続
- **着手ブランチ:** `fix/IsBgmLoopPausedUnused`
- **roadmap:** Audio › IsBgmLoopPaused — `IsBgmLoopPaused がシーンから未使用`
- `TryResumeSceneBgmAfterChangeSceneFade` / `IsSceneBgmPausedForChangeSceneFade` を `PlaySe.h` に追加（内部で `IsBgmLoopPaused`）。`SyncSceneBgmForChangeSceneFade` と `SceneGame::MaintainSceneBgmIfNeeded` から利用
- Title／Result は従来どおり `SyncSceneBgmForChangeSceneFade` 経由で状態照会（Game は Maintain でも明示利用）
- **ビルド:** Cloud VM 上では未実行

### 2. 発生した課題または設計上の選択肢

- ポーズメニュー UI 連携は未実装（将来は `IsSceneBgmPausedForChangeSceneFade` を参照可）
- `IsSceneBgmAudible` と役割は重なるが、フェード中の再 Play 抑制が主目的

### 3. 次に実行すべきステップの提案

- roadmap 次行: `ファイルサイズだけでストリーム判定`（次ブランチ `fix/StreamByFileSizeOnly`）
- シーン遷移中の BGM 手動確認（任意）

### 4. 実装済みと判断した項目（コード確認・即時 [x]）

- なし

### 5. 保留・着手見送り（§6.3）

- なし

### 6. 用途不明で確認待ち（§6.4）

- なし

### 7. 推測した用途（§6.4）

- **IsBgmLoopPaused がシーンから未使用**
  - 用途: ChangeSceneFade 中に BGM を止めたまま、フェード後に誤って別曲を差し込まない
  - 使用場面: ゲーム／タイトル／リザルトのシーン Update
  - 用途から決めた仕様: シーンが Handle を保持し、一時停止状態を API 経由で参照して再開タイミングを決める

### 8. 最終目標との査定（§6.7）

| タスク | 判定（有効／矛盾／寄与なし／査定不能） | 根拠（完成形の説明の・行） |
|--------|----------------------------------------|------------------------------|
| IsBgmLoopPaused がシーンから未使用 | 有効 | ・bgmとしてのループ再生ができる / ・ゲーム中でマスターボリュームや効果音、bgmなどまとめて音量割合をプレイヤーが設定し、その設定を保存/反映できる |

---

## セッション 2026-11-19 20:45（/loop 15m・roadmap tick）

### 1. 現在の進捗状況と完了したこと

- **ループ:** `loop-roadmap-execute`（15m）継続
- **着手ブランチ:** `fix/StreamByFileSizeOnly`
- **roadmap:** Audio › IsSuitableStreaming — `ファイルサイズだけでストリーム判定`
- `StreamLoadPolicy` を追加。オンディスクサイズに加え、拡張子から推定したデコード後 PCM サイズが閾値超ならストリーム経路にする（圧縮 BGM のメモリピーク対策）
- `AudioManager::IsSuitableStreaming` は `IsSuitableStreamingLoad` に委譲
- **ビルド:** Cloud VM 上では未実行

### 2. 発生した課題または設計上の選択肢

- 推定倍率は固定（MP3 系 ×12）。精密判定は MF デュレーション案（将来）
- 閾値値の設定化は次行 `閾値 5MB がハードコード`（`kStreamingLoadThresholdBytes` を単一参照点化済み）

### 3. 次に実行すべきステップの提案

- roadmap 次行: `閾値 5MB がハードコード`（次ブランチ `fix/StreamingThresholdConfig`）
- 4MB 級 MP3 BGM のロード経路確認（任意）

### 4. 実装済みと判断した項目（コード確認・即時 [x]）

- なし

### 5. 保留・着手見送り（§6.3）

- なし

### 6. 用途不明で確認待ち（§6.4）

- なし

### 7. 推測した用途（§6.4）

- **ファイルサイズだけでストリーム判定**
  - 用途: 圧縮 BGM を短いファイルサイズ誤って全メモリ展開しない
  - 使用場面: ランタイム Load、シーン BGM 登録
  - 用途から決めた仕様: WAV は実サイズ、MP3/M4A 等は拡大推定でストリーム選択

### 8. 最終目標との査定（§6.7）

| タスク | 判定（有効／矛盾／寄与なし／査定不能） | 根拠（完成形の説明の・行） |
|--------|----------------------------------------|------------------------------|
| ファイルサイズだけでストリーム判定 | 有効 | ・サイズが大きい音源ファイルについてストリーミング再生ができる / ・mp4やwavなど代表的な拡張子の音源ファイルを読み込める |

---

## セッション 2026-11-19 21:00（/loop 15m・roadmap tick）

### 1. 現在の進捗状況と完了したこと

- **ループ:** `loop-roadmap-execute`（15m）継続
- **着手ブランチ:** `fix/StreamingThresholdConfig`
- **roadmap:** Audio › IsSuitableStreaming — `閾値 5MB がハードコード`
- `StreamLoadPolicy::SetThresholdBytes` / `ThresholdBytes` でランタイム変更可能（既定 5MB、256KB〜256MB にクランプ）。`AudioManager` から同 API を公開
- `Audio.h` の `kFileSizeThreshold` を削除し、閾値を `StreamLoadPolicy` に集約
- **ビルド:** Cloud VM 上では未実行

### 2. 発生した課題または設計上の選択肢

- プレイヤー設定 UI や json 読み込みは未接続（起動時は既定 5MB）
- エディタ／DEVELOPMENT 向けの閾値オーバーライドは必要なら別タスク

### 3. 次に実行すべきステップの提案

- roadmap 次行: `再生秒取得がゲームシーン未使用`（次ブランチ `fix/SceneBgmPlaybackSeconds`）
- メモリ制約のある環境で閾値を下げて試す（任意）

### 4. 実装済みと判断した項目（コード確認・即時 [x]）

- なし

### 5. 保留・着手見送り（§6.3）

- なし

### 6. 用途不明で確認待ち（§6.4）

- なし

### 7. 推測した用途（§6.4）

- **閾値 5MB がハードコード**
  - 用途: 端末メモリやタイトル種別に応じて短尺／ストリーム切替閾値を変える
  - 使用場面: ゲーム初期化、将来のオプション画面
  - 用途から決めた仕様: 既定は 5MB。`AudioManager::SetStreamingLoadThresholdBytes` で起動後も変更可

### 8. 最終目標との査定（§6.7）

| タスク | 判定（有効／矛盾／寄与なし／査定不能） | 根拠（完成形の説明の・行） |
|--------|----------------------------------------|------------------------------|
| 閾値 5MB がハードコード | 有効 | ・サイズが大きい音源ファイルについてストリーミング再生ができる |

---

## セッション 2026-11-19 21:15（/loop 15m・roadmap tick）

### 1. 現在の進捗状況と完了したこと

- **ループ:** `loop-roadmap-execute`（15m）継続
- **着手ブランチ:** `fix/SceneBgmPlaybackSeconds`
- **roadmap:** Audio › GetBgmLoopPlaybackSeconds — `再生秒取得がゲームシーン未使用`
- `CommandChangeScene::GetPendingNextScene` を追加。Game→Game 遷移の `ExitScene` で `GetBgmLoopPlaybackSeconds` を呼び、`resumeGameBgmPlaybackSeconds_` に保存
- 復元の `Set` は次行 `再生位置指定がゲームシーン未使用` で `EnterScene` に接続予定
- **ビルド:** Cloud VM 上では未実行

### 2. 発生した課題または設計上の選択肢

- Title／Result 離脱時は保存しない（別 BGM のため）
- ボス撃破で BGM 停止後の Exit は handle 無効のため保存されない

### 3. 次に実行すべきステップの提案

- roadmap 次行: `再生位置指定がゲームシーン未使用`（次ブランチ `fix/SceneBgmPlaybackSeek`）
- 自機撃墜→同シーン再入で BGM 位置が戻るか確認（任意）

### 4. 実装済みと判断した項目（コード確認・即時 [x]）

- なし

### 5. 保留・着手見送り（§6.3）

- なし

### 6. 用途不明で確認待ち（§6.4）

- なし

### 7. 推測した用途（§6.4）

- **再生秒取得がゲームシーン未使用**
  - 用途: ラウンド再開（フェード付き Game→Game）で BGM を頭出しに戻さない
  - 使用場面: ランタイム（自機撃墜リスタート）
  - 用途から決めた仕様: 遷移直前に再生秒を取得しシーンインスタンスに保持

### 8. 最終目標との査定（§6.7）

| タスク | 判定（有効／矛盾／寄与なし／査定不能） | 根拠（完成形の説明の・行） |
|--------|----------------------------------------|------------------------------|
| 再生秒取得がゲームシーン未使用 | 有効 | ・bgmとしてのループ再生ができる |

---

## セッション 2026-11-19 21:30（/loop 15m・roadmap tick）

### 1. 現在の進捗状況と完了したこと

- **ループ:** `loop-roadmap-execute`（15m）継続
- **着手ブランチ:** `fix/SceneBgmPlaybackSeek`
- **roadmap:** Audio › SetBgmLoopPlaybackSeconds — `再生位置指定がゲームシーン未使用`（前 tick の Get とセット）
- `SceneGame::EnterScene` で `PlayBgmLoop` 後、`resumeGameBgmPlaybackSeconds_` があれば `SetBgmLoopPlaybackSeconds` で復元
- **ビルド:** Cloud VM 上では未実行

### 2. 発生した課題または設計上の選択肢

- roadmap 上の先頭 `- [ ]` は `StopSe` 行だが、Title は既に `StopSe` で環境ループを停止済み（次 tick で §6.2 化または `StopSeLoop` 整理可）
- シーク失敗時は秒数を保持したまま再試行可能（通常は初回成功）

### 3. 次に実行すべきステップの提案

- roadmap 先頭: `ループ SE ハンドルの個別停止が無い`（次ブランチ `fix/StopLoopSeOnSceneExit`）
- 自機撃墜リスタートで BGM が連続するか手動確認（任意）

### 4. 実装済みと判断した項目（コード確認・即時 [x]）

- なし

### 5. 保留・着手見送り（§6.3）

- なし

### 6. 用途不明で確認待ち（§6.4）

- なし

### 7. 推測した用途（§6.4）

- **再生位置指定がゲームシーン未使用**
  - 用途: Game→Game 再入で直前と同じ BGM 位置から再生再開
  - 使用場面: ランタイム（リスタートフェード）
  - 用途から決めた仕様: Exit で保存した秒数を Enter で Set

### 8. 最終目標との査定（§6.7）

| タスク | 判定（有効／矛盾／寄与なし／査定不能） | 根拠（完成形の説明の・行） |
|--------|----------------------------------------|------------------------------|
| 再生位置指定がゲームシーン未使用 | 有効 | ・bgmとしてのループ再生ができる |

---

## セッション 2026-11-19 21:45（/loop 15m・roadmap tick）

### 1. 現在の進捗状況と完了したこと

- **ループ:** `loop-roadmap-execute`（15m）継続
- **着手ブランチ:** `fix/StopLoopSeOnSceneExit`
- **roadmap:** Audio › StopSe — `ループ SE ハンドルの個別停止が無い`
- `StopSeLoop` を `PlaySe.h` に追加（`StopSe` のループ SE 向けエイリアス）。`SceneTitle::ExitScene` でタイトル環境ループを `StopSeLoop` で明示停止
- **ビルド:** Cloud VM 上では未実行

### 2. 発生した課題または設計上の選択肢

- Game／Result にループ SE スロットは未所持（将来は同 API で停止）
- フェード中も Exit まで環境ループは鳴り続ける（従来どおり）

### 3. 次に実行すべきステップの提案

- roadmap 次行: `キャッシュ命中で再 Load しない`（次ブランチ `fix/PlaySeDetailCacheReload`）
- タイトル→ゲーム遷移で環境 SE が残らないか確認（任意）

### 4. 実装済みと判断した項目（コード確認・即時 [x]）

- なし

### 5. 保留・着手見送り（§6.3）

- なし

### 6. 用途不明で確認待ち（§6.4）

- なし

### 7. 推測した用途（§6.4）

- **ループ SE ハンドルの個別停止**
  - 用途: タイトル環境ループを BGM 停止と分離して Handle 単位で片付ける
  - 使用場面: シーン Exit、シーン遷移後の SE 残骸防止
  - 用途から決めた仕様: `PlaySeLoop` 開始分は `StopSeLoop` で停止（BGM は `StopBgmLoop` のまま）

### 8. 最終目標との査定（§6.7）

| タスク | 判定（有効／矛盾／寄与なし／査定不能） | 根拠（完成形の説明の・行） |
|--------|----------------------------------------|------------------------------|
| ループ SE ハンドルの個別停止 | 有効 | ・bgmとしてのループ再生ができる / 効果音の音量制御（Handle 経由の停止） |

---

## セッション 2026-11-19 22:00（/loop 15m・roadmap tick）

### 1. 現在の進捗状況と完了したこと

- **ループ:** `loop-roadmap-execute`（15m）継続
- **着手ブランチ:** `fix/PlaySeDetailCacheReload`
- **roadmap:** Audio › PlaySeDetail — `キャッシュ命中で再 Load しない`
- `GetOrLoadSeIndex` が静的 cache 命中時に index を返すだけだったのを修正。毎回 `AudioManager::Load` を呼び、`RefreshSoundDataOnLoadCacheHit` 経由で差し替え・閾値変化を反映
- ローカル cache は index 記録のみ（Load 失敗時はエントリ削除）
- **ビルド:** Cloud VM 上では未実行

### 2. 発生した課題または設計上の選択肢

- プレイ中 SE 呼び出しごとに Load 走る（Audio 側は map 命中＋再検証）。ホットパスは要プロファイル
- cache アンロードは次行 `static cache がアンロードされない`

### 3. 次に実行すべきステップの提案

- roadmap 次行: `static cache がアンロードされない`（次ブランチ `fix/PlaySeDetailCacheEvict`）
- 開発中に SE ファイル差し替え後の再生確認（任意）

### 4. 実装済みと判断した項目（コード確認・即時 [x]）

- なし

### 5. 保留・着手見送り（§6.3）

- なし

### 6. 用途不明で確認待ち（§6.4）

- なし

### 7. 推測した用途（§6.4）

- **キャッシュ命中で再 Load しない**
  - 用途: 開発中の SE／BGM 差し替えやストリーム閾値変更を、同じパスで `PlaySe` し直したとき反映
  - 使用場面: ランタイム、レベル制作のイテレーション
  - 用途から決めた仕様: PlaySeDetail の map は index のメモ用。再検証は毎回 `Load`

### 8. 最終目標との査定（§6.7）

| タスク | 判定（有効／矛盾／寄与なし／査定不能） | 根拠（完成形の説明の・行） |
|--------|----------------------------------------|------------------------------|
| キャッシュ命中で再 Load しない | 有効 | ・mp4やwavなど代表的な拡張子の音源ファイルを読み込める |

---

## セッション 2026-11-19 22:15（/loop 15m・roadmap tick）

### 1. 現在の進捗状況と完了したこと

- **ループ:** `loop-roadmap-execute`（15m）継続
- **着手ブランチ:** `fix/PlaySeDetailCacheEvict`
- **roadmap:** Audio › PlaySeDetail — `static cache がアンロードされない`
- SE パス→index の静的 map を `PlaySeDetailCache.cpp` に移動。`ClearSeIndexPathCache` と存在しないパスの剪定を追加
- `CommandScene::ChangeScene` でシーン退出後に cache をクリア（AudioManager の SoundData はそのまま、path メモのみ解放）
- **ビルド:** Cloud VM 上では未実行

### 2. 発生した課題または設計上の選択肢

- 次シーン Enter で同じ SE を鳴らすと再度 `Load`（意図どおり）
- SoundData 本体の開放は AudioManager 側の別ポリシー

### 3. 次に実行すべきステップの提案

- roadmap 次行: `SetBgmLoopPitch がシーンから未呼び出し`（次ブランチ `fix/SceneBgmPitch`）
- 長時間セッションでの SE パス map サイズ確認（任意）

### 4. 実装済みと判断した項目（コード確認・即時 [x]）

- なし

### 5. 保留・着手見送り（§6.3）

- なし

### 6. 用途不明で確認待ち（§6.4）

- なし

### 7. 推測した用途（§6.4）

- **static cache がアンロードされない**
  - 用途: シーン遷移ごとに PlaySeDetail のパスキャッシュを空にし、長時間プレイの map 肥大を防ぐ
  - 使用場面: ランタイムのシーン切替
  - 用途から決めた仕様: ChangeScene 後にクリア。GetOrLoad 時に消えたファイルのエントリも削除

### 8. 最終目標との査定（§6.7）

| タスク | 判定（有効／矛盾／寄与なし／査定不能） | 根拠（完成形の説明の・行） |
|--------|----------------------------------------|------------------------------|
| static cache がアンロードされない | 有効 | ・mp4やwavなど代表的な拡張子の音源ファイルを読み込める |

---

## セッション 2026-11-19 22:30（/loop 15m・roadmap tick）

### 1. 現在の進捗状況と完了したこと

- **ループ:** `loop-roadmap-execute`（15m）継続
- **着手ブランチ:** `fix/SceneBgmPitch`
- **roadmap:** Audio › SetBgmLoopPitch — `SetBgmLoopPitch がシーンから未呼び出し`
- Title / Game / Result の `EnterScene` で BGM 開始後にピッチ 1.0 を明示設定
- `SceneGame::OnPlayerDefeated` で撃墜演出中 BGM を 0.82 倍速に（スロー演出）
- `SyncSceneBgmForChangeSceneFade` / `TryResumeSceneBgmAfterChangeSceneFade` でフェード時 0.88・復帰時 1.0
- **ビルド:** Cloud VM（Linux）では MSBuild 不可のため未実行

### 2. 発生した課題または設計上の選択肢

- ボス撃破時は既存どおり BGM 停止のためピッチ変更なし

### 3. 次に実行すべきステップの提案

- roadmap 次行: `SetSeVolume がゲームロジックから未使用`（次ブランチ `fix/SceneSeVolume`）

### 4. 実装済みと判断した項目（コード確認・即時 [x]）

- なし

### 5. 保留・着手見送り（§6.3）

- なし

### 6. 用途不明で確認待ち（§6.4）

- なし

### 7. 推測した用途（§6.4）

- **SetBgmLoopPitch がシーンから未呼び出し**
  - 用途: シーン遷移フェード・撃墜演出に合わせて BGM の再生速度を変え、途切れないまま雰囲気を伝える
  - 使用場面: ランタイムのタイトル／ゲーム／リザルト、ChangeSceneFade、プレイヤー撃墜
  - 用途から決めた仕様: 通常 1.0、フェード中 0.88、撃墜演出 0.82

### 8. 最終目標との査定（§6.7）

| タスク | 判定（有効／矛盾／寄与なし／査定不能） | 根拠（完成形の説明の・行） |
|--------|----------------------------------------|------------------------------|
| SetBgmLoopPitch がシーンから未呼び出し | 有効 | ・ゲーム中でマスターボリュームや効果音、bgmなどまとめて音量割合をプレイヤーが設定し、その設定を保存/反映できるようになっている |

---

## セッション 2026-11-19 22:45（/loop 15m・roadmap tick）

### 1. 現在の進捗状況と完了したこと

- **ループ:** `loop-roadmap-execute`（15m）継続
- **着手ブランチ:** `fix/SceneSeVolume`
- **roadmap:** Audio › SetSeVolume — `SetSeVolume がゲームロジックから未使用`
- `SceneTitle`: 環境ループ SE の通常／フェード音量（0.42 / 0.1）
- `SceneGame`: 被弾ヴィネット表示中 BGM を `SetSeVolume` で 0.58 にダック
- `AttackTelegraphUtils`: ボス警告 SE を 0.78 に調整
- `PlayerDamageScreenVignette::IsShowing` を追加
- **ビルド:** Cloud VM（Linux）未実行

### 2. 発生した課題または設計上の選択肢

- フェード中 BGM ダック（0.12）より被弾ダック（0.58）を優先する順序は Update 内の適用順で調整

### 3. 次に実行すべきステップの提案

- roadmap 次行: `GetSeVolume がシーンから未呼び出し`（次ブランチ `fix/SceneGetSeVolume`）

### 4. 実装済みと判断した項目（コード確認・即時 [x]）

- なし

### 5. 保留・着手見送り（§6.3）

- なし

### 6. 用途不明で確認待ち（§6.4）

- なし

### 7. 推測した用途（§6.4）

- **SetSeVolume がゲームロジックから未使用**
  - 用途: 演出に合わせてループ SE／BGM／警告音の音量をランタイムで下げ、被弾や遷移で聞き取りやすくする
  - 使用場面: タイトル環境音、ゲーム被弾、ボス攻撃予告
  - 用途から決めた仕様: タイトル 0.42、フェード 0.1、ヴィネット中 BGM 0.58、警告 SE 0.78

### 8. 最終目標との査定（§6.7）

| タスク | 判定（有効／矛盾／寄与なし／査定不能） | 根拠（完成形の説明の・行） |
|--------|----------------------------------------|------------------------------|
| SetSeVolume がゲームロジックから未使用 | 有効 | ・ゲーム中でマスターボリュームや効果音、bgmなどまとめて音量割合をプレイヤーが設定し、その設定を保存/反映できるようになっている |

---

## セッション 2026-11-19 23:00（/loop 15m・roadmap tick）

### 1. 現在の進捗状況と完了したこと

- **ループ:** `loop-roadmap-execute`（15m）継続
- **着手ブランチ:** `fix/SceneGetSeVolume`
- **roadmap:** Audio › GetSeVolume — `GetSeVolume がシーンから未呼び出し`
- Title: 環境ループ SE は `GetSeVolume` で目標値と照合してから `SetSeVolume`
- Game: 被弾ダッキングは現在音量を読んでから適用／0.58 から 1.0 へ復帰。Enter で音量確認
- Result: Enter／Update で BGM 音量を照会し不足時のみフル音量へ
- **ビルド:** Cloud VM（Linux）未実行

### 2. 発生した課題または設計上の選択肢

- フェード中 0.12 音量は被弾復帰判定（0.58 帯）の対象外

### 3. 次に実行すべきステップの提案

- roadmap 次行: `GetSeFrequencyRatio が未使用`（次ブランチ `fix/SceneSeFrequencyRead`）

### 4. 実装済みと判断した項目（コード確認・即時 [x]）

- なし

### 5. 保留・着手見送り（§6.3）

- なし

### 6. 用途不明で確認待ち（§6.4）

- なし

### 7. 推測した用途（§6.4）

- **GetSeVolume がシーンから未呼び出し**
  - 用途: 音量設定と実ハンドル音量を突き合わせ、ダッキング後の復帰と重複適用を防ぐ
  - 使用場面: タイトル環境 SE、ゲーム被弾 BGM、リザルト BGM
  - 用途から決めた仕様: 差分 0.02 超のみ Set。被弾ダック 0.58 解除時は 1.0 へ

### 8. 最終目標との査定（§6.7）

| タスク | 判定（有効／矛盾／寄与なし／査定不能） | 根拠（完成形の説明の・行） |
|--------|----------------------------------------|------------------------------|
| GetSeVolume がシーンから未呼び出し | 有効 | ・ゲーム中でマスターボリュームや効果音、bgmなどまとめて音量割合をプレイヤーが設定し、その設定を保存/反映できるようになっている |

---

## セッション 2026-11-19 23:15（/loop 15m・roadmap tick）

### 1. 現在の進捗状況と完了したこと

- **ループ:** `loop-roadmap-execute`（15m）継続
- **着手ブランチ:** `fix/SceneSeFrequencyRead`
- **roadmap:** Audio › GetSeFrequencyRatio — `GetSeFrequencyRatio が未使用`
- `SyncSceneBgmForChangeSceneFade`: フェード／通常でピッチを照会してから Set
- Title / Game / Result: Enter・Update でピッチ復帰。撃墜演出開始・終了も照会付き
- **ビルド:** Cloud VM（Linux）未実行

### 2. 発生した課題または設計上の選択肢

- 撃墜スロー 0.82 は通常戦闘に戻ったフレームで 1.0 へ復帰

### 3. 次に実行すべきステップの提案

- roadmap 次行: `SetSeFrequencyRatio がゲームから未使用`（次ブランチ `fix/SceneSeFrequencySet`）

### 4. 実装済みと判断した項目（コード確認・即時 [x]）

- なし

### 5. 保留・着手見送り（§6.3）

- なし

### 6. 用途不明で確認待ち（§6.4）

- なし

### 7. 推測した用途（§6.4）

- **GetSeFrequencyRatio が未使用**
  - 用途: 演出ピッチ変更後に実ハンドルを読み、不要な再 Set を避けつつ 1.0 へ戻す
  - 使用場面: シーンフェード BGM、撃墜スロー、シーン入場
  - 用途から決めた仕様: 差分 0.02 以内はスキップ。0.82 帯は戦闘復帰で 1.0

### 8. 最終目標との査定（§6.7）

| タスク | 判定（有効／矛盾／寄与なし／査定不能） | 根拠（完成形の説明の・行） |
|--------|----------------------------------------|------------------------------|
| GetSeFrequencyRatio が未使用 | 有効 | ・mp4やwavなど代表的な拡張子の音源ファイルを読み込める |

---

## セッション 2026-11-19 23:30（/loop 15m・roadmap tick）

### 1. 現在の進捗状況と完了したこと

- **ループ:** `loop-roadmap-execute`（15m）継続
- **着手ブランチ:** `fix/SceneSeFrequencySet`
- **roadmap:** Audio › SetSeFrequencyRatio — `SetSeFrequencyRatio がゲームから未使用`
- Game: BGM 被弾時ピッチ 0.94、撃墜スロー 0.82 を `SetSeFrequencyRatio` で直接指定
- Title: 環境ループ SE ピッチ 0.98（フェード中 0.9）
- AttackTelegraph / CmpHealthHit: 警告・被弾 SE にピッチ調整
- Scene Result の BGM ピッチ復帰も `SetSeFrequencyRatio` へ統一
- **ビルド:** Cloud VM（Linux）未実行

### 2. 発生した課題または設計上の選択肢

- `SetBgmLoopPitch` はフェード同期ヘルパーのみが使用（シーンは直接 API）

### 3. 次に実行すべきステップの提案

- roadmap 次セクション先頭の未完了行を確認（Audio 節直後）

### 4. 実装済みと判断した項目（コード確認・即時 [x]）

- なし

### 5. 保留・着手見送り（§6.3）

- なし

### 6. 用途不明で確認待ち（§6.4）

- なし

### 7. 推測した用途（§6.4）

- **SetSeFrequencyRatio がゲームから未使用**
  - 用途: 被弾・撃墜・警告 SE をスロー寄りのピッチにし、BGM と演出を連動
  - 使用場面: 戦闘ランタイム、タイトル環境音、被弾処理
  - 用途から決めた仕様: 被弾 BGM 0.94、撃墜 0.82、警告 SE 0.92、被弾 SE 0.93

### 8. 最終目標との査定（§6.7）

| タスク | 判定（有効／矛盾／寄与なし／査定不能） | 根拠（完成形の説明の・行） |
|--------|----------------------------------------|------------------------------|
| SetSeFrequencyRatio がゲームから未使用 | 有効 | ・mp4やwavなど代表的な拡張子の音源ファイルを読み込める |

---

## セッション 2026-11-19 23:45（/loop 15m・roadmap tick）

### 1. 現在の進捗状況と完了したこと

- **ループ:** `loop-roadmap-execute`（15m）継続
- **着手ブランチ:** `feature/UiStateMultiSelect`
- **roadmap:** UiState — `複数選択が無く一括調整できない`
- `UiState::selectedInstanceIndices` と Ctrl+クリックでの複数選択
- インスペクタに一括パネル（スケール倍率・位置オフセット・タグ付与）
- 削除・シーン読込時の選択インデックス整合
- **ビルド:** Cloud VM（Linux）未実行（DEVELOPMENT+IMGUI 想定）

### 2. 発生した課題または設計上の選択肢

- 複数選択時は一括パネルのみ（単体のコンポーネント編集は単一選択へ）
- 混合値表示は未実装

### 3. 次に実行すべきステップの提案

- roadmap 次行: `ランタイム用 Data／Collider 分割候補のエディタ登録が未着手`

### 4. 実装済みと判断した項目（コード確認・即時 [x]）

- なし

### 5. 保留・着手見送り（§6.3）

- なし

### 6. 用途不明で確認待ち（§6.4）

- なし

### 7. 推測した用途（§6.4）

- **複数選択が無く一括調整できない**
  - 用途: レベル制作で同種オブジェクトのスケール・位置・タグをまとめて調整
  - 使用場面: エンティティエディタのヒエラルキー／インスペクタ
  - 用途から決めた仕様: Ctrl 追加選択、3 種の一括適用ボタン

### 8. 最終目標との査定（§6.7）

| タスク | 判定（有効／矛盾／寄与なし／査定不能） | 根拠（完成形の説明の・行） |
|--------|----------------------------------------|------------------------------|
| 複数選択が無く一括調整できない | 有効 | ・各エンティティのコンポーネント構成やコンポーネント内部のパラメータをjsonデータとして作成/保存/編集/削除することができる |

---

## セッション 2026-11-20 00:00（/loop 15m・roadmap tick）

### 1. 現在の進捗状況と完了したこと

- **ループ:** `loop-roadmap-execute`（15m）継続
- **着手ブランチ:** `feature/ComponentSplitRegister`
- **roadmap:** component_split_next — ランタイム Data／Collider のエディタ登録
- 新規 Data: `CmpRuntimeTexturePath` / `CmpColliderAabb`
- `EntityEditorDefinition`・`EntityEditorUi`・`EditorSceneRuntime` に三箇所登録
- `component_split_guidelines.md` を登録済み状態に更新
- **ビルド:** Cloud VM（Linux）未実行

### 2. 発生した課題または設計上の選択肢

- `EntityFactory` へのランタイム読込はガイドラインどおり別タスク
- AABB のワイヤ表示は未実装（`CmpDrawHitSphere` 併用を推奨）

### 3. 次に実行すべきステップの提案

- roadmap 次行: コンポーネント登録三箇所同期の検証自動化

### 4. 実装済みと判断した項目（コード確認・即時 [x]）

- なし

### 5. 保留・着手見送り（§6.3）

- なし

### 6. 用途不明で確認待ち（§6.4）

- なし

### 7. 推測した用途（§6.4）

- **ランタイム用 Data／Collider 分割候補のエディタ登録**
  - 用途: 配布用テクスチャ参照と AABB データをエディタから json 保存し、後続でランタイム接続
  - 使用場面: エンティティエディタのコンポーネント追加・シーンプレビュー
  - 用途から決めた仕様: Editor 専用ロードと分離した path のみ Data

### 8. 最終目標との査定（§6.7）

| タスク | 判定（有効／矛盾／寄与なし／査定不能） | 根拠（完成形の説明の・行） |
|--------|----------------------------------------|------------------------------|
| ランタイム Data／Collider エディタ登録 | 有効 | ・各エンティティのコンポーネント構成やコンポーネント内部のパラメータをjsonデータとして作成/保存/編集/削除することができる |

---

## セッション 2026-11-20 00:15（/loop 15m・roadmap tick）

### 1. 現在の進捗状況と完了したこと

- **ループ:** `loop-roadmap-execute`（15m）継続
- **着手ブランチ:** `chore/ComponentSplitVerify`
- **roadmap:** component_split_verify — 三箇所同期の検証自動化
- `verify_editor_component_registration.py` を追加（Definition / Ui プリセット / Runtime 追加の集合比較）
- `BuildCoverageCheck.yml` に CI ステップ追加
- ガイドライン・cloud_build 手順に検査コマンドを追記
- ローカル実行 OK（`CmpCamera` の Default/Draw は warning のみ）

### 2. 発生した課題または設計上の選択肢

- `CmpCamera` のインスペクタ／既定 params 未整備は別行で対応可

### 3. 次に実行すべきステップの提案

- roadmap 次行: `Initialize が Orthographic を既定生成`（CmpCamera）

### 4. 実装済みと判断した項目（コード確認・即時 [x]）

- なし

### 5. 保留・着手見送り（§6.3）

- なし

### 6. 用途不明で確認待ち（§6.4）

- なし

### 7. 推測した用途（§6.4）

- **component_split_verify**
  - 用途: 新規 Cmp 追加時の登録漏れを PR 前に検出
  - 使用場面: エディタ開発・CI（Linux）
  - 用途から決めた仕様: 三集合の完全一致を必須、params UI は warning

### 8. 最終目標との査定（§6.7）

| タスク | 判定（有効／矛盾／寄与なし／査定不能） | 根拠（完成形の説明の・行） |
|--------|----------------------------------------|------------------------------|
| component_split_verify | 有効 | ・各エンティティのコンポーネント構成やコンポーネント内部のパラメータをjsonデータとして作成/保存/編集/削除することができる |

---

## セッション 2026-11-20 00:30（/loop 15m・roadmap tick）

### 1. 現在の進捗状況と完了したこと

- **ループ:** `loop-roadmap-execute`（15m）継続
- **着手ブランチ:** `fix/CmpCameraOrthographicDefault`
- **roadmap:** CmpCamera — `Initialize が Orthographic を既定生成`
- 初期化で StageView 同等の透視 FOV を既定設定（スプライト用正射影はウィンドウサイズで維持）
- JSON `perspectiveFovY` / `nearClip` / `farClip` をエディタ定義・インスペクタ・Runtime 適用に追加
- **ビルド:** Cloud VM（Linux）未実行

### 2. 発生した課題または設計上の選択肢

- 2D スプライト描画は引き続き `GetOrthographicMatrix` 依存

### 3. 次に実行すべきステップの提案

- roadmap 次行: 物理演算の基本機能独立（CmpLhRigidBody）

### 4. 実装済みと判断した項目（コード確認・即時 [x]）

- なし

### 5. 保留・着手見送り（§6.3）

- なし

### 6. 用途不明で確認待ち（§6.4）

- なし

### 7. 推測した用途（§6.4）

- **CmpCamera 初期投影**
  - 用途: 機能検証シーンで JSON から透視パラメータを試し、BuildUp と挙動を揃える
  - 使用場面: エディタプレビュー、MainCamera 自動スポーン
  - 用途から決めた仕様: 透視既定 0.5 rad、正射影は UI スプライト用のみ

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（完成形の説明の・行） |
|--------|------|------------------------------|
| Initialize が Orthographic を既定生成 | 有効 | ・ゲーム中にエディタ上でのプレイとゲーム配布のランタイム環境上でのプレイを可能にしてある |

---

## セッション 2026-11-20 00:45（/loop 15m・roadmap tick）

### 1. 現在の進捗状況と完了したこと

- **ループ:** `loop-roadmap-execute`（15m）継続
- **着手ブランチ:** `fix/CmpLhRigidBodyPhysicsEngine`
- **roadmap:** CmpLhRigidBody — 物理演算の基本機能化
- `Physics::PhysicsManager` を追加（重力積分・速度適用・ステージ境界クランプ）
- `EnginePhysicsManager()` を `EngineAccess` 経由で公開
- `CmpLhRigidBody::Update` は PhysicsManager に委譲
- **ビルド:** Cloud VM（Linux）未実行

### 2. 発生した課題または設計上の選択肢

- 衝突応答・LiquidFun 連携は今後の拡張（形状データはコンポーネント側のまま）

### 3. 次に実行すべきステップの提案

- roadmap 次行: 描画への外部アクセス（EngineAccess）

### 4. 実装済みと判断した項目（コード確認・即時 [x]）

- なし

### 5. 保留・着手見送り（§6.3）

- なし

### 6. 用途不明で確認待ち（§6.4）

- なし

### 7. 推測した用途（§6.4）

- **PhysicsManager**
  - 用途: 形状を持たない運動学ステップを AudioManager 同様の基本機能として共通化
  - 使用場面: プレイヤー・弾の `CmpLhRigidBody`、将来の他 Upd からの再利用
  - 用途から決めた仕様: シングルトン + `KinematicStepSettings`

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（完成形の説明の・行） |
|--------|------|------------------------------|
| 物理演算の基本機能独立 | 有効 | ・シーンマネージャーやオーディオマネージャー、入力マネージャー、描画エンジンや物理エンジン(形状データを除く)などの1つしか存在しない仕組み |

---

## セッション 2026-11-20 01:00（/loop 15m・roadmap tick）

### 1. 現在の進捗状況と完了したこと

- **ループ:** `loop-roadmap-execute`（15m）継続
- **着手ブランチ:** `fix/EngineAccessDraw`
- **roadmap:** EngineAccess — 描画リソースへの薄い API
- `EngineGetTexture` / `EngineGetModel` / `EngineSetDirectionalLightData` を追加（`EngineAccess.cpp`）
- 各シーンのライト設定・フェード白テクスチャ・SkyDome モデル取得を移行
- **ビルド:** Cloud VM（Linux）未実行

### 2. 発生した課題または設計上の選択肢

- ボリュームフォグ等は引き続き `Engine::Instance()` 直叩き（別行）

### 3. 次に実行すべきステップの提案

- roadmap 次行: CmpBounceBall 責務分割

### 4. 実装済みと判断した項目（コード確認・即時 [x]）

- なし

### 5. 保留・着手見送り（§6.3）

- なし

### 6. 用途不明で確認待ち（§6.4）

- なし

### 7. 推測した用途（§6.4）

- **EngineAccess 描画 API**
  - 用途: シーンが `AtrumEngine.h` 無しでもライト・テクスチャ・モデルを基本機能経由で触る
  - 使用場面: タイトル／ゲーム／ビルドアップのライト、フェード UI、スカイドーム生成
  - 用途から決めた仕様: 専用 `.cpp` に実装を集約

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（完成形の説明の・行） |
|--------|------|------------------------------|
| EngineAccess 描画 | 有効 | ・基本機能に外部クラスからアクセスできるようになっている |

---

## セッション 2026-11-20 01:15（/loop 15m・roadmap tick）

### 1. 現在の進捗状況と完了したこと

- **ループ:** `loop-roadmap-execute`（15m）継続
- **着手ブランチ:** `fix/CmpBounceBallSplit`
- **roadmap:** CmpBounceBall 責務分割（`GAP-Runtime-CmpBounceBall-001`）
- 新規: `CmpBallKinematics` / `CmpBallAxisAlignedBounce` / `CmpBallBounceImpactFx`
- `CmpBounceBall` は JSON 互換ファサード（`EnsureAndSyncChildComponents`）
- エディタ登録三箇所を同期（verify スクリプト OK）
- **ビルド:** Cloud VM（Linux）未実行

### 2. 発生した課題または設計上の選択肢

- 更新順はエンティティへの追加順に依存（ファサードが先に付く既存デモは問題なし）

### 3. 次に実行すべきステップの提案

- roadmap 次行: BinaryConverter のバッチ／CI フック

### 4. 実装済みと判断した項目（コード確認・即時 [x]）

- なし

### 5. 保留・着手見送り（§6.3）

- なし

### 6. 用途不明で確認待ち（§6.4）

- なし

### 7. 推測した用途（§6.4）

- **CmpBall 部品分割**
  - 用途: パーティクルデモ球だけでなく、運動のみ・バウンスのみ・演出のみをエンティティに選択追加
  - 使用場面: エディタ実験、将来のステージギミック再利用
  - 用途から決めた仕様: 同一 `PHYSICS` グループで部品を合成、`CmpBounceBall` は既存 JSON を維持

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（完成形の説明の・行） |
|--------|------|------------------------------|
| CmpBounceBall 分割 | 有効 | ・機能として成り立つ最小単位の部品がそれぞれクラスに分けられ整理されている |

---

## セッション 2026-11-20 01:30（/loop 15m・roadmap tick）

### 1. 現在の進捗状況と完了したこと

- **ループ:** `loop-roadmap-execute`（15m）継続
- **着手ブランチ:** `chore/BinaryConverterBatchCi`
- **roadmap:** BinaryConverter バッチ／CI（`GAP-Infra-BinaryConverter-001`）
- `batch_obj_to_amesh.py` / `.cmd`、`fixtures/minimal_triangle.obj`
- `verify_binary_converter.py` を追加し `BuildCoverageCheck.yml` に組み込み
- **ビルド:** Cloud VM で verify スクリプト成功（MSBuild 未実行）

### 2. 発生した課題または設計上の選択肢

- リポジトリ内実 OBJ は未同梱のため CI は専用フィクスチャのみ

### 3. 次に実行すべきステップの提案

- roadmap 次行: `GAP-Camera-StageBounds-001`（StageView と StageBounds）

### 4. 実装済みと判断した項目（コード確認・即時 [x]）

- なし

### 5. 保留・着手見送り（§6.3）

- なし

### 6. 用途不明で確認待ち（§6.4）

- なし

### 7. 推測した用途（§6.4）

- **BinaryConverter バッチ／CI**
  - 用途: 配布前パイプラインで OBJ 群を `.amesh` に一括変換し回帰を CI で検知
  - 使用場面: アーティストのエクスポート更新、ビルド前アセット前処理
  - 用途から決めた仕様: `--skip-up-to-date` による差分変換、ヘッダ検証スモーク

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（完成形の説明の・行） |
|--------|------|------------------------------|
| BinaryConverter CI | 有効 | ・ゲーム配布の際、pngやobj、jsonはddsやバイナリなどに圧縮して配布し、ゲーム中に読み込む |

---

## セッション 2026-11-20 01:45（/loop 15m・roadmap tick）

### 1. 現在の進捗状況と完了したこと

- **ループ:** `loop-roadmap-execute`（15m）継続
- **着手ブランチ:** `fix/StageViewStageBounds`
- **roadmap:** StageView がシーン毎フレーミングに追従（`GAP-Camera-StageBounds-001`）
- `StageViewFramingBounds` + `ComputeStageBattleViewPose(bounds)`
- シーン JSON の `stageViewFramingBounds` / 境界マーカー名から解決（`SceneBuildUp`）
- `combat_bounds_sandbox.scene.json` にサンプル bounds を追記
- **ビルド:** Cloud VM（Linux）未実行

### 2. 発生した課題または設計上の選択肢

- `SceneGame` は既定 StageBounds のまま（本編 AABB と一致）
- `stage_view_camera_tuning.json` の実行時読込は別 roadmap 行

### 3. 次に実行すべきステップの提案

- StageViewCameraTuning エディタプレビュー／ホットリロード

### 4. 実装済みと判断した項目（コード確認・即時 [x]）

- なし

### 5. 保留・着手見送り（§6.3）

- なし

### 6. 用途不明で確認待ち（§6.4）

- なし

### 7. 推測した用途（§6.4）

- **シーン単位 StageView フレーミング**
  - 用途: JSON 試遊・デバッグプレイエリアでカメラがプレイ範囲に合わせて引き／寄りする
  - 使用場面: BuildUp ランタイム、`DebugPlayArea` サンドボックス
  - 用途から決めた仕様: 明示 JSON 優先、無ければ `*BoundMarker` エンティティ

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（完成形の説明の・行） |
|--------|------|------------------------------|
| StageView シーン bounds | 有効 | プレイ方針メモ: プレイ中カメラが地形・オブジェクトを見失わず操作に追従する（player_friendly_camera_policy と整合） |

---

## セッション 2026-11-20 02:00（/loop 15m・roadmap tick）

### 1. 現在の進捗状況と完了したこと

- **ループ:** `loop-roadmap-execute`（15m）継続
- **着手ブランチ:** `fix/StageViewTuningPreview`
- **roadmap:** StageViewCamera tuning のエディタプレビュー／ホットリロード
- `StageViewCameraTuning` + ImGui ドック「Stage camera tuning」
- Game ビューで JSON プレビュー ON 時に `SetupStageViewCamera` が tuning を反映（FOV／アスペクト含む）
- **ビルド:** Cloud VM（Linux）未実行

### 2. 発生した課題または設計上の選択肢

- C++ 定数との自動同期は次 roadmap 行（手動反映フローは README 維持）

### 3. 次に実行すべきステップの提案

- `tuning.json` と `StageViewCamera.h` 定数の自動同期

### 4. 実装済みと判断した項目（コード確認・即時 [x]）

- なし

### 5. 保留・着手見送り（§6.3）

- なし

### 6. 用途不明で確認待ち（§6.4）

- なし

### 7. 推測した用途（§6.4）

- **tuning JSON プレビュー**
  - 用途: レベル／カメラデザイン時にエンジン再起動なしで戦闘カメラを試す
  - 使用場面: エンティティエディタ Game ビュー、外部エディタで JSON 保存→自動リロード
  - 用途から決めた仕様: DEVELOPMENT のみ・本番ランタイムは従来 C++ 定数

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（完成形の説明の・行） |
|--------|------|------------------------------|
| tuning プレビュー | 有効 | ・各エンティティのコンポーネント構成や…jsonデータとして…編集（制作ワークフローでカメラ調整をエディタ内で行う） |

---

## セッション 2026-11-20 02:15（/loop 15m・roadmap tick）

### 1. 現在の進捗状況と完了したこと

- **ループ:** `loop-roadmap-execute`（15m）継続
- **着手ブランチ:** `fix/StageViewTuningJsonSync`
- **roadmap:** tuning.json と C++ 定数の自動同期（`GAP-Infra-stage_view_camera_tuning-001`）
- `stage_view_tuning_contract.py` / `verify_stage_view_tuning_sync.py` / `sync_stage_view_tuning_json.py`
- `BuildCoverageCheck.yml` に verify を追加
- **ビルド:** Cloud VM で verify OK（MSBuild 未実行）

### 2. 発生した課題または設計上の選択肢

- 同期は CI 検証＋JSON 書戻しスクリプト（C++ が正本）

### 3. 次に実行すべきステップの提案

- tuning JSON の client 解像度をランタイム参照（次 roadmap 行）

### 4. 実装済みと判断した項目（コード確認・即時 [x]）

- なし

### 5. 保留・着手見送り（§6.3）

- なし

### 6. 用途不明で確認待ち（§6.4）

- なし

### 7. 推測した用途（§6.4）

- **tuning 同期**
  - 用途: カメラ調整 JSON とゲーム内定数のズレを配布前に検知
  - 使用場面: C++ 定数変更後の `sync --write`、PR の BuildCoverageCheck
  - 用途から決めた仕様: 全スカラー1対1契約（bounds・focus 係数含む）

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（完成形の説明の・行） |
|--------|------|------------------------------|
| tuning 同期 | 有効 | ・エディタとランタイム環境上でのプレイ（カメラ方針の一貫性） |

---

## セッション 2026-11-20 02:30（/loop 15m・roadmap tick）

### 1. 現在の進捗状況と完了したこと

- **ループ:** `loop-roadmap-execute`（15m）継続
- **着手ブランチ:** `fix/StageViewClientResolution`
- **roadmap:** tuning JSON の client 解像度を StageView 計算に接続
- `GetStageViewDesignAspectRatio()` + `EnsureDesignBaselineLoaded()`
- **ビルド:** Cloud VM（Linux）未実行

### 2. 発生した課題または設計上の選択肢

- Release では引き続き `Window` 既定値（JSON 未同梱）

### 3. 次に実行すべきステップの提案

- ステージカメラのアスペクトが既定解像度固定（実ウィンドウサイズ追従）

### 4. 実装済みと判断した項目（コード確認・即時 [x]）

- なし

### 5. 保留・着手見送り（§6.3）

- なし

### 6. 用途不明で確認待ち（§6.4）

- なし

### 7. 推測した用途（§6.4）

- **設計解像度の runtime 参照**
  - 用途: tuning JSON で指定したターゲット解像度と戦闘カメラの引きが一致
  - 使用場面: DEVELOPMENT プレイ・Game ビュー試写
  - 用途から決めた仕様: 初回姿勢計算時に JSON を遅延読込

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（完成形の説明の・行） |
|--------|------|------------------------------|
| client 解像度参照 | 有効 | プレイ中カメラが演出側で誘導され、調整データとゲーム計算が一致する |

---

## セッション 2026-11-20 02:45（/loop 15m・roadmap tick）

### 1. 現在の進捗状況と完了したこと

- **ループ:** `loop-roadmap-execute`（15m）継続
- **着手ブランチ:** `fix/StageCameraLiveAspect`
- **roadmap:** ステージカメラのアスペクトを実ウィンドウサイズに追従
- `AtrumEngine::TryGetClientViewportSize` / `EngineTryGetClientViewportSize`
- `GetStageViewDesignAspectRatio` の優先順（プレビュー JSON → 実サイズ → フォールバック）
- **ビルド:** Cloud VM（Linux）未実行

### 2. 発生した課題または設計上の選択肢

- エディタ Game ビューのサブビューポートは別途 `FitCameraPerspectiveToViewport` が投影を上書き

### 3. 次に実行すべきステップの提案

- `verify_camera_policy` の CI 組込みと AllowDebugCameraControl 検査

### 4. 実装済みと判断した項目（コード確認・即時 [x]）

- なし

### 5. 保留・着手見送り（§6.3）

- なし

### 6. 用途不明で確認待ち（§6.4）

- なし

### 7. 推測した用途（§6.4）

- **ライブアスペクト**
  - 用途: ウィンドウリサイズ後も戦闘カメラの水平フィットがずれにくい
  - 使用場面: 本編プレイ、Development フルウィンドウ
  - 用途から決めた仕様: JSON プレビュー時のみ設計解像度を優先

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（完成形の説明の・行） |
|--------|------|------------------------------|
| ライブアスペクト | 有効 | プレイ中カメラが地形・オブジェクトを見失わず操作に追従する（player_friendly_camera_policy） |

---

## セッション 2026-11-20 03:00（/loop 15m・roadmap tick）

### 1. 現在の進捗状況と完了したこと

- **ループ:** `loop-roadmap-execute`（15m）継続
- **着手ブランチ:** `chore/CameraPolicyCiVerify`
- **roadmap:** カメラ方針 CI + `AllowDebugCameraControl` 検査（2 行）
- `verify_camera_policy.py` 拡張、`BuildCoverageCheck.yml` に追加
- **ビルド:** Cloud VM で verify OK

### 2. 発生した課題または設計上の選択肢

- 本編シーンは `IsStageViewCameraLocked` が主ガード（ヘッダで `AllowDebug -> true` だけ禁止）

### 3. 次に実行すべきステップの提案

- `GAP-Scene-SceneMovieRegistry-001`（ムービーシーン起動配線）

### 4. 実装済みと判断した項目（コード確認・即時 [x]）

- なし

### 5. 保留・着手見送り（§6.3）

- なし

### 6. 用途不明で確認待ち（§6.4）

- なし

### 7. 推測した用途（§6.4）

- **カメラ方針 CI**
  - 用途: 新シーン追加時にプレイヤー向けカメラ固定を回帰で守る
  - 使用場面: PR／Cloud Agent の BuildCoverageCheck
  - 用途から決めた仕様: EntityEditor のデバッグ入力委譲パターンを静的検査

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（完成形の説明の・行） |
|--------|------|------------------------------|
| カメラ方針 CI | 有効 | プレイ中カメラは演出側が誘導（player_friendly_camera_policy） |

---

## セッション 2026-11-20 03:15（/loop 15m・roadmap tick）

### 1. 現在の進捗状況と完了したこと

- **ループ:** `loop-roadmap-execute`（15m）継続
- **着手ブランチ:** `fix/SceneMovieRegistry`
- **roadmap:** ムービーシーン起動のゲームプレイ配線（`GAP-Scene-SceneMovieRegistry-001`）
- Release タイトルで決定→`SceneMovieRegistry::Launch`→本編 Game
- パス定数 `GameAssets::kTitleIntroMovieTrack`
- **ビルド:** Cloud VM（Linux）未実行

### 2. 発生した課題または設計上の選択肢

- Development+ImGui 起動は従来どおりエディタ直行（タイトル経由なし）

### 3. 次に実行すべきステップの提案

- `GAP-Scene-SceneBuildUpRegistry-001`（エディタから最小検証シーン）

### 4. 実装済みと判断した項目（コード確認・即時 [x]）

- なし

### 5. 保留・着手見送り（§6.3）

- なし

### 6. 用途不明で確認待ち（§6.4）

- なし

### 7. 推測した用途（§6.4）

- **タイトル→ムービー→本編**
  - 用途: 配布ビルドでカットシーン JSON を本フローに載せる
  - 使用場面: タイトルでスタート決定後、短いカメラトラック再生
  - 用途から決めた仕様: 終了後は `SceneGame` へフェード復帰

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（完成形の説明の・行） |
|--------|------|------------------------------|
| SceneMovie 配線 | 有効 | シーン内ムービーが再生完了まで破綻しない（scene_movie.md） |

---

## セッション 2026-11-20 03:30（/loop 15m・roadmap tick）

### 1. 現在の進捗状況と完了したこと

- **ループ:** `loop-roadmap-execute`（15m）継続
- **着手ブランチ:** `fix/SceneBuildUpEditorLaunch`
- **roadmap:** エディタから SceneBuildUp 起動（`GAP-Scene-SceneBuildUpRegistry-001`）
- メニュー／プロジェクトパネルに `SceneBuildUp で実行`、デバッグメニューに DebugPlayArea テンプレ
- `EditorAssetsPaths::kDebugPlayAreaTemplateScene`
- **ビルド:** Cloud VM（Linux）未実行

### 2. 発生した課題または設計上の選択肢

- エディタ内 Runtime タブ試行（`requestPlayRuntimeScene`）とフル遷移を分離

### 3. 次に実行すべきステップの提案

- `GAP-Scene-SceneMovie-004`（returnScene null 時の復帰）

### 4. 実装済みと判断した項目（コード確認・即時 [x]）

- なし

### 5. 保留・着手見送り（§6.3）

- なし

### 6. 用途不明で確認待ち（§6.4）

- なし

### 7. 推測した用途（§6.4）

- **SceneBuildUp エディタ配線**
  - 用途: 本編外で JSON シーンをフルランタイム検証
  - 使用場面: コンポーネント試験、DebugPlayArea テンプレのワンクリック起動
  - 用途から決めた仕様: 戻り先は `SceneEntityEditor`

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（完成形の説明の・行） |
|--------|------|------------------------------|
| BuildUp エディタ起動 | 有効 | ・シーンの構成を保存し、エディタ上／ランタイム環境上でのプレイを可能に |

---

## セッション 2026-11-20 03:45（/loop 15m・roadmap tick）

### 1. 現在の進捗状況と完了したこと

- **ループ:** `loop-roadmap-execute`（15m）継続
- **着手ブランチ:** `fix/SceneMovieReturnScene`
- **roadmap:** `GAP-Scene-SceneMovie-004`（returnScene null 時の復帰）
- `ResolveSceneMovieReturnTarget` を追加（Release: `SceneGame`、Development: `SceneEntityEditor`）
- `RequestReturnToCaller` が常に遷移可能、Esc も returnScene 未設定で有効
- **ビルド:** Cloud VM（Linux）未実行

### 2. 発生した課題または設計上の選択肢

- なし

### 3. 次に実行すべきステップの提案

- 次の未完了 roadmap 行（例: SceneMovie リムライト等）

### 4. 実装済みと判断した項目（コード確認・即時 [x]）

- なし

### 5. 保留・着手見送り（§6.3）

- なし

### 6. 用途不明で確認待ち（§6.4）

- なし

### 7. 推測した用途（§6.4）

- **SceneMovie 復帰フォールバック**
  - 用途: Launch 誤用やトラック失敗時もプレイ／編集へ戻れる
  - 使用場面: タイトルイントロ以外の単体ムービー試験
  - 用途から決めた仕様: 明示 returnScene を優先、未設定はビルド種別に応じた既定シーン

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（完成形の説明の・行） |
|--------|------|------------------------------|
| Movie 復帰 | 有効 | ・シーン内ムービー／カットシーンが再生完了まで破綻しない |

---

## セッション 2026-11-20 04:00（/loop 15m・roadmap tick）

### 1. 現在の進捗状況と完了したこと

- **ループ:** `loop-roadmap-execute`（15m）継続
- **着手ブランチ:** `fix/SceneGameOrderUpdateStop`
- **roadmap:** `GAP-Scene-GameOrder-001`（フェードイン中のシーン Update）
- `CommandChangeScene::AllowsCurrentSceneUpdate`（フェードアウト中のみ Update 停止）
- `GameOrder::Run` が上記で判定
- **ビルド:** Cloud VM（Linux）未実行

### 2. 発生した課題または設計上の選択肢

- BGM 遅延など `GetIsExecute` 依存はフェード全体のまま維持

### 3. 次に実行すべきステップの提案

- `GAP-Scene-SceneCameraRig-001`（FindPrimaryCmpCamera）

### 4. 実装済みと判断した項目（コード確認・即時 [x]）

- なし

### 5. 保留・着手見送り（§6.3）

- なし

### 6. 用途不明で確認待ち（§6.4）

- なし

### 7. 推測した用途（§6.4）

- **フェードイン中 Update**
  - 用途: ロード直後もカメラ・エンティティが時間進行する
  - 使用場面: タイトル→ゲーム／エディタ→BuildUp 遷移
  - 用途から決めた仕様: 暗転完了までは旧シーン Update 停止

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（完成形の説明の・行） |
|--------|------|------------------------------|
| GameOrder Update | 有効 | ・プレイヤーがシーンを開き、主要オブジェクトとカメラが安定して動作する |

---

## セッション 2026-11-20 04:15（/loop 15m・roadmap tick）

### 1. 現在の進捗状況と完了したこと

- **ループ:** `loop-roadmap-execute`（15m）継続
- **着手ブランチ:** `fix/SceneCameraRigFindPrimary`
- **roadmap:** `GAP-Scene-SceneCameraRig-001`
- `ScenePrimaryCameraResolve` で rig + `GetSupplementalCameraEntities` を統合検索
- `SceneBuildUp` が JSON 実体 `entities_` を supplemental として公開
- **ビルド:** Cloud VM（Linux）未実行

### 2. 発生した課題または設計上の選択肢

- rig 優先、未設定時は supplemental（JSON）を参照

### 3. 次に実行すべきステップの提案

- `GAP-Scene-SceneBuildUp-009` は supplemental 経由で解消済みの可能性（§6.2 確認）

### 4. 実装済みと判断した項目（コード確認・即時 [x]）

- なし

### 5. 保留・着手見送り（§6.3）

- なし

### 6. 用途不明で確認待ち（§6.4）

- なし

### 7. 推測した用途（§6.4）

- **BuildUp JSON カメラ**
  - 用途: DebugPlayArea の `camera.main` を primary に
  - 使用場面: SceneBuildUp フルランタイム検証
  - 用途から決めた仕様: 自動スポーンは JSON にカメラ無しのときのみ

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（完成形の説明の・行） |
|--------|------|------------------------------|
| primary 解決 | 有効 | ・開発者がエディタ既定の機能検証シーンから、エディタ外ランタイム機能だけを最小構成で試せる |

---

## セッション 2026-11-20 04:30（/loop 15m・roadmap tick）

### 1. 現在の進捗状況と完了したこと

- **ループ:** `loop-roadmap-execute`（15m）継続
- **着手ブランチ:** `fix/SceneBuildUpCameraConnect`
- **roadmap:** `GAP-Scene-SceneBuildUp-009`（§6.2 即時完了）
- 前 tick の `GetSupplementalCameraEntities` + `Scene::GetCamera(this)` で要件充足を確認
- **ビルド:** 変更なしのためスキップ

### 2. 発生した課題または設計上の選択肢

- なし

### 3. 次に実行すべきステップの提案

- `CommandScene 遷移要求・Development 起動シーン不一致`

### 4. 実装済みと判断した項目（コード確認・即時 [x]）

- **entities_ と GetCamera の接続**（`SceneBuildUp::GetSupplementalCameraEntities`・`ScenePrimaryCameraResolve`・develop `6f1196a`）

### 5. 保留・着手見送り（§6.3）

- なし

### 6. 用途不明で確認待ち（§6.4）

- なし

### 7. 推測した用途（§6.4）

- なし（確認のみ tick）

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（完成形の説明の・行） |
|--------|------|------------------------------|
| BuildUp カメラ接続 | 有効（実装済み） | ・開発者がエディタ既定の機能検証シーンから、エディタ外ランタイム機能だけを最小構成で試せる |

---

## セッション 2026-11-20 04:45（/loop 15m・roadmap tick）

### 1. 現在の進捗状況と完了したこと

- **ループ:** `loop-roadmap-execute`（15m）継続
- **着手ブランチ:** `fix/CommandSceneDevStartup`
- **roadmap:** CommandScene と Development 起動／PlayRequest の整合
- `ResolveDevelopmentBootTarget`（PlayRequest → launch_config、JSON 検証、Clear）
- DebugPlayArea は BuildUp 直起動、他は EntityEditor に boot JSON
- Release: 起動 JSON 検証、未設定時 DebugPlayArea テンプレへフォールバック
- **ビルド:** Cloud VM（Linux）未実行

### 2. 発生した課題または設計上の選択肢

- 通常シーン JSON はエディタ起動＋パス反映、機能検証は BuildUp

### 3. 次に実行すべきステップの提案

- SceneMovie トラックパス EditorAssets 解決

### 4. 実装済みと判断した項目（コード確認・即時 [x]）

- なし

### 5. 保留・着手見送り（§6.3）

- なし

### 6. 用途不明で確認待ち（§6.4）

- なし

### 7. 推測した用途（§6.4）

- **起動シーン配線**
  - 用途: 起動シーン設定と実際の初回シーンを一致させる
  - 使用場面: DebugPlayArea 試験、launch_config 共有
  - 用途から決めた仕様: PlayRequest 優先、消費後 Clear

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（完成形の説明の・行） |
|--------|------|------------------------------|
| CommandScene 起動 | 有効 | ・開発者がエディタ既定の機能検証シーンから、エディタ外ランタイム機能だけを最小構成で試せる |

---

## セッション 2026-11-20 05:00（/loop 15m・roadmap tick）

### 1. 現在の進捗状況と完了したこと

- **ループ:** `loop-roadmap-execute`（15m）継続
- **着手ブランチ:** `fix/SceneMovieTrackPath`
- **roadmap:** movie トラックパス EditorAssets 解決（GAP-Scene-SceneMovie-006）
- `ResolveMovieCameraTrackJsonPath` + `EditorAssets::kMovieCameraTracksDirectory`
- `MovieCameraController::TryBeginFromJsonFile` で解決後ロード
- `GameAssets::kTitleIntroMovieTrack` を EditorAssets 定数に委譲
- **ビルド:** Cloud VM（Linux）未実行

### 2. 発生した課題または設計上の選択肢

- ファイル名のみ・`Camera/` 相対も候補探索

### 3. 次に実行すべきステップの提案

- トラック読込失敗時の UX（roadmap 次行）

### 4. 実装済みと判断した項目（コード確認・即時 [x]）

- なし

### 5. 保留・着手見送り（§6.3）

- なし

### 6. 用途不明で確認待ち（§6.4）

- なし

### 7. 推測した用途（§6.4）

- **ムービートラックパス**
  - 用途: カットシーン JSON を作業ディレクトリ非依存で読む
  - 使用場面: タイトルイントロ、SceneMovie Launch
  - 用途から決めた仕様: 存在確認して最初に見つかったパスを採用

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（完成形の説明の・行） |
|--------|------|------------------------------|
| トラックパス | 有効 | ・シーン内ムービー／カットシーンが再生完了まで破綻しない |

---

## セッション 2026-11-20 05:15（/loop 15m・roadmap tick）

### 1. 現在の進捗状況と完了したこと

- **ループ:** `loop-roadmap-execute`（15m）継続
- **着手ブランチ:** `fix/SceneMovieTrackLoadFail`
- **roadmap:** トラック読込失敗時の即復帰を改善
- 失敗時は StageView + 2.5 秒待機、Development で ImGui にパス表示
- **ビルド:** Cloud VM（Linux）未実行

### 2. 発生した課題または設計上の選択肢

- Release は UI なしだが Esc／待機で即フェード復帰を回避

### 3. 次に実行すべきステップの提案

- SkyDome とトラック先頭姿勢（roadmap 次行）

### 4. 実装済みと判断した項目（コード確認・即時 [x]）

- なし

### 5. 保留・着手見送り（§6.3）

- なし

### 6. 用途不明で確認待ち（§6.4）

- なし

### 7. 推測した用途（§6.4）

- **トラック失敗 UX**
  - 用途: 壊れた JSON をデバッグしやすくする
  - 使用場面: 制作中のトラック差し替えミス
  - 用途から決めた仕様: 要求／解決パスを文字列で表示

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（完成形の説明の・行） |
|--------|------|------------------------------|
| トラック失敗 | 有効 | ・シーン内ムービー／カットシーンが再生完了まで破綻しない |

---

## セッション 2026-11-20 05:30（/loop 15m・roadmap tick）

### 1. 現在の進捗状況と完了したこと

- **ループ:** `loop-roadmap-execute`（15m）継続
- **着手ブランチ:** `fix/SceneMovieSkyDomeCamera`
- **roadmap:** SkyDome をトラック先頭姿勢適用後に生成
- `EnterScene` で `TryBeginFromJsonFile` → 失敗時 `SetupStageViewCamera` の後に `SpawnVisuals`
- `CreateSkyDomeBoundToCamera` で生成処理を分離
- **ビルド:** Cloud VM（Linux）未実行

### 2. 発生した課題または設計上の選択肢

- なし

### 3. 次に実行すべきステップの提案

- トラック開始失敗時 SetupStageViewCamera（§6.2 確認済みの可能性）

### 4. 実装済みと判断した項目（コード確認・即時 [x]）

- なし

### 5. 保留・着手見送り（§6.3）

- なし

### 6. 用途不明で確認待ち（§6.4）

- なし

### 7. 推測した用途（§6.4）

- **SkyDome カメラ順序**
  - 用途: カットシーン開始フレームの背景がカメラと一致
  - 使用場面: タイトルイントロムービー
  - 用途から決めた仕様: トラック先頭キーフレーム適用後に SkyDome 生成

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（完成形の説明の・行） |
|--------|------|------------------------------|
| SkyDome 順序 | 有効 | ・シーン内ムービー／カットシーンが再生完了まで破綻しない |

---

## セッション 2026-11-20 05:45（/loop 15m・roadmap tick）

### 1. 現在の進捗状況と完了したこと

- **ループ:** `loop-roadmap-execute`（15m）継続
- **着手ブランチ:** `fix/SceneMovieStageViewOnFail`
- **roadmap:** GAP-Scene-SceneMovie-007（失敗時の StageView フォールバック削除）
- `EnterScene` のトラック失敗パスから `SetupStageViewCamera` を除去
- 失敗待ち UX（ImGui／保持時間）は維持
- **ビルド:** Cloud VM（Linux）未実行

### 2. 発生した課題または設計上の選択肢

- scene_movie 方針「失敗時は即 return」と、前 tick の表示用 StageView が矛盾していたため方針優先

### 3. 次に実行すべきステップの提案

- ムービー BGM／復帰ポリシー（roadmap 次行）

### 4. 実装済みと判断した項目（コード確認・即時 [x]）

- なし

### 5. 保留・着手見送り（§6.3）

- なし

### 6. 用途不明で確認待ち（§6.4）

- なし

### 7. 推測した用途（§6.4）

- **失敗時カメラ**
  - 用途: ムービー専用ポリシーをトラック失敗でも一貫
  - 使用場面: 壊れたトラックの短いエラー表示→復帰
  - 用途から決めた仕様: MovieCamera のみ、StageView へ逃げない

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（完成形の説明の・行） |
|--------|------|------------------------------|
| StageView 削除 | 有効 | ・シーン内ムービー／カットシーンが再生完了まで破綻しない |

---

## セッション 2026-11-20 06:00（/loop 15m・roadmap tick）

### 1. 現在の進捗状況と完了したこと

- **ループ:** `loop-roadmap-execute`（15m）継続
- **着手ブランチ:** `fix/SceneMovieBgmPolicy`
- **roadmap:** SceneMovie オーディオ方針のコード化（GAP-Scene-SceneMovie-001）
- `SceneMovieAudioPolicy`（定数・Enter/Update/復帰前 BGM 登録/Exit）
- `RequestReturnToCaller` で Game/Title 向け遅延 BGM
- **ビルド:** Cloud VM（Linux）未実行

### 2. 発生した課題または設計上の選択肢

- トラック JSON 音声は将来拡張としてフラグのみ

### 3. 次に実行すべきステップの提案

- 遷移予約後のカメラ／SkyDome 停止（roadmap 次行）

### 4. 実装済みと判断した項目（コード確認・即時 [x]）

- なし

### 5. 保留・着手見送り（§6.3）

- なし

### 6. 用途不明で確認待ち（§6.4）

- なし

### 7. 推測した用途（§6.4）

- **ムービー BGM 方針**
  - 用途: タイトルイントロ→本編の音体験を切替で維持
  - 使用場面: SceneTitle→SceneMovie→SceneGame
  - 用途から決めた仕様: ムービー中は BGM 無し、復帰は遅延 BGM + 先シーン Enter

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（完成形の説明の・行） |
|--------|------|------------------------------|
| Movie オーディオ | 有効 | ・プレイ中に BGM／SE が途切れず再生され、シーン遷移後も期待どおり鳴り続ける |

---

## セッション 2026-11-20 06:15（/loop 15m・roadmap tick）

### 1. 現在の進捗状況と完了したこと

- **ループ:** `loop-roadmap-execute`（15m）継続
- **着手ブランチ:** `fix/SceneMoviePendingChangeStop`
- **roadmap:** GAP-Scene-SceneMovie-008（復帰フェード中の視覚更新）
- `TickSceneMovieVisualsDuringPendingTransition` で pending 中も camera/skyDome Update
- **ビルド:** Cloud VM（Linux）未実行

### 2. 発生した課題または設計上の選択肢

- 入力・再遷移は pending 中は従来どおり抑止

### 3. 次に実行すべきステップの提案

- SceneMovie リムライト（roadmap 次行）

### 4. 実装済みと判断した項目（コード確認・即時 [x]）

- なし

### 5. 保留・着手見送り（§6.3）

- なし

### 6. 用途不明で確認待ち（§6.4）

- なし

### 7. 推測した用途（§6.4）

- **フェード中の背景**
  - 用途: 復帰フェード中にスカイが止まって見えない
  - 使用場面: ムービー終了→本編
  - 用途から決めた仕様: pending 中は視覚 Update のみ継続

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（完成形の説明の・行） |
|--------|------|------------------------------|
| pending 更新 | 有効 | ・シーン内ムービー／カットシーンが再生完了まで破綻しない |

---

## セッション 2026-11-20 06:30（/loop 15m・roadmap tick）

### 1. 現在の進捗状況と完了したこと

- **ループ:** `loop-roadmap-execute`（15m）継続
- **着手ブランチ:** `fix/SceneMovieRimLight`
- **roadmap:** SceneMovie リム・ボリュームフォグ（GAP-Scene-SceneMovie-003）
- `ApplySceneMovieStageLighting`（本編に近いリム／フォグ／ゴッドレイ）
- **ビルド:** Cloud VM（Linux）未実行

### 2. 発生した課題または設計上の選択肢

- カットシーン用に本編より弱めの霧密度

### 3. 次に実行すべきステップの提案

- GameOrder の音声管理（roadmap 次行）

### 4. 実装済みと判断した項目（コード確認・即時 [x]）

- なし

### 5. 保留・着手見送り（§6.3）

- なし

### 6. 用途不明で確認待ち（§6.4）

- なし

### 7. 推測した用途（§6.4）

- **ムービー見た目**
  - 用途: スカイドーム背景のシルエット可読性
  - 使用場面: タイトルイントロカット
  - 用途から決めた仕様: SceneGame 系プリセットを弱めに適用

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（完成形の説明の・行） |
|--------|------|------------------------------|
| Movie ライティング | 有効 | ・ランタイムでライト・フォグ等の見た目がデータどおり反映される |

---

## セッション 2026-11-20 06:45（/loop 15m・roadmap tick）

### 1. 現在の進捗状況と完了したこと

- **ループ:** `loop-roadmap-execute`（15m）継続
- **着手ブランチ:** `fix/GameOrderAudioTick`
- **roadmap:** GAP-Scene-GameOrder-002（フレームループの音声管理）
- `TickGameOrderFrameAudio` を `GameOrder::Run` に組み込み
- `AudioManager::Update` を Engine::Process から GameOrder 側へ移動
- **ビルド:** Cloud VM（Linux）未実行

### 2. 発生した課題または設計上の選択肢

- BGM ダックは引き続き各シーン＋CommandChangeScene が担当

### 3. 次に実行すべきステップの提案

- SceneGame 見た目パラメータ（roadmap 先頭の Game 節）

### 4. 実装済みと判断した項目（コード確認・即時 [x]）

- なし

### 5. 保留・着手見送り（§6.3）

- なし

### 6. 用途不明で確認待ち（§6.4）

- なし

### 7. 推測した用途（§6.4）

- **GameOrder 音声 tick**
  - 用途: ストリーミング SE/BGM をシーン更新後に補充
  - 使用場面: 毎フレームの本編プレイ
  - 用途から決めた仕様: シーン Update 後・描画前に `audio_->Update()`

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（完成形の説明の・行） |
|--------|------|------------------------------|
| GameOrder 音声 | 有効 | ・プレイ中に BGM／SE が途切れず再生され、シーン遷移後も期待どおり鳴り続ける |

---

## セッション 2026-11-20 07:00（/loop 15m・roadmap tick）

### 1. 現在の進捗状況と完了したこと

- **着手ブランチ:** `fix/SceneGameLightingData`
- **roadmap:** GAP Render SceneGame 見た目パラメータのデータ化
- `game_scene_lighting.json`（Assets / EditorAssets）
- `ApplySceneGameLightingFromProfile` で JSON をエンジンへ反映
- `SceneGame::DrawScene` から直書き `SetupGameLighting` を除去

### 7. 推測した用途（§6.4）

- **本編見た目プロファイル**
  - 用途: レベル制作でフォグ・リム・影を JSON 差し替え
  - 使用場面: 本編 SceneGame の毎フレーム描画前
  - 仕様: 読込失敗時は従来リテラル相当のデフォルトへフォールバック

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（完成形の説明の・行） |
|--------|------|------------------------------|
| SceneGame 見た目データ | 有効 | ・ランタイムでライト・フォグ等の見た目がデータどおり反映される |

---

## セッション 2026-11-20 07:15（/loop 15m・roadmap tick）

### 1. 完了

- **ブランチ:** `fix/SceneGameBossDefeatBgm`
- **roadmap:** OnBossDefeated の BGM 即停止を廃止
- 撃破演出 `IsFinished` 後・リザルト遷移直前に `StopBgmLoop`
- 演出オブジェクト無しのフォールバック遷移時のみ従来どおり停止

### 7. 推測した用途（§6.4）

- ボス撃破演出中は戦闘 BGM を維持し、リザルトへ切り替える直前で停止

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| 撃破 BGM | 有効 | プレイ中に BGM／SE が途切れず再生され、シーン遷移後も期待どおり鳴り続ける |

---

## セッション 2026-11-20 07:30（/loop 15m・roadmap tick）

### 完了

- `BootSceneGameOperatorGuidance` で読込成否を保持
- `LoadFromJsonFile` を try/catch 化し失敗時は状態をクリア（代替案内なし）
- 読込失敗時は Update／Draw／OnBattleReady をスキップ

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| operator_guidance 入場 | 有効 | 初回利用者がオペレータ案内に沿って主要操作を完了できる（JSON 欠落時は本編のみ継続） |

---

## セッション 2026-11-20 07:45（/loop 15m・roadmap tick）

### 完了

- `ApplySceneGameVisualPresetOnEnter` を `EnterScene` 先頭で呼び出し
- プロファイル適用後に `FlushDirectionalLightConstantBuffer` で初回描画前に CB 同期

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| Enter 見た目 | 有効 | ランタイムでライト・フォグ等の見た目がデータどおり反映される |

---

## セッション 2026-11-20 08:00（/loop 15m・roadmap tick）

### 完了

- 入場順を `SpawnCombatEntities` → イントロカメラ → `SpawnEnvironmentEntities` に分割
- SkyDome がイントロ開始姿勢の `GetCamera()` を参照するよう順序を修正

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| 入場スポーン順 | 有効 | プレイヤーがシーンを開き、主要オブジェクトとカメラが安定して動作する |

---

## セッション 2026-11-20 08:15（/loop 15m・roadmap tick）

### 完了

- ボス撃破演出中に `SyncSceneBgmDuringBossDeathPresentation` で BGM 同期／再開
- `MaintainSceneBgmIfNeeded` の撃破全停止を自機撃墜時のみに限定

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| 撃破演出 BGM | 有効 | プレイ中に BGM／SE が途切れず再生され、シーン遷移後も期待どおり鳴り続ける |

---

## セッション 2026-11-20 08:30（/loop 15m・roadmap tick）

### 完了

- `SceneGameDefeatAudioPolicy` で撃破／撃墜の BGM 方針を集約
- ボス: 演出完了まで維持、自機: スロー化のみ（即停止しない）

### 4. 実装済みと判断した項目（§6.2）

- ボス即 Stop 廃止は前 tick 済み。本 tick でポリシー化して roadmap 項目を [x]

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| 撃破 BGM 対称性 | 有効 | プレイ中に BGM／SE が途切れず再生され、シーン遷移後も期待どおり鳴り続ける |

---

## セッション 2026-11-20 08:45（/loop 15m・roadmap tick）

### 完了

- イントロ中に `UpdateSceneGameIntroBarrageAudio` でボス `Update` を実行（弾幕 SE）
- 自機 Update・LateResolve は未到達のまま。戦闘パーティクルもイントロ分岐で tick

### 7. 推測した用途

- カメラ演出中もボス弾の発射音で開幕の密度を出す（操作不可のまま）

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| イントロ SE | 有効 | プレイ中に BGM／SE が途切れず再生され、シーン遷移後も期待どおり鳴り続ける |

---

## セッション 2026-11-20 09:00（/loop 15m・roadmap tick）

### 完了

- イントロ中は `MaintainSceneBgmIfNeeded(false)` で `PlayBgmLoop` 再発火を抑止
- フェード同期・一時停止再開のみ継続

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| イントロ BGM | 有効 | プレイ中に BGM／SE が途切れず再生され、シーン遷移後も期待どおり鳴り続ける |

---

## セッション 2026-11-20 09:15（/loop 15m・roadmap tick）

### 完了

- `SyncSceneBgmDuringDefeatPresentation` をボス／自機撃墜演出の両 Update で実行
- pendingDefeat 中のフル Maintain は抑止し、演出中はストリーム同期のみ維持

### 4. 実装済みと判断した項目（§6.2）

- ボス側 Sync は前 tick 済み。自機撃墜分を補完して roadmap [x]

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| pendingDefeat BGM | 有効 | プレイ中に BGM／SE が途切れず再生され、シーン遷移後も期待どおり鳴り続ける |

---

## セッション 2026-11-20 09:30（/loop 15m・roadmap tick）

### 完了

- `ReleaseSceneGameBgmOnExitScene` でフェード中は BGM を `CommandChangeScene` へ引き継ぎ
- `OnSceneTransitionEffectFinished` でフェードイン完了後に停止

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| Exit BGM フェード | 有効 | プレイ中に BGM／SE が途切れず再生され、シーン遷移後も期待どおり鳴り続ける |

---

## セッション 2026-11-20 09:45（/loop 15m・roadmap tick）

### 完了

- 遷移演出中も `MaintainSceneBgmIfNeeded(false)` で同期・再開のみ実行
- フル Maintain（PlayBgmLoop 再発火）は従来どおり遷移中はスキップ

### 4. 実装済みと判断した項目（§6.2）

- GAP 文言は「Maintain 未到達」が課題。同期専用 tick 追加で [x]

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| 遷移 BGM 維持 | 有効 | プレイ中に BGM／SE が途切れず再生され、シーン遷移後も期待どおり鳴り続ける |

---

## セッション 2026-11-20 10:00（/loop 15m・roadmap tick）

### 完了

- `PushToEngine` で Fog/GodRays/Shadow の SetSettings 後に SetDirectionalLightData＋Flush
- volumetric_fog.md の CB 反映順を修正（旧 SetupGameLighting 相当）

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| Fog Flush | 有効 | ランタイムでライト・フォグ等の見た目がデータどおり反映される |

---

## セッション 2026-11-20 10:15（/loop 15m・roadmap tick）

### 完了

- `SceneTitle::ExitScene` で `ReleaseSceneBgmOnExitDuringFade`（Game と共用）
- 関数をシーン横断名に整理

### 注意

- ファイル先頭の次 `- [ ]` は CommandScene「Development 起動…」（roadmap 310 行）。本 tick はブランチ `fix/SceneTitleFadeExitBgm` に合わせ SceneTitle を実施

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| Title Exit BGM | 有効 | プレイ中に BGM／SE が途切れず再生され、シーン遷移後も期待どおり鳴り続ける |

---

## セッション 2026-11-20 10:30（/loop 15m・roadmap tick）

### 完了

- `Assets/Scenes` 指定時は Development でも Release と同様 `SceneBuildUp` 起動（戻り先 Title）
- DebugPlayArea / EditorAssets は従来どおり BuildUp+Editor または EntityEditor

### 7. 推測した用途

- launch_config の本番シーン JSON で開発ビルドからプレイ経路を再現

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| Dev 起動経路 | 有効 | プレイヤーがシーンを開き、主要オブジェクトとカメラが安定して動作する |

---

## セッション 2026-11-20 10:45（/loop 15m・roadmap tick）

### 完了

- `SceneTransitionBgmPolicy` で Play 向け遷移 BGM 方針を文書化（ダック→Exit 引き継ぎ→完了時 Stop→deferred Enter）
- `SceneResult::ExitScene` も `ReleaseSceneBgmOnExitDuringFade` に統一
- CommandScene::ChangeScene にポリシー参照コメント

### 既存実装の課題点

- 二重トラックの音量クロスフェードは未実装（方針ヘッダに将来拡張を明記）

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| 遷移 BGM ポリシー | 有効 | プレイ中に BGM／SE が途切れず再生され、シーン遷移後も期待どおり鳴り続ける |

---

## セッション 2026-11-20 10:48（/loop 15m・roadmap tick）

### 完了

- `CommandChangeScene::Set` を遷移実行中も最後の要求を保持（フェードアウト中は nextScene 差し替え、以降はキュー）
- 遷移完了直後に `TryStartQueuedSet` で連続遷移を継続

### 推測した用途（§6.4）

- プレイ中の連打やロード直後の戻りで、最後に選んだシーンへ意図どおり遷移する

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| 遷移中 Set キュー | 有効 | プレイヤーがシーンを開き、主要オブジェクトとカメラが安定して動作する |

---

## セッション 2026-11-20 10:50（/loop 15m・roadmap tick）

### 完了

- `SceneBuildUpPlayerSilhouetteDraw` で CmpParry 付きルートエンティティを自機とみなし、本編同様 `PlayerTerrainSilhouette` を描画末尾に呼ぶ
- Development ランタイムビュー・Release DrawScene の両経路に接続

### 推測した用途（§6.4）

- BuildUp 検証シーンで地形に隠れた自機位置を視認し、ステージカメラでの操作確認をしやすくする

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| BuildUp 地形シルエット | 有効 | プレイヤーがカメラを直接操作せずとも自動でカメラが動作し、見やすいカメラワークをとるようになっている |

---

## セッション 2026-11-20 11:05（/loop 15m・roadmap tick）

### 完了

- `SceneBuildUpRuntimeVisualApply` 経由で `game_scene_lighting.json` と同一のフォグ・リム・ゴッドレイ・シャドウパイプラインを BuildUp に接続
- `EnterScene` と毎フレーム描画前の `SetupRuntimeLighting` で反映

### 推測した用途（§6.4）

- エディタ JSON シーンの実行時プレビューで本編に近いライティングを確認する制作・デバッグ

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| BuildUp 見た目パイプライン | 有効 | ランタイムでライト・フォグ等の見た目がデータどおり反映される（エンジン前提・見た目） |

---

## セッション 2026-11-20 11:18（/loop 15m・roadmap tick）

### 完了

- `ResolveSceneBuildUpJsonPath` で `Assets/Scenes`・`EditorAssets` 配下とファイル名のみの指定を cwd 非依存で解決
- `SceneBuildUpRegistry::Launch` で正規化済みパスを保持

### 推測した用途（§6.4）

- 配布 exe・別作業ディレクトリからも同一の BuildUp 検証シーン JSON を開く

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| sceneJsonPath 解決 | 有効 | プレイヤーがシーンを開き、主要オブジェクトとカメラが安定して動作する |

---

## セッション 2026-11-20 11:32（/loop 15m・roadmap tick）

### 完了

- `SceneBuildUpStageCameraFraming` で `stageViewFramingBounds`／境界マーカーを `StageViewFramingBounds` へ取り込み
- `Update`・実行時ビュー・Release 描画で `MaintainLockedStageCamera`（Publish + SetupStageViewCamera + PrepareCamerasForDraw）

### 推測した用途（§6.4）

- JSON 検証シーン（combat_bounds_sandbox 等）を本編相当のステージカメラ枠でプレビューする

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| BuildUp 境界→カメラ | 有効 | プレイヤーがカメラを直接操作せずとも自動でカメラが動作し、見やすいカメラワークをとるようになっている |

---

## セッション 2026-11-20 11:48（/loop 15m・roadmap tick）

### 完了

- シーン JSON 読込失敗時はデフォルト 1 体へ置換せず、空シーン＋失敗メッセージ
- Development は ImGui、Release は暗転オーバーレイで可視化

### 推測した用途（§6.4）

- 誤パスや壊れた JSON を黙って本番構成に見せない（制作・配布プレビュー）

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| BuildUp 読込失敗の可視化 | 有効 | 読めないアセットやスクリプトの失敗が、黙って消えず分かる |

---

## セッション 2026-11-20 12:03（/loop 15m・roadmap tick）

### 完了

- `SceneBuildUpAudioPolicy` で本編相当 BGM（bgmGame）・環境ループ SE・入場時 `PlaySceneTransitionSe`、退出時フェード連携停止
- `Update` で `AssignPendingFadeSyncedSceneBgm` / `SyncSceneBgmForChangeSceneFade` を維持

### 推測した用途（§6.4）

- BuildUp 検証プレイで音の文脈を本編に近づけ、フェード遷移時の BGM 挙動を確認する

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| BuildUp 入退場音声 | 有効 | プレイ中に BGM／SE が途切れず再生され、シーン遷移後も期待どおり鳴り続ける |

---

## セッション 2026-11-20 12:18（/loop 15m・roadmap tick）

### 完了

- `SceneTitleBgmMaintain` で BGM の `IsSceneBgmAudible` 監視と再 `PlayBgmLoop`、環境ループ SE も再開
- `Update` で `MaintainSceneTitleBgmIfNeeded` にフェード同期を集約

### 推測した用途（§6.4）

- タイトル滞在中のストリーム異常で BGM が止まっても無音にせず復帰する

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| Title BGM 維持 | 有効 | プレイ中に BGM／SE が途切れず再生され、シーン遷移後も期待どおり鳴り続ける |

---

## セッション 2026-11-20 12:32（/loop 15m・roadmap tick）

### 完了

- `SceneTitleVisualApply` で `game_scene_lighting.json` 経由のリム・フォグ・ゴッドレイ・シャドウをタイトルに適用
- `EnterScene` と `DrawScene` の両方で反映（本編との見た目急変を抑制）

### 推測した用途（§6.4）

- タイトルから本編・ムービーへの遷移でライティングポリシーが連続するプレイ体験

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| Title 見た目パイプライン | 有効 | ランタイムでライト・フォグ等の見た目がデータどおり反映される |

---

## セッション 2026-11-20 12:48（/loop 15m・roadmap tick）

### 完了

- `SceneTitleSubmitTransition` で Submit 直後に `PlaySubmitSe` と `ChangeSceneFade` へイントロムービー遷移
- 遷移実行中・二重 Submit の抑止

### 推測した用途（§6.4）

- タイトル開始操作の手応え（SE＋即フェード）と音切れの少ない本編入り

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| Title Submit フェード | 有効 | プレイ中に BGM／SE が途切れず再生され、シーン遷移後も期待どおり鳴り続ける |

---

## セッション 2026-11-20 13:03（/loop 15m・roadmap tick）

### 完了

- `SceneResultBgmMaintain` で `bgmResult` の途切れ検知・再開とフェード同期を SceneGame/Title と同等化

### 推測した用途（§6.4）

- クリア後リザルト画面でストリーム異常などにより BGM が止まっても復帰する

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| Result BGM 維持 | 有効 | プレイ中に BGM／SE が途切れず再生され、シーン遷移後も期待どおり鳴り続ける |

---

## セッション 2026-11-20 13:18（/loop 15m・roadmap tick）

### 完了

- `SceneResultVisualApply` で `game_scene_lighting.json` のリム・フォグ・ゴッドレイ・シャドウをリザルトに適用
- 簡易平行光のみの `SetupStageLighting` を廃止

### 推測した用途（§6.4）

- クリア後リザルトでも本編と同系統の見た目で演出の連続性を保つ

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| Result 見た目パイプライン | 有効 | ランタイムでライト・フォグ等の見た目がデータどおり反映される |

---

## セッション 2026-11-20 13:33（/loop 15m・roadmap tick）

### 完了

- `SceneResultExitBgmPolicy` でフェード中 Exit は `ReleaseSceneBgmOnExitDuringFade`（即 Stop しない）
- `SceneResultSubmitTransition` でタイトル BGM を deferred 登録してからフェード遷移

### 既存実装の課題点

- なし（Exit は既に共有ポリシー経路だったが、Result 専用ポリシーと Submit 連携で明示化）

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| Result Exit BGM | 有効 | プレイ中に BGM／SE が途切れず再生され、シーン遷移後も期待どおり鳴り続ける |

---

## セッション 2026-11-20 13:48（/loop 15m・roadmap tick）

### 完了

- `Scene::AllowDebugCameraControl` 既定を `false` に変更（未設定シーンで Debug 入力を許可しない）
- `SceneDebugCameraInputPolicy` でロックと Allow の組み合わせを単一正本化し `CmpCamera::PrepareBeforeDraw` を 1 フラグに整理
- `player_friendly_camera_policy.md` のガード表を更新

### 推測した用途（§6.4）

- 本編・タイトル等ではプレイヤーがカメラを直接操作せずステージカメラを維持する（`ultimateGoal` ゲーム中のカメラワーク）
- 制作エディタのみ明示的に Allow を true 化してビューポート操作を許可する

### ビルド

- Cloud VM のため `buildDevelopment.cmd` 未実行（従来どおり）

### loop lease

- Linux VM に `pwsh` が無く `loopLease.ps1` は未実行

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| AllowDebugCameraControl ガード | 有効 | マリオオデッセイのように、プレイヤーがカメラを直接操作せずとも自動でカメラが動作し、見やすいカメラワークをとるようになっている |

---

## セッション 2026-11-20 14:03（/loop 15m・roadmap tick）

### 完了

- `SceneStageCameraLockDefaults.h` で基底のステージカメラロック既定を `true` にコード化
- `Scene::IsStageViewCameraLocked()` が当該定数を返すよう変更
- `verify_camera_policy.py` で基底既定・ Scene サブクラス自動検出と未登録の失敗を追加

### 推測した用途（§6.4）

- 新規プレイ用シーン追加時、チェックリスト漏れでも意図しないフリーカメラを防ぐ
- エディタ系のみ `false` 明示と `UNLOCKED_EDITOR` 登録で制作ビュー操作を維持

### ビルド

- Cloud VM のため `buildDevelopment.cmd` 未実行

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| 新規シーン stage camera lock | 有効 | マリオオデッセイのように、プレイヤーがカメラを直接操作せずとも自動でカメラが動作し、見やすいカメラワークをとるようになっている |

---

## セッション 2026-11-20 14:18（/loop 15m・roadmap tick）

### 完了

- `SceneParticleEditorAllowDebugPolicy` で Development+ImGui のみ `AllowDebugCameraControl` を true に限定
- `SceneParticleEditor` がポリシー経由で override を宣言
- `verify_camera_policy.py` に ParticleEditor 向け Allow 検査を追加

### 推測した用途（§6.4）

- パーティクル JSON 編集の 3D プレビューでデバッグカメラを回せる（制作・デバッグ）
- Release／ImGui 無しでは基底既定どおり入力なし

### ビルド

- Cloud VM のため `buildDevelopment.cmd` 未実行

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| ParticleEditor Allow 限定 | 有効 | エディタ上でのプレイとゲーム配布のランタイム環境上でのプレイを可能にしてある（制作シーンのみ自由視点） |

---

## セッション 2026-11-20 14:33（/loop 15m・roadmap tick）

### 完了

- `MovieCameraTrackKeyframeParsePolicy` でキーフレーム単位の検証を分離
- `LoadMovieCameraTrackFromJson` が不正キーフレームをスキップし、有効 2 件以上で成功
- `movie_camera_controller.md` にスキップ方針を追記

### 推測した用途（§6.4）

- 制作中のトラック JSON に 1 キーフレームだけ壊れていても、ムービーシーンのカットシーン確認を継続できる
- ランタイムでは有効キーフレームのみでシネマチック再生

### ビルド

- Cloud VM のため `buildDevelopment.cmd` 未実行

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| ムービートラック部分キーフレーム | 有効 | ムービーシーンなどでシネマチックな演出を行うことができる |

---

## セッション 2026-11-20 14:48（/loop 15m・roadmap tick）

### 完了

- `verify_movie_camera_track_json.py` で sample／Camera 配下の keyframes JSON をスモーク検証
- 不正キーフレームスキップのインラインテストを同スクリプトに含める
- `BuildCoverageCheck.yml` に CI ステップを追加

### 推測した用途（§6.4）

- マージ前にカットシーン用トラック JSON の破損を検知（配布・プレイ検証）
- 制作時に `sample_movie_track.json` を正本として CI と揃える

### ビルド

- Cloud VM のため `buildDevelopment.cmd` 未実行（Python スモークは OK）

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| ムービートラック JSON スモーク | 有効 | ムービーシーンなどでシネマチックな演出を行うことができる |

---

## セッション 2026-11-20 15:03（/loop 15m・roadmap tick）

### 完了

- `LightingEffectsImGuiPanel` で `Lighting::ImGui` とエンジン配線の UI を共通化
- `LightingEffectsDebugImGui` を `GameOrder` とエンティティエディタ「デバッグ」メニューに接続
- `verify_lighting_imgui_wire.py` を CI に追加

### 推測した用途（§6.4）

- Development エディタでフォグ／ゴッドレイ／影パラメータを即時調整し、シーン見た目を検証する
- ランタイム本番ではウィンドウ非表示のまま（メニューで明示的に開く）

### ビルド

- Cloud VM のため `buildDevelopment.cmd` 未実行

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| Lighting ImGui 配線 | 有効 | ライトをコンポーネント化して配置/編集できる／グラフィックスエンティティとして実装（デバッグ UI でライティング拡張を調整） |

---

## セッション 2026-11-20 15:18（/loop 15m・roadmap tick）

### 完了

- `SceneGame::DrawScene` から `ApplySceneGameLightingFromProfile` を削除（`EnterScene` の `ApplySceneGameVisualPresetOnEnter` のみ）
- `SceneGameEnterVisualPreset.h` に Draw 再適用禁止の方針を明記
- `verify_scene_game_visual_apply.py` を CI に追加

### 推測した用途（§6.4）

- 本編プレイ中、ImGui や演出経路で変えたフォグ／ゴッドレイが毎フレーム C++ 既定で潰れない
- `game_scene_lighting.json` はシーン進入時に一度だけエンジンへ反映

### ビルド

- Cloud VM のため `buildDevelopment.cmd` 未実行

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| SceneGame Draw 見た目再適用 | 有効 | ランタイムでライト・フォグ等の見た目がデータどおり反映される |

---

## セッション 2026-11-20 15:33（/loop 15m・roadmap tick）

### 完了

- `LightingEngineEffectsBinding` でフォグ／ゴッドレイ／影の正本を `AtrumEngine` に統一
- `Visual::Lighting` をエンジン参照ファサードへ再構成（独自 `volumetricFog_` 等を削除）
- `volumetric_fog.md` と `verify_lighting_effects_shared.py` を更新

### 推測した用途（§6.4）

- `Lighting::RefVolumetricFog` と `Engine::RefVolumetricFog` が同一設定を指し、プロファイル／ImGui 変更が 3D CB に一貫して届く

### ビルド

- Cloud VM のため `buildDevelopment.cmd` 未実行

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| Lighting と Engine のフォグ共有 | 有効 | ランタイムでライト・フォグ等の見た目がデータどおり反映される |

---

## セッション 2026-11-20 15:48（/loop 15m・roadmap tick）

### 完了

- `EditorFrameLightingDiagnostics` を追加し `EditorFrame` から描画診断メニューを公開
- エンティティエディタ「デバッグ」メニューを EditorFrame 経由に変更
- SceneBuildUp ランタイムビューに「描画診断」メニューバーを追加
- `verify_editor_frame_lighting_panel.py` を CI に追加

### 推測した用途（§6.4）

- 開発時に実行ビュー／BuildUp でフォグ・ゴッドレイ・影を即時調整し、本編 JSON 反映を目視確認
- EditorFrame を正本入口にし、エンジン診断 UI の置き場を統一

### ビルド

- Cloud VM のため `buildDevelopment.cmd` 未実行

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| EditorFrame ライティング診断 | 有効 | エディタ上でのプレイとランタイム検証（デバッグ UI でライティング結果を確認） |

---

## セッション 2026-11-20 16:03（/loop 15m・roadmap tick）

### 完了

- `EditorSceneRuntimeFunctionalViewDraw` で BuildUp 同等の実行ビュー描画を追加
- `runtimeFunctionalViewDraw` トグルと「機能試験描画」チェックボックス
- シーン実行時にステージカメラ境界を適用し Update で `MaintainLockedStageCamera`

### 推測した用途（§6.4）

- エディタ内で Cmp 単体試験を本番に近いライト／カメラ／シルエットで確認
- SceneBuildUp へ遷移せず Runtime タブで機能検証

### ビルド

- Cloud VM のため `buildDevelopment.cmd` 未実行

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| 実行ビュー機能試験 | 有効 | シーンの構成を保存し、ゲーム中にエディタ上でのプレイとゲーム配布のランタイム環境上でのプレイを可能にしてある |

---

## セッション 2026-11-20 16:15（/loop 15m・roadmap tick）

### 完了

- `SceneBuildUpLaunchPolicy::QueueFadeToBuildUp` を追加（`Launch` + `CommandChangeScene::Set` + `ChangeSceneFade` を 1 API 化）
- エンティティエディタ／パーティクルエディタの BuildUp 起動を同 API へ寄せた
- `verify_scene_build_up_launch_policy.py` を CI（BuildCoverageCheck）に追加
- roadmap「Launch は生成してポインタ返却のみ」を `[x]`

### 推測した用途（§6.4）

- デバッグ用プレイエリア／F3 デモシーン起動で、毎回同じフェード遷移手順を書かずに済む
- `SceneBuildUpRegistry::Launch` の責務（生成＋ポインタ返却）と遷移キュー投入を呼び手側で分離しやすくする

### ビルド

- Cloud VM のため `buildDevelopment.cmd` 未実行

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| Launch は生成してポインタ返却のみ | 有効 | エディタ既定の機能検証シーンからエディタ外ランタイムを試す（debug プレイ導線の整理） |

---

## セッション 2026-11-20 16:30（/loop 15m・roadmap tick）

### 完了

- `SceneBuildUpLaunchPolicy::CreateOwnedBuildUp` で「Launch＝生成のみ」を公開 API 化
- `CommandScene`／`SceneReleaseCarousel` の直 `Launch` をポリシー経由に変更
- `verify_scene_build_up_launch_policy.py` で `Registry::Launch` 直呼びを定義箇所以外禁止
- roadmap「Launch は Scene 生成のみ」を `[x]`

### 推測した用途（§6.4）

- 起動時（CommandScene）とカルーセル遷移で生成と `EnterScene` の責務を分け、フェード遷移経路と矛盾しない
- 機能検証シーンのスキャフォールド手順で「作る／入る／遷移する」の段階をコード上も読み分けやすくする

### ビルド

- Cloud VM のため `buildDevelopment.cmd` 未実行

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| Launch は Scene 生成のみ | 有効 | シーンの構成を保存し、ゲーム中にエディタ上でのプレイとゲーム配布のランタイム環境上でのプレイを可能にしてある |

---

## セッション 2026-11-20 16:45（/loop 15m・roadmap tick）

### 完了

- `SceneBuildUpRegistryLaunchDedupe::AcquireOrCreateOwned` で resolved パス一致時は既存 `SceneBuildUp` を返却
- 同一パスの重複エントリを所有リストから除去、`SetReturnScene` で再 Launch 時の戻り先を更新
- `verify_scene_build_up_launch_policy.py` に dedupe 接続の静的検査を追加
- roadmap「Launch が同一パス重複を防がない」を `[x]`

### 推測した用途（§6.4）

- デバッグプレイ／機能検証シーンを連打しても `ownedScenes_` が同一 JSON で膨らまない
- F1 戻り先が再実行たびにエディタ側の最新 `returnScene` に追従する

### ビルド

- Cloud VM のため `buildDevelopment.cmd` 未実行

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| Launch が同一パス重複を防がない | 有効 | エディタ既定の機能検証シーンからエディタ外ランタイムを試す（debug 導線の安定） |

---

## セッション 2026-11-20 17:00（/loop 15m・roadmap tick）

### 完了

- `AudioStreamingDebugLog` で `CollectStreamingMemoryDebug` 結果を `LogFile` へ行追記
- ImGui「ストリーミング音声デバッグ」に「スナップショットをログへ」ボタンを追加
- `verify_audio_streaming_debug_log.py` を CI に追加
- roadmap「ストリーミング診断が ImGui 表示のみでログに残らない」を `[x]`

### 推測した用途（§6.4）

- BGM ストリーム異常を画面キャプチャだけでなくログファイルで共有・再現
- remaining 肥大やキュー枯渇の調査を開発ビルドのデバッグ作業で行う

### ビルド

- Cloud VM のため `buildDevelopment.cmd` 未実行

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| ストリーミング診断のログ | 有効 | 特に明記なし（デバッグ・プロファイリング用途であり最終目標の `・` 行と矛盾なし） |

---

## セッション 2026-11-20 17:15（/loop 15m・roadmap tick）

### 完了

- `EditorFrameStreamingDiagnostics` でストリーミングメモリトグルを共通化
- `EditorFrame::DrawRenderDiagnosticsMainMenuBar` に「デバッグ」メニューを追加（SceneBuildUp ランタイムで ON 可能）
- エンティティエディタのデバッグメニューも同 API を利用
- roadmap「ストリーミングデバッグ ON が SceneBuildUp 経路で開けない」を `[x]`

### 注記

- ファイル出現順の次候補は **EntityEditorUi「Release 起動パスを表示するだけ」**（本 tick は着手ブランチに合わせ StreamingMenu を実施）

### 推測した用途（§6.4）

- BuildUp プレビュー中に BGM ストリーム診断ウィンドウを開き、エディタに戻らず計測する

### ビルド

- Cloud VM のため `buildDevelopment.cmd` 未実行

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| BuildUp でストリーミング診断 ON | 有効 | エディタ上でのプレイとランタイム検証（開発時デバッグ） |

---

## セッション 2026-11-20 17:30（/loop 15m・roadmap tick）

### 完了

- `TryQueueBuildUpFromReleaseStartup` で `launch_config` の起動 JSON を BuildUp 起動に接続
- プロジェクトパネルに「Release 起動で BuildUp」ボタンを追加（検証は `IsValidBootSceneJsonPath`）
- `verify_entity_editor_release_startup_launch.py` を CI に追加
- roadmap「Release 起動パスを表示するだけ」を `[x]`

### 推測した用途（§6.4）

- Release 向けに設定した起動シーンを Development エディタから即 BuildUp 試行し、配布起動と同じ JSON を検証
- 表示のみだった起動パスを実際のデバッグ導線にする

### ビルド

- Cloud VM のため `buildDevelopment.cmd` 未実行

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| Release 起動から BuildUp | 有効 | シーンの構成を保存し、エディタ上でのプレイとランタイム環境上でのプレイを可能にしてある |

---

## セッション 2026-11-20 17:45（/loop 15m・roadmap tick）

### 完了

- `StageEditorPlayScenePath` / `TakePendingEditorPlayScenePath` で PlayRequest の書き込み・消費を共通化
- 「シーンを実行」時に PlayRequest へ JSON を載せ、次回起動解決と BuildUp 起動で `Path` を消費
- `CommandSceneDevelopmentBoot` と `SceneEntityEditor` を新 API 経由に変更
- roadmap「EditorScenePlayRequest::Path が参照されない」を `[x]`

### 推測した用途（§6.4）

- エディタで選んだシーン JSON を次の Development 起動や BuildUp に引き継ぐ
- 起動シーン設定とワンショット実行の橋渡し

### ビルド

- Cloud VM のため `buildDevelopment.cmd` 未実行

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| EditorScenePlayRequest::Path | 有効 | シーンの構成を保存し、エディタ上でのプレイとランタイム環境上でのプレイを可能にしてある |

---

## セッション 2026-11-20 18:00（/loop 15m・roadmap tick）

### 完了

- `ClearStagedEditorPlayScenePath` で PlayRequest の明示破棄 API を追加
- `TakePendingEditorPlayScenePath` 内の `Clear` は従来どおり消費時に実行
- `SceneEntityEditor::ExitScene` と `SceneBuildUp::ExitScene` で未消費パスをリセット
- `verify_editor_scene_play_request_clear.py` を CI に追加
- roadmap「EditorScenePlayRequest::Clear が呼ばれない」を `[x]`

### 推測した用途（§6.4）

- 検証シーン入退場で古い再生 JSON が次の起動や BuildUp に残らないようにする

### ビルド

- Cloud VM のため `buildDevelopment.cmd` 未実行

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| EditorScenePlayRequest::Clear | 有効 | シーンの構成を保存し、エディタ上でのプレイとランタイム環境上でのプレイを可能にしてある |

---

## セッション 2026-11-20 18:15（/loop 15m・roadmap tick）

### 完了

- `TrySetStartupSceneAndQueueBuildUp` で `TrySetRepositoryStartupScene`（PlayRequest::Set）後に BuildUp を予約
- プロジェクトパネルに「設定して BuildUp」ボタンを追加
- `verify_editor_scene_play_request_buildup.py` を CI に追加
- roadmap「Set 後に BuildUp 起動が無い」を `[x]`

### 推測した用途（§6.4）

- 起動シーン JSON を launch_config に保存した直後に、ワンアクションで SceneBuildUp 検証へ遷移

### ビルド

- Cloud VM のため `buildDevelopment.cmd` 未実行

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| Set 後 BuildUp | 有効 | シーンの構成を保存し、エディタ上でのプレイとランタイム環境上でのプレイを可能にしてある |

---

## セッション 2026-11-20 18:30（/loop 15m・roadmap tick）

### 完了

- `QueueEditorScenePlayViaBuildUp` でプロジェクトパネル「シーンを実行」を BuildUp フェード遷移に接続
- 従来の Runtime タブ試行は「エディタ内プレビュー」ボタンへ分離
- `verify_scene_entity_editor_buildup_play.py` を CI に追加
- roadmap「シーン実行が本番相当 SceneBuildUp 経路にならない」を `[x]`

### 推測した用途（§6.4）

- 配布ランタイムと同じ SceneBuildUp でシーン JSON を検証し、エディタ内シミュレーションとの差を減らす

### ビルド

- Cloud VM のため `buildDevelopment.cmd` 未実行

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| シーン実行 BuildUp 経路 | 有効 | シーンの構成を保存し、エディタ上でのプレイとゲーム配布のランタイム環境上でのプレイを可能にしてある |

---

## セッション 2026-11-20 18:45（/loop 15m・roadmap tick）

### 完了

- `SceneEntityEditorDebugHotkeys` を追加し **F2** で DebugPlayArea テンプレを BuildUp 起動
- パーティクルエディタへは **F4**（F3 はパーティクルデモ BuildUp のまま）
- `verify_scene_entity_editor_f2_feature_scene.py` を CI に追加
- roadmap「F2 はパーティクル遷移のみで機能検証シーン未割当」を `[x]`

### 推測した用途（§6.4）

- ワンキーで最小機能検証シーンへ入り、Cmp 試験の立ち上げを短くする

### ビルド

- Cloud VM のため `buildDevelopment.cmd` 未実行

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| F2 機能検証 BuildUp | 有効 | エディタ既定の機能検証シーンからエディタ外ランタイムを試す |

---

## セッション 2026-11-20 19:00（/loop 15m・roadmap tick）

### 完了

- `ApplyInEditorPlayRuntimeTabFocusOnly` でエディタ内プレビュー後は Runtime タブ＋フォーカスのみ
- 自動の `previewSimulationEnabled`／`runtimeFunctionalViewDraw`／ステージカメラ適用を削除
- `verify_scene_entity_editor_runtime_tab_focus.py` を CI に追加
- roadmap「シーン実行後は Runtime タブフォーカスのみ」を `[x]`

### 推測した用途（§6.4）

- プレビュー表示だけ先に Runtime に切り、実行モードや機能試験描画はツールバーで明示 ON

### ビルド

- Cloud VM のため `buildDevelopment.cmd` 未実行

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| Runtime タブフォーカスのみ | 有効 | エディタ上でのプレイとランタイム検証の段階分け |

---

## セッション 2026-11-20 19:15（/loop 15m・roadmap tick）

### 完了

- `GetSupplementalCameraEntities` でプレビュー Entity をカメラ解決に参加
- `ApplyPrimaryFromPreviewEntities` で JSON の primary `CmpCamera` を `PrepareBeforeDraw`
- プレビューにカメラが無いときのみ従来の rig 既定位置へフォールバック
- `RebuildPreviewScene` 後に `SetupPreviewCamera` を呼ぶよう順序を修正
- roadmap「SetupPreviewCamera が GetCamera のみ更新」を `[x]`

### 推測した用途（§6.4）

- シーン JSON のカメラ配置をエディタプレビューで早期に一致させる

### ビルド

- Cloud VM のため `buildDevelopment.cmd` 未実行

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| SetupPreviewCamera と CmpCamera | 有効 | エンティティのコンポーネント構成を json で編集し、プレイ検証に活かす |

---

## セッション 2026-11-20 19:30（/loop 15m・roadmap tick）

### 完了

- `PrepareSceneViewCameraForDraw` で Runtime タブは `CmpCamera` を維持（機能試験描画時は Update 側の StageView を `PrepareCamerasForDraw` のみ）
- Game タブのみ `ApplyGameViewStageCameraForDraw`（戦闘ステージ相当の従来挙動）
- Runtime 描画経路から `SetupStageViewCamera`＋DebugCamera 退避を除去
- roadmap「Runtime タブで SetupStageViewCamera に差し替え」を `[x]`

### 推測した用途（§6.4）

- Runtime タブで JSON カメラコンポーネントの挙動を本編カメラ上書きなしで確認する

### ビルド

- Cloud VM のため `buildDevelopment.cmd` 未実行

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| Runtime CmpCamera 検証 | 有効 | シーン構成の編集とエディタ上でのプレイ／ランタイム検証を分けて通せる |

---

## セッション 2026-11-20 19:45（/loop 15m・roadmap tick）

### 完了

- `QueueInEditorPlayRuntimeTabOnly` で UI からのエディタ内実行要求を一本化
- `SceneEntityEditorInEditorPlayTick::Process` で保存・プレビュー再構築・Runtime タブのみ（BuildUp 非連動）
- プロジェクト／メニューの「エディタ内プレビュー」が上記キューを利用
- roadmap「実行は Runtime タブ切替のみ」を `[x]`

### 推測した用途（§6.4）

- エディタ内の軽い実行確認と本番 BuildUp 経路の誤混在を防ぐ

### ビルド

- Cloud VM のため `buildDevelopment.cmd` 未実行

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| エディタ内プレイと BuildUp 分離 | 有効 | シーン構成の編集とエディタ上プレイ／配布ランタイムを別経路で通せる |

---

## セッション 2026-11-20 20:00（/loop 15m・roadmap tick）

### 完了

- `ApplyHoveredEditViewCameraInput` で Edit ビューホバー時のみ `CmpCamera::PrepareBeforeDraw(true)`
- `AllowDebugCameraControl` は false のまま GameOrder と二重入力を回避
- `Draw` から `camera->Update()` 直呼びを除去
- roadmap「Edit ホバー時に Draw 内で camera->Update」を `[x]`

### 推測した用途（§6.4）

- エディタ Edit ビューだけ自由視点を操作しつつ、Runtime／GameOrder 側のカメラ契約を崩さない

### ビルド

- Cloud VM のため `buildDevelopment.cmd` 未実行

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| Edit ビューカメラと Cmp 経路 | 有効 | エンティティ編集とプレイ検証のワークフローを一貫させる |

---

## セッション 2026-11-20 20:15（/loop 15m・roadmap tick）

### 完了

- `ActiveCameraSystem::SyncEngineAfterSceneViewFit` で RT 再バインド窓口を追加
- `BindForSceneViewRender` が GameOrder の active を優先し Fit 後に同期
- `DrawSceneViewContent` から `engine->SetCamera` 直呼びを除去
- roadmap「シーンビュー RT が GameOrder 後に SetCamera」を `[x]`

### 推測した用途（§6.4）

- エンジン主導のアクティブカメラ契約を保ったまま ImGui シーンビュー RT を描画する

### ビルド

- Cloud VM のため `buildDevelopment.cmd` 未実行

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| アクティブカメラと RT 描画 | 有効 | エディタ上プレイ検証とゲーム配布ランタイムのカメラ解決を揃える |

---

## セッション 2026-11-20 20:30（/loop 15m・roadmap tick）

### 完了

- `InitializeSceneDocumentEntities` で親子の未初期化子を補完
- `DrawSceneDocumentEntities` でルート Draw＋親非 Active 時の子フォールバック
- `DrawPreviewEntities` を上記に委譲
- roadmap「DrawPreview が親 root のみ」を `[x]`

### 推測した用途（§6.4）

- 階層付きシーン JSON をエディタプレビューで欠けなく確認する

### ビルド

- Cloud VM のため `buildDevelopment.cmd` 未実行

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| 階層プレビュー描画 | 有効 | エンティティのコンポーネント構成を json で編集しプレイ検証に活かす |

---

## セッション 2026-11-20 20:45（/loop 15m・roadmap tick）

### 完了

- `ReleasePreviewEntities` でプレビュー Entity の Finalize／clear
- `ResetEditorTransientPlayUi` で Runtime／BuildUp 系リクエストと Edit タブへ復帰
- `ExitScene` が上記を呼ぶよう配線
- roadmap SceneEntityEditor 節の未完了行をすべて `[x]`（本 tick で Exit 行）

### 推測した用途（§6.4）

- BuildUp 往復後もエディタ UI が実行モードのまま残らないようにする

### ビルド

- Cloud VM のため `buildDevelopment.cmd` 未実行

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| Exit 時 Ui 整合 | 有効 | シーン編集とエディタ上プレイ／ランタイム検証を安全に往復させる |

---

## セッション 2026-11-20 21:00（/loop 15m・roadmap tick）

### 完了

- `FrameDeltaTimeDebugImGui` で FPS/ms オーバーレイと 120 フレームヒストグラム
- `GameOrder` から毎フレーム描画、`EditorFrame`「デバッグ」メニューで ON/OFF
- roadmap「フレーム時間のオーバーレイ／ヒストグラムが無い」を `[x]`

### 推測した用途（§6.4）

- Development でヒッチ検知と平均フレーム時間の当たりを付ける

### ビルド

- Cloud VM のため `buildDevelopment.cmd` 未実行

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| フレーム時間可視化 | 有効 | フレーム時間を物理・アニメ・遷移と同じ基準で使う流れのデバッグ支援 |

---

## セッション 2026-11-20 21:15（/loop 15m・roadmap tick）

### 完了

- `EditorDebugPlayAreaScaffold` でテンプレから `DebugPlayArea/*.scene.json` を生成
- プロジェクトパネルに「テストエリア作成」UI、成功時は自動でシーンを開く
- `ListEditableSceneJsonFiles` に DebugPlayArea を含める
- roadmap「テストエリア新規作成が scaffold スクリプト手動実行のみ」を `[x]`

### 推測した用途（§6.4）

- デバッグ・プロファイリング用に本編外で Entity／Cmp を素早く試す

### ビルド

- Cloud VM のため `buildDevelopment.cmd` 未実行

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| エディタ内テストエリア作成 | 有効 | ファイルの新規作成と json 編集の自作エディタ導線を前に進める |

---

## セッション 2026-11-20 21:30（/loop 15m・roadmap tick）

### 完了

- `DebugPlayAreaScaffoldGuidance` で生成後手順文言を共通化
- Python `scaffold_play_area.py` の案内をプレビュー／BuildUp／起動シーン／F2 まで拡張
- エディタに「作成して BuildUp」「作成してエディタ内プレビュー」ショートカット
- roadmap「生成後は Launch のみ案内」を `[x]`

### 推測した用途（§6.4）

- scaffold から試験起動までの導線を一本化し手順分散を減らす

### ビルド

- Cloud VM のため `buildDevelopment.cmd` 未実行

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| テストエリア手順案内 | 有効 | デバッグ用シーンでランタイム挙動を素早く再現する流れを支える |

---

## セッション 2026-11-20 21:45（/loop 15m・roadmap tick）

### 完了

- `personalize_play_area_scene.py` / `PersonalizeDebugPlayAreaSceneJson` で tagCatalog と Prop/Floor の instance 名を差し替え
- Python・エディタ scaffold が共有ロジックを利用
- roadmap「生成は sceneName 差替のみ」を `[x]`（scaffold_play_area 節完了）

### 推測した用途（§6.4）

- 用途別テストシーンを並べても階層名が衝突しにくくする

### ビルド

- Cloud VM のため `buildDevelopment.cmd` 未実行

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| テストシーン個別化 | 有効 | json でエンティティ構成を編集し検証に活かす |

---

## セッション 2026-11-20 22:00（/loop 15m・roadmap tick）

### 完了

- `CreateInitialSceneForDevelopmentBoot` で起動シーン種別を一本化
- 起動パス未設定／不正時は DebugPlayArea テンプレ BuildUp へフォールバック
- `CommandScene` から EntityEditor 直指定を削除
- roadmap「起動時シーンが SceneEntityEditor に固定」を `[x]`

### 推測した用途（§6.4）

- Development 起動直後から機能検証 BuildUp に入れる

### ビルド

- Cloud VM のため `buildDevelopment.cmd` 未実行

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| 起動先の解決 | 有効 | エディタ上プレイと配布ランタイム検証の段階分けを支える |

---

## セッション 2026-11-20 22:45（/loop 15m・roadmap tick）

### 完了

- `CommandSceneReleaseBootApply` で Release／非 Development 起動を集約
- `launch_config` 未設定・無効時は `SceneTitle` へ（DebugPlayArea テンプレ BuildUp フォールバックを削除）
- `verify_command_scene_release_boot.py` を CI に追加
- roadmap「startup 空なら BuildUp ではなく Title」を `[x]`
- 前 tick の verify 偽陰性修正と `vcxproj.filters` 反映（`78a53cc`）

### 推測した用途（§6.4）

- 配布ビルド: 起動シーン未指定時は本編タイトルからプレイ開始（デバッグ用 BuildUp へ誤って落ちない）
- Development: 従来どおりテンプレ BuildUp フォールバックは `ResolveDevelopmentBootTarget` 側のみ

### ビルド

- Cloud VM のため `buildDevelopment.cmd` 未実行

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| startup 空 → Title | 有効 | シーン構成の保存とランタイム環境でのプレイ導線を、配布時は Title 起点に揃える |

### 次 tick 候補

- SceneBuildUp「BuildUp が tagCatalog マージを飛ばす」（`fix/SceneBuildUpTagCatalogMerge` 予定）

---

## セッション 2026-11-21 01:30（/loop 15m・roadmap tick）

### 完了

- `Scene::TryHandleInputWhileSceneTransitionBlocked` を追加
- `GameOrder` が遷移フェード中（Update 停止中）に上記を呼ぶ
- `SceneBuildUpF1ReturnRequest` で F1 復帰を共通化し、BuildUp が override
- `verify_scene_build_up_f1_during_fade.py` を CI に追加
- roadmap 当該行を `[x]`

### 推測した用途（§6.4）

- 機能検証 BuildUp: 出入りの暗転中も F1 でエディタ／タイトルへ戻れる

### ビルド

- Cloud VM のため `buildDevelopment.cmd` 未実行

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| フェード中 F1 | 有効 | エディタ上プレイとランタイム検証の往復を途切れさせない |

### 次 tick 候補

- LoopTick13Demo「機能検証シーンの正本として未整備」（`feature/LoopTick13DemoCanonical` 予定）

---

## セッション 2026-11-21 04:15（/loop 15m・roadmap tick）

### 完了

- `SceneBuildUpChangeSceneDrawPolicy` で `GetIsExecute()` 時のみ遷移演出を描画
- 機能検証ビューにフェード UI が常時重ならないよう分離
- `verify_scene_build_up_change_scene_draw_when_active.py` を CI に追加
- roadmap 当該行を `[x]`（SceneBuildUp 節は一通り完了）

### 推測した用途（§6.4）

- 機能検証 BuildUp: 通常フレームはランタイム描画のみ、F1／フェード時だけ暗転オーバーレイ

### ビルド

- Cloud VM のため `buildDevelopment.cmd` 未実行

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| 遷移描画の分離 | 有効 | エディタ外ランタイムでの最小構成試験画面を汚さない |

---

## セッション 2026-11-21 04:00（/loop 15m・roadmap tick）

### 完了

- `SceneBuildUpSceneDisplayNameResolve` で ctor 時に JSON の `sceneName` を軽量読み取り
- 失敗時のみパス stem へフォールバック（EnterScene 後も従来どおり document で上書き）
- `verify_scene_build_up_display_name_resolve.py` を CI に追加
- roadmap 当該行を `[x]`

### 推測した用途（§6.4）

- 機能検証 BuildUp: フェード〜EnterScene 前の専用ウィンドウヘッダに実シーン名を出す

### ビルド

- Cloud VM のため `buildDevelopment.cmd` 未実行

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| displayName 即反映 | 有効 | シーン構成 JSON の名称をランタイム試験 UI に正しく示す |

---

## セッション 2026-11-21 03:45（/loop 15m・roadmap tick）

### 完了

- `SceneBuildUpMissingPrimaryCameraFeedback` で暗転＋Development 時 ImGui 通知
- `DrawScene` / `DrawRuntimeViewContent` の camera==nullptr で feedback を呼ぶ
- `verify_scene_build_up_camera_null_feedback.py` を CI に追加
- roadmap 当該行を `[x]`

### 推測した用途（§6.4）

- 機能検証 BuildUp: CmpCamera 未配置テンプレと読込失敗を UI で切り分ける

### ビルド

- Cloud VM のため `buildDevelopment.cmd` 未実行

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| カメラ欠落表示 | 有効 | 空エンティティ＋カメラ試験の制作フローを短縮する |

---

## セッション 2026-11-21 03:30（/loop 15m・roadmap tick）

### 完了

- `SceneBuildUpEnterSceneEditorUiPolicy` で入場時に Runtime タブを強制しない契約を明示
- 残っていた `requestFocusRuntimeSceneView` のみ解除（Edit/Game 比較を妨げない）
- `verify_scene_build_up_enter_scene_no_runtime_tab_lock.py` を CI に追加
- roadmap 当該行を `[x]`

### 推測した用途（§6.4）

- 機能検証 BuildUp: 専用ウィンドウは Draw で表示しつつ、エディタの Game/Edit タブ操作は奪わない

### ビルド

- Cloud VM のため `buildDevelopment.cmd` 未実行

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| Runtime タブ非固定 | 有効 | エディタ上プレイと BuildUp ランタイムを並行して比較できる |

---

## セッション 2026-11-21 03:15（/loop 15m・roadmap tick）

### 完了

- 描画パスのライト再適用は前 tick の `SceneBuildUpRuntimeLightingPolicy` で解消済み（§6.2 追認）
- 方針ヘッダに「HalfLambert を Draw で再固定しない」旨を追記
- エディタ機能試験 RT から毎描画の `ApplySceneBuildUpRuntimeVisualFromProfile` を除去
- `verify_scene_build_up_no_half_lambert_per_draw.py` を CI に追加
- roadmap 当該行を `[x]`

### 推測した用途（§6.4）

- 機能検証 BuildUp: JSON／profile 由来の lightModel を保持し Cmp ライト試験を上書きしない

### ビルド

- Cloud VM のため `buildDevelopment.cmd` 未実行

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| 半ランバート毎描画禁止 | 有効 | グラフィックスコンポーネント検証で意図したライト設定を維持 |

### 実装済みと判断した項目（§6.2）

- 描画毎 `SetupRuntimeLighting` 除去はセッション 02:30 で完了済み。本 tick は CI とエディタ RT の追従

---

## セッション 2026-11-21 03:00（/loop 15m・roadmap tick）

### 完了

- `SceneBuildUpRuntimeEntityDraw` で `DrawSceneDocumentEntities` に委譲（親描画＋非 Active 親の子フォールバック）
- `verify_scene_build_up_child_instance_draw.py` を CI に追加
- roadmap 当該行を `[x]`

### 推測した用途（§6.4）

- 機能検証 BuildUp: 階層付き JSON で子が親経由で描画される契約をランタイムでも再現する

### ビルド

- Cloud VM のため `buildDevelopment.cmd` 未実行

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| 子 Draw 契約 | 有効 | シーン構成の保存とランタイムプレイでヒエラルキーを正しく見る |

---

## セッション 2026-11-21 02:45（/loop 15m・roadmap tick）

### 完了

- `BuildSceneBuildUpRuntimeViewStatusLine` でシーン名＋JSON パスをヘッダ文言化
- `SceneBuildUpDedicatedRuntimeViewWindow` で専用ウィンドウと RT プレビューを配線
- `verify_scene_build_up_runtime_view_window_text.py` を CI に追加
- roadmap 当該行を `[x]`

### 推測した用途（§6.4）

- 機能検証 BuildUp: DebugPlayArea や F2 起動後、どの JSON を試しているか UI 上で即確認する

### ビルド

- Cloud VM のため `buildDevelopment.cmd` 未実行

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| BuildUp ヘッダに JSON | 有効 | エディタ外ランタイムでの機能試験で誤ったシーンを掴まない |

---

## セッション 2026-11-21 02:30（/loop 15m・roadmap tick）

### 完了

- `DrawScene` / `DrawRuntimeViewContent` から毎フレームのライト再適用を除去
- `SceneBuildUpRuntimeLightingPolicy::ApplyOnceOnEnterScene` を EnterScene のみで呼ぶ
- `verify_scene_build_up_runtime_lighting_enter_only.py` を CI に追加
- roadmap 当該行を `[x]`

### 推測した用途（§6.4）

- 機能検証 BuildUp: Runtime ビュー表示中に ImGui デバッグ窓と競合せず、Enter 時のライト状態を維持する

### ビルド

- Cloud VM のため `buildDevelopment.cmd` 未実行

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| ライト Enter のみ | 有効 | ランタイム環境でのプレイ描画コストと見た目の一貫性を保つ |

---

## セッション 2026-11-21 02:15（/loop 15m・roadmap tick）

### 完了

- `SceneBuildUpExitSceneEditorUiRestore` で ExitScene 時に Edit タブへ復帰
- `RestoreSceneViewTabAfterBuildUpExit` で `requestFocusRuntimeSceneView` を解除
- `verify_scene_build_up_exit_scene_view_tab.py` を CI に追加
- roadmap 当該行を `[x]`

### 推測した用途（§6.4）

- 機能検証 BuildUp: F1／フェード復帰後にエディタのエディットビューへ戻り編集を続ける

### ビルド

- Cloud VM のため `buildDevelopment.cmd` 未実行

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| ExitScene で Edit 復帰 | 有効 | エディタ上プレイとランタイム検証の往復後、編集フローを継続できる |

---

## セッション 2026-11-21 02:00（/loop 15m・roadmap tick）

### 完了

- `SceneBuildUpAllowDebugCameraContract` で BuildUp が `AllowDebugCameraControl` を override しない契約を明文化
- `verify_scene_build_up_allow_debug_camera_contract.py` と `verify_camera_policy.py` の BuildUp 検査を追加
- roadmap 当該行を `[x]`

### 推測した用途（§6.4）

- 機能検証 BuildUp: CmpCamera／GameOrder 経路だけで本番と同じ「プレイヤ操作なし」カメラ契約を保つ

### ビルド

- Cloud VM のため `buildDevelopment.cmd` 未実行

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| AllowDebug 非 override | 有効 | マリオオデッセイ型の自動カメラワークを本編／検証ランタイムで揃える |

---

## セッション 2026-11-21 01:45（/loop 15m・roadmap tick）

### 完了

- `SceneBuildUpSceneDocumentTagCatalog` で registry マージ＋定義タグ同期を `EntityEditor::LoadFromJsonPath` と揃えた
- `SceneBuildUpSceneDocumentLoad::TryLoad` の成功直後に適用
- `verify_scene_build_up_tag_catalog_merge.py` を CI に追加
- roadmap 当該行を `[x]`

### 推測した用途（§6.4）

- 機能検証 BuildUp: `camera.main` 等ロールタグをエディタプレビューと同じ契約で解決する

### ビルド

- Cloud VM のため `buildDevelopment.cmd` 未実行

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| tagCatalog マージ | 有効 | エディタ上プレイとランタイム環境上のプレイでシーン JSON のタグ契約を同一にする |

---

## セッション 2026-11-21 01:15（/loop 15m・roadmap tick）

### 完了

- `stageViewCameraLocked_` で `IsStageViewCameraLocked` をランタイム化（既定 true）
- Development: **F5** で StageView ロック ON/OFF（`SceneBuildUpStageCameraLock`）
- ロック OFF 時は `SetupStageViewCamera` をスキップ（CmpCamera 試験向け）
- `verify_camera_policy.py` を BuildUp 切替対応に更新
- `verify_scene_build_up_stage_camera_lock_toggle.py` を CI に追加
- roadmap 当該行を `[x]`

### 推測した用途（§6.4）

- 機能検証 BuildUp: 本番同等のロック ON を既定にしつつ、Cmp 検証用に OFF へ切替

### ビルド

- Cloud VM のため `buildDevelopment.cmd` 未実行

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| StageView ロック切替 | 有効 | エディタ上プレイとランタイム検証でカメラ／Cmp を試せる |

---

## セッション 2026-11-21 01:00（/loop 15m・roadmap tick）

### 完了

- `BindAfterGameOrderPrepare`（`SceneBuildUpRuntimeSubviewDrawCamera`）を追加
- `DrawRuntimeViewContent` から `MaintainLockedStageCamera` を除去（Prepare 後の二重 `SetupStageViewCamera` を防止）
- `verify_scene_build_up_draw_runtime_camera.py` を CI に追加
- roadmap 当該行を `[x]`

### 推測した用途（§6.4）

- 機能検証 RT サブビュー: GameOrder のカメラ契約に合わせ、CmpCamera 行列をステージ固定ポーズで上書きしない

### ビルド

- Cloud VM のため `buildDevelopment.cmd` 未実行

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| RT 描画カメラ | 有効 | ランタイムでカメラ／見た目が意図どおり維持される |

---

## セッション 2026-11-21 00:45（/loop 15m・roadmap tick）

### 完了

- `ApplyStageCameraAfterSceneLoad` を追加（BuildEntities 直後に `SetupStageViewCamera` のみ）
- `EnterScene` は `MaintainLockedStageCamera` ではなく上記を使用（Enter 時は `PrepareCamerasForDraw` しない）
- Update／Draw は従来どおり `MaintainLockedStageCamera`
- `verify_scene_build_up_enter_setup_stage_camera.py` を CI に追加
- roadmap「ロード直後 SetupStageViewCamera を呼ぶ」を `[x]`

### 推測した用途（§6.4）

- 機能検証 BuildUp: JSON ロード直後にステージカメラ姿勢を確定し、Draw 直前の Prepare と責務分離

### ビルド

- Cloud VM のため `buildDevelopment.cmd` 未実行

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| Enter 直後カメラ | 有効 | ランタイムでカメラ／見た目がデータどおり初期化される |

---

## セッション 2026-11-21 00:30（/loop 15m・roadmap tick）

### 完了

- `SceneCameraRig::TickSpawnedEntities` と `Scene::TickCameraRigEntities` を追加
- `SceneBuildUp::Update` で JSON `entities_` の後に rig 自動スポーン実体を Update
- `verify_scene_build_up_camera_rig_update.py` を CI に追加
- roadmap「cameraRig 実体は Update されない」を `[x]`

### 推測した用途（§6.4）

- 機能検証 BuildUp: 既定 MainCamera プレハブの Upd Cmp が毎フレーム動き、Cmp 試験が本番ループに近づく

### ビルド

- Cloud VM のため `buildDevelopment.cmd` 未実行

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| cameraRig Update | 有効 | カメラをエンティティ＋Cmp で組み立てる方針のランタイム更新 |

---

## セッション 2026-11-21 00:15（/loop 15m・roadmap tick）

### 完了

- `SceneBuildUpLoadFailureEntities::BuildEntitiesAfterDocumentLoad` で読込失敗時は **Entity を 0 件**（ダミー `Entity_0` なし）
- `EnterScene` で `loadSucceeded` を明示し、失敗 UI（既存の暗転・ImGui 通知）と組み合わせ
- `verify_scene_build_up_load_failure_no_dummy_entity.py` を CI に追加
- roadmap「ロード失敗時に空の Entity_0 を生成」を `[x]`（ギャップは「ダミー生成をやめる」）

### 推測した用途（§6.4）

- 機能検証 BuildUp: 壊れた JSON を即わかる状態にし、見かけだけプレイ可能な 1 体フォールバックを出さない

### ビルド

- Cloud VM のため `buildDevelopment.cmd` 未実行

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| 読込失敗の可視化 | 有効 | json エンティティ編集の検証で不整合をすぐ気づける |

---

## セッション 2026-11-21 00:00（/loop 15m・roadmap tick）

### 完了

- `SceneBuildUpSceneDocumentLoad` で JSON 読込・Entity 化・フレーミング解決を集約（内部のみ `EditorSceneRuntime` を include）
- `SceneBuildUp.cpp` / `SceneBuildUpStageCameraFraming.cpp` から `EditorSceneRuntime.h` を除去
- `verify_scene_build_up_no_editor_scene_runtime.py` を CI に追加
- roadmap「SceneBuildUp が EditorSceneRuntime を参照しない」を `[x]`

### 推測した用途（§6.4）

- 機能検証 BuildUp: ランタイムシーンがエディタ実装ヘッダに直接依存せず、ロード境界を 1 ファイルに固定
- 将来のランタイム専用ローダ差し替え時に BuildUp 本体の include を触らなくてよい

### ビルド

- Cloud VM のため `buildDevelopment.cmd` 未実行

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| BuildUp ロード境界 | 有効 | シーン JSON を保存しランタイムで再生する流れの責務分離 |

---

## セッション 2026-11-20 23:45（/loop 15m・roadmap tick）

### 完了

- `SceneBuildUp::Draw` から `EditorFrame` ワークベンチ（`ResetPerFrame` / `LayoutDedicatedSceneViewWindow` / 診断メニュー / RT ハブ）を除去
- Development でも `DrawScene` 直描画の最小ランタイム経路に統一（読込失敗 ImGui のみ残す）
- `verify_scene_build_up_draw_skips_editor_frame.py` 追加。照明／ストリーミング verify から BuildUp 依存を削除
- roadmap「SceneBuildUp::Draw が EditorFrame ワークベンチを常時描画」を `[x]`

### 推測した用途（§6.4）

- 機能検証 BuildUp: エディタドック無しで Cmp／Entity を本番同等の Draw ループで試す
- 診断メニューは `SceneEntityEditor` ワークベンチ側に集約

### ビルド

- Cloud VM のため `buildDevelopment.cmd` 未実行

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| BuildUp 最小ランタイム Draw | 有効 | エディタ上プレイとランタイム検証の段階分け（検証シーンは UI 薄い経路） |

---

## セッション 2026-11-20 23:30（/loop 15m・roadmap tick）

### 完了

- `ResolveSceneBuildUpReturnTarget` を追加（Development: EntityEditor、Release: Title）
- `SceneBuildUp::Update` の F1 復帰で null 時も既定戻り先へフェード遷移
- `verify_scene_build_up_return_scene.py` を CI に追加
- roadmap「returnScene が null だと機能検証シーンから抜けられない」を `[x]`

### 推測した用途（§6.4）

- デバッグ: returnScene 未指定の BuildUp 単体起動後も F1 でエディタ／タイトルへ戻る
- 配布: 起動 JSON 試行後に Title へ復帰可能

### ビルド

- Cloud VM のため `buildDevelopment.cmd` 未実行

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| BuildUp F1 復帰 | 有効 | エディタ上プレイとランタイム検証の往復を最小操作で維持 |

---

## セッション 2026-11-20 23:15（/loop 15m・roadmap tick）

### 完了

- `CommandSceneBootEnterApply` で sceneNow 確定後の Enter を一本化
- Release 起動 BuildUp は Enter 前に `IsValidBootSceneJsonPath` で検証し、失敗時は Title へ差し替え
- Development は Enter 前検証なし（壊れた JSON は BuildUp::EnterScene 側で扱う）
- `verify_command_scene_enter_after_scene_now.py` を CI に追加
- roadmap「sceneNow 確定後 Enter を無条件」を `[x]`

### 推測した用途（§6.4）

- 配布ビルド: 存在するが壊れた起動 JSON で BuildUp Enter せず Title に落とす
- Development: Launch→Enter の二段 API を CommandScene で明示し、起動直後の導線を固定

### ビルド

- Cloud VM のため `buildDevelopment.cmd` 未実行

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| Enter 前の起動検証 | 有効 | シーン構成を保存しランタイム環境でプレイ可能にする（壊れた起動 JSON を Title で受ける） |

---

## セッション 2026-11-20 23:00（/loop 15m・roadmap tick）

### 完了

- `CanLaunchBootSceneAtPath`（`CommandSceneBootLaunchPathGate`）を追加。Launch 前は存在チェックのみ
- `ResolveDevelopmentBootTarget` / `CreateInitialSceneForReleaseBoot` から `IsValidBootSceneJsonPath`（TryLoad）を除去
- `IsValidBootSceneJsonPath` はエディタ UI 向け検証として維持
- `verify_command_scene_launch_no_tryload.py` を CI に追加
- roadmap「Launch 前に TryLoad を呼ばない」を `[x]`

### 推測した用途（§6.4）

- 機能検証 BuildUp: 起動は軽量にし、JSON 検証・タグ同期は `SceneBuildUp::EnterScene` で一括
- 壊れた JSON は EnterScene 側の失敗 UI で扱い、起動前の二重読込を避ける

### ビルド

- Cloud VM のため `buildDevelopment.cmd` 未実行

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| Launch 前 TryLoad 禁止 | 有効 | エディタ上プレイとランタイム検証の段階分け（読込責務を EnterScene に集約） |

---

## セッション 2026-11-21 03:30（/loop 15m・roadmap tick）

### 完了

- `EditorAssetsPaths::kDebugPlayAreaLoopTick13DemoScene` を追加
- `EditorDebugPlayAreaFeatureVerificationCanonical` で正本パスを集約
- F2／デバッグメニューの BuildUp 起動先を LoopTick13Demo 正本へ切替（テンプレは scaffold 専用のまま）
- `verify_loop_tick13_demo_feature_verification_canonical.py` を CI に追加
- `verify_scene_entity_editor_f2_feature_scene.py` を正本起動に合わせて更新
- roadmap 当該行を `[x]`

### 推測した用途（§6.4）

- 機能検証 BuildUp: リポジトリ同梱の安定 JSON をワンキー／メニューから再現性高く起動
- ループ／CI: LoopTick13Demo をコード参照の正本として、オフライン検証の種にする

### ビルド

- Cloud VM のため `buildDevelopment.cmd` 未実行

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（`・` 行） |
|--------|------|------|
| LoopTick13Demo 正本 | 有効 | シーン構成の保存と、エディタ外ランタイムでのプレイ試験（Entity JSON の編集・プレイ導線） |

### 次 tick 候補

- combat_bounds_sandbox「コード／既定導線から参照されない」（`fix/CombatBoundsSandboxRef` 予定）

---

## セッション 2026-11-21 03:45（/loop 15m・roadmap tick）

### 完了

- `EditorAssetsPaths::kDebugPlayAreaCombatBoundsSandboxScene` を追加
- `EditorDebugPlayAreaCombatBoundsSandbox` でパスを集約
- デバッグメニューと **F6** から `combat_bounds_sandbox` を BuildUp 起動
- `verify_combat_bounds_sandbox_debug_play_area.py` を CI に追加
- `debug_test_play_area.md` のホットキー表を更新
- roadmap 当該行を `[x]`

### 推測した用途（§6.4）

- ステージ端マーカーと `stageViewFramingBounds` の見え方を、本編 BuildUp 経路で即確認
- レベル／カメラ調整: 左右端・地上高の JSON 契約をエディタ外ランタイムで試す

### ビルド

- Cloud VM のため `buildDevelopment.cmd` 未実行

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（`・` 行） |
|--------|------|------|
| combat_bounds_sandbox 参照 | 有効 | シーン構成の保存と、エディタ上プレイ／ランタイム環境でのプレイ試験 |

### 次 tick 候補

- DebugPlayAreaGenerator「ランタイムプレビュー手順がコード断片のみ」（`fix/DebugPlayAreaGeneratorRuntimePreview` 予定）

---

## セッション 2026-11-21 04:00（/loop 15m・roadmap tick）

### 完了

- `DebugPlayAreaRuntimePreviewGuidance`（`kStepsJa` / `kPanelHintJa`）を追加
- プロジェクトパネルにランタイムプレビュー手順を常時表示
- `DebugPlayAreaGenerator/README.md` から `Launch` 断片を除去しエディタ導線に差し替え
- `scaffold_play_area.py` の完了メッセージを正本文言に統一
- `verify_debug_play_area_generator_runtime_preview.py` を CI に追加
- roadmap 当該行を `[x]`

### 推測した用途（§6.4）

- コンポーネント試験: scaffold 後に README を見ずエディタ上の案内だけで BuildUp へ入る
- 半自律／CI: 手順の正本をヘッダ＋検証で固定し、ドキュメントと UI のズレを防ぐ

### ビルド

- Cloud VM のため `buildDevelopment.cmd` 未実行

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（`・` 行） |
|--------|------|------|
| ランタイムプレビュー手順 | 有効 | エディタ上でのプレイとランタイム環境でのプレイを可能にする（BuildUp 導線のコード正本化） |

### 次 tick 候補

- EditorSceneRuntime「親なしインスタンスだけ初期化」（`fix/EditorSceneRuntimeParentOnly` 予定）

---

## セッション 2026-11-21 04:15（/loop 15m・roadmap tick）

### 完了

- `EditorSceneRuntimeEntityInitializePolicy` を追加（ルート初期化＋未 Active の多段パス）
- `InitializeSceneDocumentEntities` を EditorSceneRuntime から分離
- `verify_editor_scene_runtime_initialize_hierarchy.py` を CI に追加
- `verify_scene_entity_editor_draw_preview_roots.py` を policy 分割に合わせて更新
- roadmap 当該行を `[x]`

### 推測した用途（§6.4）

- 階層付き DebugPlayArea／BuildUp: 子付きインスタンスの Cmp が未初期化のまま動かない
- エディタプレビュー: 親子 JSON をランタイムと同じ Initialize 契約で再現

### ビルド

- Cloud VM のため `buildDevelopment.cmd` 未実行

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（`・` 行） |
|--------|------|------|
| 子 Initialize | 有効 | 保存したエンティティをゲーム内に生成し、ランタイムで試す（階層シーンの安定動作） |

### 次 tick 候補

- EditorSceneRuntime「Entity 名が definition のみ」（`fix/EditorSceneRuntimeInstanceName` 予定）

---

## セッション 2026-11-21 04:30（/loop 15m・roadmap tick）

### 完了

- `EditorSceneRuntimeInstanceNamePolicy` を追加し `instanceName` を `Entity::SetHash` へ反映
- `BuildEntitiesFromSceneDocument` で生成直後に適用
- `verify_editor_scene_runtime_instance_name.py` を CI に追加
- roadmap 当該行を `[x]`

### 推測した用途（§6.4）

- 機能検証 BuildUp: ヒエラルキー表示名（instanceName）とランタイム識別を一致させデバッグしやすくする
- 同一 definition を複数配置したシーンでインスタンス単位の切り分け

### ビルド

- Cloud VM のため `buildDevelopment.cmd` 未実行

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（`・` 行） |
|--------|------|------|
| instanceName 反映 | 有効 | コンポーネント構成を json で編集し、保存したエンティティをランタイムで試す |

### 次 tick 候補

- launch_config「startup が Feature 検証シーンではない」（`fix/LaunchConfigFeatureStartup` 予定）

---

## セッション 2026-11-21 04:45（/loop 15m・roadmap tick）

### 完了

- `EditorAssets/launch_config.json` を追加（startup = LoopTick13Demo 正本）
- `ResolveStartupScenePath` が `Assets/launch_config` 未設定時に EditorAssets 側を読む
- Development 起動フォールバックをテンプレから機能検証正本へ変更
- `EditorLaunchConfigFeatureVerificationDefaults` を追加
- `verify_launch_config_feature_startup.py` を CI に追加
- roadmap 当該行を `[x]`

### 推測した用途（§6.4）

- Development 起動: リポジトリ clone 直後から BuildUp 機能検証シーンへ入れる
- Release 起動: `./Assets/launch_config.json` を別途本編シーン向けに維持可能

### ビルド

- Cloud VM のため `buildDevelopment.cmd` 未実行

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（`・` 行） |
|--------|------|------|
| Feature 検証 startup | 有効 | エディタ上プレイとランタイム環境でのプレイ試験（既定起動の最小構成） |

### 次 tick 候補

- launch_config「設定読込失敗を区別できない」（`fix/LaunchConfigLoadDiagnostics` 予定）

---

## セッション 2026-11-21 05:00（/loop 15m・roadmap tick）

### 完了

- `EditorLaunchConfigLoadDiagnostics` で読込失敗を原因別に分類
- `ResolveStartupScenePathDetailed` と日本語説明 API を追加
- プロジェクトパネルが未設定文言の代わりに理由＋参照パスを表示
- `verify_launch_config_load_diagnostics.py` を CI に追加
- roadmap 当該行を `[x]`

### 推測した用途（§6.4）

- デバッグ: launch_config 欠損・JSON 壊れ・シーン JSON 不存在を UI で即切り分け
- Release／Development 起動: 空パスとファイル不存在を別メッセージで把握

### ビルド

- Cloud VM のため `buildDevelopment.cmd` 未実行

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（`・` 行） |
|--------|------|------|
| 読込診断 | 有効 | エディタ上プレイとランタイム検証の起動設定を把握しやすくする |

### 次 tick 候補

- launch_config「startupScenePath 空で false」（`fix/LaunchConfigEmptyStartupPath` 予定）

---

## セッション 2026-11-21 05:15（/loop 15m・roadmap tick）

### 完了

- `LaunchConfigLoadIssue::StartupPathEmpty` を追加（キー欠落と空文字を分離）
- `TryLoadLaunchConfigWithIssue` を公開 API 化
- プロジェクトパネル診断文言に「空だが launch_config は存在」を反映
- `verify_launch_config_empty_startup_path.py` を CI に追加
- roadmap 当該行を `[x]`

### 推測した用途（§6.4）

- デバッグ: 手編集で `""` にしたケースとファイル自体の欠損を切り分け
- Release 起動調査: false 一択ではなく UI で空パスを明示

### ビルド

- Cloud VM のため `buildDevelopment.cmd` 未実行

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（`・` 行） |
|--------|------|------|
| 空 path 診断 | 有効 | ランタイム環境でのプレイ試験における起動設定の把握 |

### 次 tick 候補

- launch_config「JSON は startupScenePath のみ」（`fix/LaunchConfigSchemaVersion` 予定）

---

## セッション 2026-11-21 05:30（/loop 15m・roadmap tick）

### 完了

- `EditorLaunchConfigSchema`（schemaVersion=1）を定義
- `EditorLaunchConfigJsonDocument::BuildDocument` で保存 JSON を共通化
- 保存時に `defaultFeatureVerificationScenePath` を同梱
- `EditorAssets/launch_config.json` をスキーマ付きに更新
- `verify_launch_config_schema_version.py` を CI に追加
- roadmap 当該行を `[x]`

### 推測した用途（§6.4）

- 手編集 launch_config: 将来キー追加と機能検証既定の参照点を JSON 上で明示
- エディタ「起動シーンに設定」: 上書き保存でもメタを欠かず残す

### ビルド

- Cloud VM のため `buildDevelopment.cmd` 未実行

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（`・` 行） |
|--------|------|------|
| launch_config スキーマ | 有効 | シーン構成の保存とランタイム／エディタ連携の起動設定の拡張余地 |

### 次 tick 候補

- EditorSceneAsset「ListSceneJson は単一 Dir」（`fix/EditorSceneAssetMultiDirList` 予定）

---

## セッション 2026-11-21 05:45（/loop 15m・roadmap tick）

### 完了

- `EditorEditableSceneJsonRoots` で編集可能シーンの走査ルート 3 件を正本化
- `ListEditableSceneJsonFiles` をループ走査に整理
- `verify_editor_scene_asset_multi_dir_list.py` を CI に追加
- `verify_scaffold_play_area_editor.py` を roots 正本に合わせて更新
- roadmap 当該行を `[x]`

### 推測した用途（§6.4）

- プロジェクト一覧: Assets/Scenes・EditorAssets/Scenes・DebugPlayArea を一画面で開ける
- 機能検証シーン選定: launch_config 保存前にパスを一覧から選べる

### ビルド

- Cloud VM のため `buildDevelopment.cmd` 未実行

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（`・` 行） |
|--------|------|------|
| 複数 Dir 一覧 | 有効 | ファイルの新規作成・削除・名前変更などをエディタから行う（シーン JSON 一覧の網羅） |

### 次 tick 候補

- EditorSceneAsset「startupScenePath を生文字列で保存」（`fix/LaunchConfigStartupPathNormalize` 予定）

---

## セッション 2026-11-21 06:00（/loop 15m・roadmap tick）

### 完了

- `EditorLaunchConfigStartupPathNormalize` を追加（lexically_normal + cwd 相対化）
- `TrySaveLaunchConfig` / `TrySetRepositoryStartupScene` で保存前に正規化
- `EditorScenePlayRequest::Set` も正規化済みパスを使用
- `verify_launch_config_startup_path_normalize.py` を CI に追加
- roadmap 当該行を `[x]`

### 推測した用途（§6.4）

- 起動シーン設定: 絶対パス入力や `\\` 混在でも launch_config を移植しやすい文字列に統一
- チーム共有: リポジトリ相対の startupScenePath で差分を安定化

### ビルド

- Cloud VM のため `buildDevelopment.cmd` 未実行

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（`・` 行） |
|--------|------|------|
| パス正規化 | 有効 | シーン構成の保存とランタイム試験（起動 JSON の移植性） |

### 次 tick 候補

- EditorSceneAsset「PlayRequest がパスだけ保持」（`fix/EditorScenePlayRequestMetadata` 予定）

---

## セッション 2026-11-21 06:15（/loop 15m・roadmap tick）

### 完了

- `EditorScenePlayStagingKind` と `EditorScenePlayRequest::Stage` を追加
- BuildUp／エディタ内プレビュー／起動シーン保存で意図を付与
- `EditorScenePlayRequestStaging.cpp` に実装を分離（パスは正規化済み）
- `verify_editor_scene_play_request_metadata.py` を CI に追加
- roadmap 当該行を `[x]`（EditorSceneAsset／launch_config 節は完了）

### 推測した用途（§6.4）

- デバッグ: 次回起動が BuildUp 予約かメモリ内プレビューかを追跡可能に
- Exit 時 Clear: BuildUp／EntityEditor 双方でステージングを解放

### ビルド

- Cloud VM のため `buildDevelopment.cmd` 未実行

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（`・` 行） |
|--------|------|------|
| PlayRequest メタ | 有効 | エディタ上プレイとランタイム起動の段階分け |

### 次 tick 候補

- Lighting 節の先頭 `- [ ]`（ファイル出現順。`progress_report` または roadmap 先頭未完了行を参照）

---

## セッション 2026-11-21 06:30（/loop 15m・roadmap tick）

### 完了（§6.2 含む）

- `EditorAssets/Visual/game_scene_lighting.json` と `Assets` 同梱を正本化（既存 JSON をベースにフォグ `color`・`startDistance` を明示）
- `volumetric_fog.md` にプロファイル JSON 節を追加
- `SceneGameLightingProfile` が `startDistance` を読込可能に
- `verify_visual_lighting_profile_json.py` を CI に追加
- roadmap `visual_lighting_profile` を `[x]`

### 推測した用途（§6.4）

- **レベル制作:** リム・フォグ・ゴッドレイ・影をカメラ tuning と同様に JSON 差し替えで調整
- **配布:** EditorAssets 正本と Assets 配布物の内容一致を CI で担保

### ビルド

- Cloud VM のため `buildDevelopment.cmd` 未実行

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（`・` 行） |
|--------|------|------|
| visual_lighting_profile | 有効 | VolumetricFog・ライトをコンポーネント／データで編集可能にする流れ |

### 次 tick 候補

- Lighting「シャドウ／シェード適用状態のログ・可視化が無い」（`directional_shadow.md`）

---

## セッション 2026-11-21 06:45（/loop 15m・roadmap tick）

### 完了

- `DirectionalShadowApplyDiagnostics` … 平面影 draw/skip・`CAST_SHADOW` マージ統計
- `DrawPlanarGroundShadow` の各 return で skip 理由を記録（無言 return を廃止）
- ライティング効果パネルに「適用状態 / 診断」オーバーレイと「適用状態をログへ」
- `directional_shadow.md` 診断節、`verify_directional_shadow_apply_diagnostics.py`（CI）
- roadmap 当該行 `[x]`

### 推測した用途（§6.4）

- **デバッグ:** 平面影が出ないとき planar OFF・radius・engine null を統計とログで切り分け
- **開発:** ImGui でシェード／CAST_SHADOW の適用状態を即確認

### ビルド

- Cloud VM のため `buildDevelopment.cmd` 未実行

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（`・` 行） |
|--------|------|------|
| Shadow 診断 | 有効 | グラフィックス調整をエディタ／デバッグで確認する流れ（間接的にライト影響の可視化） |

### 次 tick 候補

- Repository 節「サブシステム別フレームプロファイラが未実装」（`GAP-Infra-frame_profiler-001`）

---

## セッション 2026-11-21 07:00（/loop 15m・roadmap tick）

### 完了

- `SubsystemFrameProfiler` … GameOrder / Process / PreDraw / SceneDraw / PostDraw の区間計測
- ImGui オーバーレイ + 「描画診断」メニューから詳細窓
- `frame_profiler.md`・`verify_subsystem_frame_profiler.py`（CI）
- roadmap 当該行 `[x]`

### 推測した用途（§6.4）

- **デバッグ:** Update が重いのか Present 前後が重いのかを Development 実行中に即比較
- **プロファイル:** 本番相当 Release ではスコープは no-op

### ビルド

- Cloud VM のため `buildDevelopment.cmd` 未実行

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（`・` 行） |
|--------|------|------|
| frame_profiler | 有効 | エディタ／ゲーム開発のデバッグ・反復（性能当たり付けは制作効率に寄与） |

### 次 tick 候補

- 「Debug 構成も DEVELOPMENT+USE_IMGUI で本番用コンパイル差分が不明瞭」（`GAP-Ship-CG2_BuildUp-002`）

---

## セッション 2026-11-21 07:15（/loop 15m・roadmap tick）

### 完了

- `ship_build_configurations.md` で Release＝出荷候補、Development／Debug＝非出荷を明示
- `BuildConfigurationContract` … 未登録マクロ組み合わせは `#error`、起動ログに構成ラベル
- vcxproj: Development に `NDEBUG`、Debug に `MSVC_DEBUG_CONFIGURATION`
- `verify_ship_debug_compile_diff.py` を CI に追加
- roadmap 当該行 `[x]`

### 推測した用途（§6.4）

- **配布:** CI ReleaseBuild とローカル Development の意図差を手順書と検証で固定
- **デバッグ:** LogFile の「Build configuration:」で現ビルド種別を即確認

### ビルド

- Cloud VM のため `buildDevelopment.cmd` 未実行

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（`・` 行） |
|--------|------|------|
| 構成契約 | 有効 | ランタイム出力・エディター exe 化と配布時のビルド切り分け |

### 次 tick 候補

- `buildDevelopment.cmd` が CI と異なる入口で前検証も無い（`GAP-Infra-buildDevelopment-001`）

---

## セッション 2026-11-21 07:30（/loop 15m・roadmap tick）

### 完了

- `buildDevelopment.cmd` … 先に `verify_vcxproj_coverage.py`、MSBuild は `CG2_BuildUp.slnx`（Development|x64）
- `cloud_build_vcxproj_check.md` にローカル手順を追記
- `verify_build_development_ci_parity.py` を CI に追加
- roadmap 当該行 `[x]`

### 推測した用途（§6.4）

- **制作:** ローカル Development 成功が GitHub `DevelopmentBuild` の骨格と一致
- **エージェント:** Linux では verify のみ、Windows で cmd フル手順

### ビルド

- Cloud VM のため `buildDevelopment.cmd` 未実行

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（`・` 行） |
|--------|------|------|
| buildDevelopment 整合 | 有効 | エディター exe 化・配布前のビルド再現 |

### 次 tick 候補

- Release ビルド後の exe 起動スモークが CI に無い（`GAP-Infra-Release_smoke-001`）

---

## セッション 2026-11-21 07:45（/loop 15m・roadmap tick）

### 完了

- `ATRUM_RELEASE_SMOKE_FRAMES` で WinMain を自動終了（CI 専用・通常未設定）
- `runReleaseStartupSmoke.ps1` を `ReleaseBuild.yml` ビルド後に実行
- `release_startup_smoke.md`・`verify_release_startup_smoke_ci.py`（CI）
- roadmap 当該行 `[x]`

### 推測した用途（§6.4）

- **配布:** Release 成果物の exe が数フレーム再生できる退行検知
- **CI:** Windows runner 上で SDL/D3D12 初期化までのスモーク

### ビルド

- Cloud VM のため `buildDevelopment.cmd` / Release スモーク未実行

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（`・` 行） |
|--------|------|------|
| Release スモーク | 有効 | ランタイム出力・配布 exe の起動確認 |

### 次 tick 候補

- Release でも Editor 系ソースが ExcludedFromBuild されずリンクされる（`GAP-Ship-EditorSources-001`）

---

## セッション 2026-11-21 08:00（/loop 15m・roadmap tick）

### 完了

- Release|x64 でエディタ TU を `ExcludedFromBuild`（allowlist 6 件は BuildUp 用に残す）
- `editor_sources_release_link_allowlist.txt`・apply/verify スクリプト・`ship_build_configurations.md` 追記
- roadmap 当該行 `[x]`

### 推測した用途（§6.4）

- **配布:** Release バイナリからエディタ UI オブジェクトをリンク単位で除去
- **保守:** vcxproj 追加時は verify が Release 除外漏れを検知

### ビルド

- Cloud VM のため未実行（Release リンク構成は CI ReleaseBuild で確認）

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（`・` 行） |
|--------|------|------|
| Editor Release 除外 | 有効 | エディター exe 化とゲーム配布のビルド分離 |

### 次 tick 候補

- 同梱アセットとランタイム参照パスの整合（`GAP-Infra-asset_pack_verify-001`）

---

## セッション 2026-11-21 08:15（/loop 15m・roadmap tick）

### 完了

- `verify_asset_pack_paths.py` … GameAssets/EditorAssets 定数と PostBuild xcopy を突合
- `asset_pack_verify.md`、CI 追加
- `Project/Assets/Visual/game_scene_lighting.json` を PostBuild 正本へ同期、`EditorAssets/Scenes/` を用意
- roadmap 当該行 `[x]`

### 推測した用途（§6.4）

- **配布:** Release exe からの相対パスがビルド前に存在することを担保
- **制作:** ルート `Assets/` と `Project/Assets/` のズレを検知

### 既存実装の課題点

- リポジトリ直下 `Assets/` と `Project/Assets/` が併存。ランタイム正本は `Project/Assets`（PostBuild ソース）

### ビルド

- Cloud VM のため未実行

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（`・` 行） |
|--------|------|------|
| asset_pack_verify | 有効 | リソース配布・ランタイム出力の同梱整合 |

### 次 tick 候補

- エディタ既定の機能検証シーン FeatureTest（`GAP-Infra-FeatureTestScene-001`）

---

## セッション 2026-11-21 08:30（/loop 15m・roadmap tick）

### 完了

- `EditorAssets/DebugPlayArea/FeatureTest.scene.json` を同梱正本として追加
- `kDebugPlayAreaFeatureTestScene`／canonical／`launch_config`／F2・デバッグメニュー導線を FeatureTest に統一
- `verify_feature_test_scene_canonical.py`、CI 差し替え、`feature_test_scene.md`
- roadmap 当該行 `[x]`

### 推測した用途（§6.4）

- **デバッグ:** 開発者が毎回同じ最小シーンから BuildUp 経路で機能単位を試す
- **エディタ:** launch_config と F2 の既定が一覧で探さず開ける

### 既存実装の課題点

- `LoopTick13Demo.scene.json` は残置（デモ用）。正本は FeatureTest に一本化

### ビルド

- Cloud VM のため未実行

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（`・` 行） |
|--------|------|------|
| FeatureTestScene | 有効 | シーン構成の保存と、エディタ上プレイ／配布ランタイムでのプレイ導線 |

### 次 tick 候補

- `buildDevelopment.cmd` の NuGet 復元（`GAP-Infra-buildDevelopment-002`）

---

## セッション 2026-11-21 08:45（/loop 15m・roadmap tick）

### 完了

- `restoreDevelopmentNuGetPackages.cmd` … `packages.config` → `Project/packages`、SDL2 targets 存在確認
- `buildDevelopment.cmd` から復元を呼び出し（`DevelopmentBuild.yml` と同序）
- `verify_build_development_nuget_restore.py`、CI 追加、`cloud_build_vcxproj_check.md` 更新
- roadmap 当該行 `[x]`

### 推測した用途（§6.4）

- **半自律／制作:** クリーン clone 後も `buildDevelopment.cmd` 単体で SDL2 依存を揃えてビルド
- **Ship:** ローカル手順と CI の前段を一致させる

### 既存実装の課題点

- `nuget.exe` が PATH に無い環境では手動インストールが必要（CI は setup-nuget）

### ビルド

- Cloud VM（Linux）のため未実行

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（`・` 行） |
|--------|------|------|
| buildDevelopment NuGet | 有効 | エディターを exe（Development 配布）化するためのビルド手順の再現性 |

### 次 tick 候補

- Release 向け最小起動スモーク手順書（`GAP-Ship-smoke_runbook-001`）

---

## セッション 2026-11-21 09:00（/loop 15m・roadmap tick）

### 完了

- `smoke_runbook.md`（Release ビルド・自動／手動スモーク・切り分け）
- ルート `README.md` から手順書と `runReleaseStartupSmoke.ps1` へリンク
- `verify_ship_smoke_runbook.py`、CI 追加
- roadmap 当該行 `[x]`

### 推測した用途（§6.4）

- **配布:** 人手で Title 表示まで確認する共有手順
- **Ship:** CI スモークとローカル手順の対応付け

### ビルド

- Cloud VM のため未実行

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（`・` 行） |
|--------|------|------|
| smoke_runbook | 有効 | ランタイムテンプレート exe とアセットを同梱してゲームとして動かせる |

### 次 tick 候補

- ForDebug 系 cpp の Release リンク除外（`GAP-Ship-ForDebugSources-001`）

---

## セッション 2026-11-21 09:15（/loop 15m・roadmap tick）

### 完了

- `apply_for_debug_release_exclusions_to_vcxproj.py` で ForDebug TU 9 件に Release 除外
- allowlist に `Log.cpp` のみ残置（ランタイム LogFile 参照）
- `verify_for_debug_sources_release_exclude.py`、CI、`ship_build_configurations.md` 追記
- roadmap 当該行 `[x]`

### 推測した用途（§6.4）

- **配布:** デバッグ ImGui／プロファイラ UI を出荷 exe から分離
- **Ship:** Release ビルドのリンク面を契約で固定

### 既存実装の課題点

- `DirectionalShadowApplyDiagnostics.cpp` 等エンジン側デバッグ TU は別 roadmap（Release 除外未着手の可能性）

### ビルド

- Cloud VM のため未実行

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（`・` 行） |
|--------|------|------|
| ForDebugSources | 有効 | ゲーム配布時にランタイムテンプレート exe からデバッグ UI を漏らさない |

### 次 tick 候補

- Release リンク設定の GenerateDebugInformation（`GAP-Ship-CG2_BuildUp-003`）

---

## セッション 2026-11-21 09:30（/loop 15m・roadmap tick）

### 完了

- `CG2_BuildUp.vcxproj` の `Release|x64` リンクで `GenerateDebugInformation=false`
- `verify_release_link_generate_debug_info.py`、CI、`ship_build_configurations.md` 追記
- roadmap `CG2_BuildUp` 行 `[x]`（`GAP-Ship-CG2_BuildUp-003`）

### 推測した用途（§6.4）

- **配布:** 出荷フォルダに `.pdb` を残さない
- **Ship:** Release 成果物のサイズと情報漏えいリスク低減

### ビルド

- Cloud VM のため未実行

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（`・` 行） |
|--------|------|------|
| Release PDB 無効 | 有効 | ゲーム配布時にリソースを圧縮・同梱して配布する（出荷物の整理） |

### 次 tick 候補

- roadmap 先頭未完: 本番相当プロファイル計測（`Development_opt`）または `verify_config_matrix`

---

## セッション 2026-11-21 09:45（/loop 15m・roadmap tick）

### 完了

- `development_opt_profile.md`（DBG-Profiler-001・Development / Profile / Release の切り分け）
- `buildDevelopmentProfile.cmd` … Development 構成で `ClCompile_Optimization=MinSpace`
- `verify_development_opt_profile.py`、CI、`frame_profiler.md` リンク
- roadmap `Development_opt` 行 `[x]`（`GAP-Ship-Development_opt-001`）

### 推測した用途（§6.4）

- **デバッグ:** サブシステムプロファイラと Release スループットの二系統計測
- **制作:** 最適化 OFF の日常ビルドと計測用ビルドの使い分け

### ビルド

- Cloud VM のため未実行

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（`・` 行） |
|--------|------|------|
| Development_opt | 有効 | 基本機能・描画エンジンの性能を配布前に切り分けて検証 |

### 次 tick 候補

- `verify_config_matrix`（コンパイル定義と Release 除外マトリクス）

---

## セッション 2026-11-21 10:00（/loop 15m・roadmap tick）

### 完了

- `verify_config_matrix.py` … マクロ契約・Editor/ForDebug Release 除外・PDB 無効を一括実行
- `cloud_build_vcxproj_check.md` に期待マトリクス表を追記
- CI の個別 4 ステップを `verify_config_matrix` に集約
- roadmap 当該行 `[x]`（`GAP-Infra-verify_config_matrix-001`）

### 推測した用途（§6.4）

- **CI/Ship:** 意図外マクロやエディタ TU の Release 混入を単一コマンドで検知
- **Cloud Linux:** ビルド前に構成契約だけ先に検証

### ビルド

- Cloud VM のため未実行

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（`・` 行） |
|--------|------|------|
| verify_config_matrix | 有効 | ゲーム配布時に開発専用コードを同梱しない |

### 次 tick 候補

- JsonTest デバッグパネル用 TU の Release 除外（roadmap `JsonTest`）

---

## セッション 2026-11-21 10:15（/loop 15m・roadmap tick）

### 完了

- `JsonTest.cpp` に Release `ExcludedFromBuild`
- `apply_json_test_release_exclusion_to_vcxproj.py`・`verify_json_test_release_exclude.py`
- `verify_config_matrix`・`cloud_build_vcxproj_check.md`・`ship_build_configurations.md` 更新
- roadmap 当該行 `[x]`（`GAP-Ship-JsonTest-001`）

### 推測した用途（§6.4）

- **配布:** ImGui 向け JSON 実験パネルを出荷 exe に載せない

### ビルド

- Cloud VM のため未実行

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（`・` 行） |
|--------|------|------|
| JsonTest Release 除外 | 有効 | json 等のランタイム読込のみを配布物に残す |

### 次 tick 候補

- Cloud Linux 上の Development ビルド再現（`cloud_dev_build`）

---

## セッション 2026-11-21 10:30（/loop 15m・roadmap tick）

### 完了

- `runCloudDevelopmentPreflight.sh` … Linux で実行可能な Development 前段検証バンドル
- `cloud_dev_build.md`・`cloud_build_vcxproj_check.md` 追記
- `verify_cloud_dev_build.py`（プリフライト実行込み）を CI に追加
- roadmap 当該行 `[x]`（`GAP-Infra-cloud_dev_build-001`）

### 推測した用途（§6.4）

- **半自律／Cloud Agent:** Windows MSBuild 無しでも tick 完了判定の代替手順
- **CI:** Linux job で手順の退行検知

### ビルド

- 本 tick 環境で `runCloudDevelopmentPreflight.sh` を実行済み（`verify_cloud_dev_build` 経由）

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（`・` 行） |
|--------|------|------|
| cloud_dev_build | 有効 | エディター exe 化の前段として Development 手順を再現可能にする |

### 次 tick 候補

- スモーク起動用 CLI（roadmap `main`・`GAP-Ship-main-001` 付近）

---

## セッション 2026-11-21 10:45（/loop 15m・roadmap tick）

### 完了

- `ReleaseStartupSmokeCliParse` / `ResolveReleaseStartupSmokeFrameBudget`
- `main` が `lpCmdLine` の `--smoke-frames` を優先（環境変数はフォールバック）
- `runReleaseStartupSmoke.ps1` を CLI 起動に変更、`verify_release_startup_smoke_cli.py`、CI
- roadmap 当該行 `[x]`（`GAP-Ship-main-001`）

### 推測した用途（§6.4）

- **CI/Ship:** 非対話で N フレーム後に終了する公式フック
- **手動:** コマンドラインからスモーク長を指定

### ビルド

- Cloud VM のため未実行（C++ 追加は vcxproj 登録済み）

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（`・` 行） |
|--------|------|------|
| main smoke CLI | 有効 | 配布ビルドのランタイムを最小起動で検証する |

### 次 tick 候補

- roadmap 先頭の Editor 系 `- [ ]`（例: StageViewCameraTuning）

---

## セッション 2026-11-21 11:00（/loop 15m・roadmap tick）

### 完了

- 既存 `StageViewCameraTuningPanel` を正本化（Game view プレビュー／自動ホットリロード）
- `EditorStageViewCameraTuningHotReloadTick` … パネル折りたたみ時も `PollHotReload`
- `verify_stage_view_camera_tuning_editor_preview.py`、CI、`odyssey_style_stage_camera.md` 更新
- roadmap 当該行 `[x]`（`GAP-Editor-StageViewCameraTuning-001`）

### 推測した用途（§6.4）

- **制作:** JSON を外部エディタで編集しながら Game ビューで戦闘カメラを即確認
- **Authoring:** CLI プレビューに加えエディタ内ループを短縮

### 既存実装の課題点

- focus ポーズの Game view プレビューは battle のみ（focus ブレンドは別タスク余地）

### ビルド

- Cloud VM のため未実行

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（`・` 行） |
|--------|------|------|
| StageViewCameraTuning | 有効 | インスペクタ／プレビューで制作パラメータを編集し結果を確認 |

### 次 tick 候補

- DebugPlayArea 用 EditorAssets パス定数（`GAP-Editor-EditorAssetsPaths-001`）

---

## セッション 2026-11-21 11:15（/loop 15m・roadmap tick）

### 実装済みと判断した項目（コード確認・即時 [x]）

- `EditorAssetsPaths.h` に DebugPlayArea 一式の定数が既に存在（directory / template / FeatureTest / combat_bounds）
- 今 tick は `verify_editor_assets_paths_debug_play_area.py` とドキュメント突合でギャップを解消

### 推測した用途（§6.4）

- **デバッグ:** テスト再生 JSON の場所をエディタコードで一意参照
- **制作:** プロジェクト一覧・scaffold・BuildUp 起動が同じパス契約を共有

### ビルド

- Cloud VM のため未実行

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（`・` 行） |
|--------|------|------|
| EditorAssetsPaths | 有効 | エディタでシーン構成を保存しプレイ試験する導線 |

### 次 tick 候補

- シーン切替時の未保存確認（`GAP-Editor-EntityEditor-002`）

---

## セッション 2026-11-21 11:30（/loop 15m・roadmap tick）

### 完了タスク

- `GAP-Editor-EntityEditor-002`: シーン切替時の未保存確認（保存して開く／保存せず開く／キャンセル）
- `EntityEditorSceneSwitchUnsavedPrompt`、dirty 追跡、`TryOpenSceneFromPath`
- CI: `verify_entity_editor_scene_switch_unsaved.py`

### 推測した用途（§6.4）

- **制作（Authoring）:** 別シーンを開く前に編集内容を失わないよう確認する
- **デバッグ:** DebugPlayArea scaffold のシーン生成後も同じ切替ポリシーを適用

### ビルド

- Cloud VM: `runCloudDevelopmentPreflight.sh` OK（MSBuild 未実行）

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（`・` 行） |
|--------|------|------|
| EntityEditor 未保存確認 | 有効 | シーンの構成を保存することと、エディタ上でのプレイを可能にしてある |

### 次 tick 候補

- エディタメニューに機能検証シーン起動（`GAP-Editor-EditorEntityWorkbench-001`）

---

## セッション 2026-11-21 11:45（/loop 15m・roadmap tick）

### 完了タスク

- `GAP-Editor-EditorEntityWorkbench-001`: 「エディタ」メニューから FeatureTest 正本の SceneBuildUp 起動
- `EditorEntityWorkbenchFeatureTestMenuLaunch` でデバッグメニューと共通化
- `verify_feature_test_scene_canonical.py` にエディタメニュー検査を追加

### 推測した用途（§6.4）

- **デバッグ:** F2 以外でもメニューから最小構成のランタイム機能検証へ遷移
- **制作:** パーティクルエディタと同列のエディタ導線で検証シーンにアクセス

### ビルド

- Cloud VM: `runCloudDevelopmentPreflight.sh` OK（MSBuild 未実行）

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（`・` 行） |
|--------|------|------|
| エディタメニュー機能検証 | 有効 | シーンの構成を保存し、エディタ上プレイと配布ランタイムプレイを可能に |

### 次 tick 候補

- テスト再生エリア JSON がエディタのシーン一覧に出ない（`GAP-Editor-EntityEditorUi-004`）

---

## セッション 2026-11-21 12:00（/loop 15m・roadmap tick）

### 完了タスク

- `GAP-Editor-EntityEditorUi-004`: DebugPlayArea をプロジェクトパネルで「テスト再生エリア」グループ表示
- `EditorSceneJsonFileEntry` で `*.scene.json` を一覧対象に明示
- DebugPlayArea 配下のシーン JSON 削除を許可

### 推測した用途（§6.4）

- **デバッグ:** scaffold 生成物や FeatureTest を一覧から開いてエディタ内プレビュー／BuildUp
- **制作:** 本編 Scenes とテスト用 JSON の場所を UI 上で区別

### ビルド

- Cloud VM: `runCloudDevelopmentPreflight.sh` OK（MSBuild 未実行）

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（`・` 行） |
|--------|------|------|
| DebugPlayArea 一覧 | 有効 | エディタ上でシーン構成を編集・プレイ試験する導線 |

### 次 tick 候補

- スロット種別不一致のコンポーネント無警告スキップ（`GAP-Editor-EditorSceneRuntime-002`）

---

## セッション 2026-11-21 12:15（/loop 15m・roadmap tick）

### 完了タスク

- `GAP-Editor-EditorSceneRuntime-002`: `TryAddEmptyComponent` 失敗時に `EditorSceneRuntimeBuildWarnings` で理由を記録
- プレビュー再構築時、プロジェクトパネル `importStatusMessage` に表示（種別不一致／未対応型）

### 推測した用途（§6.4）

- **制作（Authoring）:** JSON 手編集で Draw/Upd/Data を誤ったときに黙って消えない
- **デバッグ:** プレビュー適用直後にどの Entity のどのコンポーネントが落ちたか確認

### ビルド

- Cloud VM: `runCloudDevelopmentPreflight.sh` OK（MSBuild 未実行）

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（`・` 行） |
|--------|------|------|
| スロット種別警告 | 有効 | コンポーネント構成を json として編集できる |

### 次 tick 候補

- エディットビュー配置ギズモ（`GAP-Editor-SceneEntityEditor-002`）

---

## セッション 2026-11-21 12:30（/loop 15m・roadmap tick）

### 完了タスク

- `GAP-Editor-SceneEntityEditor-002`: エディットビューで選択オブジェクトを床面（Y=0.05）左ドラッグ移動
- 選択位置に RGB 軸ラインの簡易ギズモ表示、`EditorEditViewScreenRay` でピック

### 推測した用途（§6.4）

- **制作（Authoring）:** インスペクタ数値入力なしでシーン内配置を調整し JSON 保存
- **デバッグ:** 右ドラッグは従来どおりカメラ、左ドラッグはオブジェクト移動

### ビルド

- Cloud VM: `runCloudDevelopmentPreflight.sh` OK（MSBuild 未実行）

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（`・` 行） |
|--------|------|------|
| エディットビュー配置 | 有効 | シーンの構成を保存しエディタ上でプレイ試験できる |

### 次 tick 候補

- 選択エンティティ複製 UI（`GAP-Editor-EntityEditor-001`）

---

## セッション 2026-11-21 12:45（/loop 15m・roadmap tick）

### 完了タスク

- `GAP-Editor-EntityEditor-001`: ヒエラルキー「複製」ボタンと `DuplicateSelectedInstance`
- 同一親を維持し、名前は `_Copy` 連番、配置は +0.5 XZ オフセット

### 推測した用途（§6.4）

- **制作（Authoring）:** レベル内の反復配置をプレハブ往復なしで 1 クリック
- **デバッグ:** 既存定義のバリエーションを素早く増やして試す

### ビルド

- Cloud VM: `runCloudDevelopmentPreflight.sh` OK（MSBuild 未実行）

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（`・` 行） |
|--------|------|------|
| インスタンス複製 | 有効 | コンポーネント構成を json として作成・編集できる |

### 次 tick 候補

- 親付きインスタンスの Initialize（`GAP-Editor-EditorSceneRuntime-006`）

---

## セッション 2026-11-21 13:00（/loop 15m・roadmap tick）

### 実装済みと判断した項目（コード確認・即時 [x]）

- `BuildEntitiesFromSceneDocument` は `SetParent` 後に `InitializeSceneDocumentEntities` を呼ぶ
- ポリシーはルート `Initialize` の子再帰＋最大 8 パスの取り残し初期化
- `verify_editor_scene_runtime_initialize_hierarchy.py` OK（CI 登録済み）

### 推測した用途（§6.4）

- **プレイ:** 階層付きシーン JSON を BuildUp／プレビューで開いても子の Upd Cmp が動く
- **制作:** 親子ヒエラルキー付きシーンをエディタ保存後、そのまま実行確認

### ビルド

- Cloud VM: `runCloudDevelopmentPreflight.sh` OK（MSBuild 未実行）

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（`・` 行） |
|--------|------|------|
| 親子 Initialize | 有効 | ゲーム中に保存したエンティティを生成し、シーン構成をプレイできる |

### 次 tick 候補

- ライト・ポスト系パラメータのエディタ UI（`GAP-Editor-AtrumEngineVisual-001`）

---

## セッション 2026-11-21 13:15（/loop 15m・roadmap tick）

### 完了タスク

- `GAP-Editor-AtrumEngineVisual-001`: 入場時 `game_scene_lighting.json` 反映と「エディタ」メニューから Fog/GodRays/影パネル
- シーンビュー描画は CB フラッシュのみとし、ImGui 調整が上書きされないよう整理

### 推測した用途（§6.4）

- **制作（Authoring）:** 本編と同じライト・ポストをプレビューしながらパラメータ調整
- **デバッグ:** デバッグメニューに加えエディタメニューから同一パネルを開く

### ビルド

- Cloud VM: `runCloudDevelopmentPreflight.sh` OK（MSBuild 未実行）

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（`・` 行） |
|--------|------|------|
| ライト・ポスト UI | 有効 | シーン構成の編集とエディタ上プレイ試験 |

### 次 tick 候補

- エディタプレビューにフォグ／リム／ゴッドレイ／影（`GAP-Editor-SceneEntityEditor-004`）

---

## セッション 2026-11-21 13:30（/loop 15m・roadmap tick）

### 完了タスク

- `GAP-Editor-SceneEntityEditor-004`: `SceneEntityEditorSceneViewVisualEffectsApply` で描画直前に Fog / Rim / GodRays / 影 CB をフラッシュ
- 実行ビュー（`DrawEditorSceneRuntimeFunctionalView`）でもカメラ設定後に同処理を適用
- `verify_scene_entity_editor_preview_visual_effects.py` を CI に追加

### 推測した用途（§6.4）

- **制作（Authoring）:** 入場時の `game_scene_lighting.json` 反映に加え、各シーンビュー描画で本編相当のポストを確認
- **実行タブ:** BuildUp 相当の機能描画でも平行光のみにならない

### ビルド

- Cloud VM: `runCloudDevelopmentPreflight.sh` OK（MSBuild 未実行）

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（`・` 行） |
|--------|------|------|
| プレビュー視覚効果 | 有効 | シーン構成の編集とエディタ上プレイ試験 |

### 次 tick 候補

- CmpMesh／Billboard が TryAddEmptyComponent に無い（`GAP-Editor-EditorSceneRuntime-003`）

---

## セッション 2026-11-21 13:45（/loop 15m・roadmap tick）

### 完了タスク

- `GAP-Editor-EditorSceneRuntime-003`: `CmpMesh` / `CmpEditorTextureBillboard` を Definition・Ui プリセット・`TryAddEmptyComponent` に登録
- スロットパラメータから obj／画像パスをプレビュー用ロード（`modelAssetPath` / `textureFilePath`）
- `verify_editor_component_registration.py` の関数名パース修正、`verify_editor_scene_runtime_cmp_mesh_billboard_try_add.py` 追加

### 推測した用途（§6.4）

- **制作（Authoring）:** component_split_guidelines の分割例どおり、メッシュ／ビルボードを独立スロットで組み立て
- **アセット試作:** AssetPath 副作用に頼らず直指定でもプレビュー表示

### ビルド

- Cloud VM: `runCloudDevelopmentPreflight.sh` OK（MSBuild 未実行）

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（`・` 行） |
|--------|------|------|
| 描画コンポーネント分割 | 有効 | 各エンティティのコンポーネント構成を json で作成／編集できる |

### 次 tick 候補

- インスペクタ変更は自動保存しない（`GAP-Editor-EntityEditor-004`）

---

## セッション 2026-11-21 14:00（/loop 15m・roadmap tick）

### 完了タスク

- `GAP-Editor-EntityEditor-004`: `EntityEditorAuthoringDocumentEditNotify` でインスペクタ／タグ／ヒエラルキー／配置ドラッグ確定後に `requestSaveToDisk` をキュー
- ドラッグ中は `NotifyPreviewOnly` で毎フレームのディスク書き込みを回避

### 推測した用途（§6.4）

- **制作（Authoring）:** パラメータや配置変更のたびに手動保存し忘れても JSON に残る
- **レビュー:** `jsonFilePath` 未設定時は従来どおりプレビューのみ

### ビルド

- Cloud VM: `runCloudDevelopmentPreflight.sh` OK（MSBuild 未実行）

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（`・` 行） |
|--------|------|------|
| 自動保存 | 有効 | 各エンティティのコンポーネント構成やパラメータを json で作成／保存／編集できる |

### 次 tick 候補

- AssetPath プレビューが Draw 分割を Data に内包（`GAP-Editor-EditorSceneRuntime-005`）

---

## セッション 2026-11-21 14:15（/loop 15m・roadmap tick）

### 完了タスク

- `GAP-Editor-EditorSceneRuntime-005`: `EditorSceneRuntimeEditorAssetPathPreviewLink` で AssetPath を宣言済み `CmpMesh` / `CmpEditorTextureBillboard` にのみ反映（暗黙 `AddDrawCmp` 廃止）
- 描画ロードを `EditorSceneRuntimeDrawPreviewApply` に集約

### 推測した用途（§6.4）

- **制作:** json に Draw スロットを残したまま Data と描画を分離して再利用
- **移行:** Draw 無しの旧定義はビルド警告で不足を明示

### ビルド

- Cloud VM: `runCloudDevelopmentPreflight.sh` OK（MSBuild 未実行）

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（`・` 行） |
|--------|------|------|
| コンポーネント分割 | 有効 | 各エンティティのコンポーネント構成を json で作成／編集できる |

### 次 tick 候補

- BuildEntities が camera.main 欠落を補わない（`GAP-Editor-EditorSceneRuntime-007`）

---

## セッション 2026-11-21 14:30（/loop 15m・roadmap tick）

### 完了タスク

- `GAP-Editor-EditorSceneRuntime-007`: `EditorSceneRuntimeMainCameraSupplement` で `BuildEntitiesFromSceneDocument` 直前に `ScenePrimaryCameraResolve` 契約で MainCamera を自動追加
- 既定プレハブ → `EntityFactory::MainCameraEntity` の順で `SceneCameraRig` と同等

### 推測した用途（§6.4）

- **デバッグ:** カメラ無しシーン JSON でもエディタプレビュー／機能検証が描画可能
- **Authoring:** `CmpCamera` + `camera.main` タグがあるシーンは従来どおり上書きなし

### ビルド

- Cloud VM: `runCloudDevelopmentPreflight.sh` OK（MSBuild 未実行）

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（`・` 行） |
|--------|------|------|
| MainCamera 補完 | 有効 | シーン構成をエディタ上プレイ試験できる |

### 次 tick 候補

- BuildPreviewScene がエディタ内ベクタのみ（`GAP-Editor-EntityEditor-011`）

---

## セッション 2026-11-21 14:45（/loop 15m・roadmap tick）

### 完了タスク

- `GAP-Editor-EntityEditor-011`: `EntityEditorBuildPreviewSceneBuildUpParity` で BuildUp と同じタグ同期後に `BuildEntitiesFromSceneDocument`、PlayRequest と `sceneBuildUpLaunchPath` を同期

### 推測した用途（§6.4）

- **デバッグ:** エディタプレビューと BuildUp 実行が同じ JSON→Entity 契約になる
- **FeatureScene 検証:** プレビュー再構築のたびに実行用パスが PlayRequest に載る

### ビルド

- Cloud VM: `runCloudDevelopmentPreflight.sh` OK（MSBuild 未実行）

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（`・` 行） |
|--------|------|------|
| BuildUp プレビュー接続 | 有効 | シーン構成をエディタ上プレイ試験できる |

### 次 tick 候補

- roadmap P1 ギャップスキャン追補または EntityEditor 節の次 `- [ ]`（ファイル出現順）

---

## セッション 2026-11-21 15:00（/loop 15m・roadmap tick）

### 完了タスク

- `GAP-Editor-EntityEditor-003`（roadmap 行）: `EntityEditorSceneDocumentLoadFromPath` で欠落／オープン失敗時に `importStatusMessage` を設定
- パス不存在時のみ空シーンへ置換（従来挙動）＋理由を明示

### 推測した用途（§6.4）

- **制作:** 誤パスや壊れた JSON で黙って空シーンになるのを防ぎ、再保存ミスを抑制
- **デバッグ:** 開けないファイルは前の Document を維持

### ビルド

- Cloud VM: `runCloudDevelopmentPreflight.sh` OK（MSBuild 未実行）

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（`・` 行） |
|--------|------|------|
| シーン JSON 読込通知 | 有効 | json データとしてシーン構成を作成／保存／編集できる |

### 次 tick 候補

- ProcessDroppedFileImports が Entities 生成まで繋がない（roadmap EntityEditor 節）

---

## セッション 2026-11-21 15:15（/loop 15m・roadmap tick）

### 完了タスク

- roadmap EntityEditor 節: `ProcessDroppedFileImports` が Imported コピー後にシーンへインスタンスを生成するよう接続
- `EntityEditorDroppedImportSpawnSceneInstances`: obj → `CmpMesh` + `CmpEditorAssetPath`、png/jpg → `CmpEditorTextureBillboard` + AssetPath。成功時 `EntityEditorAuthoringDocumentEditNotify`
- CI: `verify_entity_editor_dropped_import_spawn_instances.py`

### 推測した用途（§6.4）

- **制作:** D&D で obj／画像をシーンへ即配置し、インスペクタ微調整へつなぐ（AUT-AssetLoop-001）
- **Authoring:** Imported 相対パスがコンポーネントに載りプレビュー再構築と自動保存が走る

### ビルド

- Cloud VM: `runCloudDevelopmentPreflight.sh` OK（MSBuild 未実行）

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（`・` 行） |
|--------|------|------|
| ドロップ→Entity 生成 | 有効 | 各エンティティのコンポーネント構成を json として作成／編集できる流れを進める |

### 次 tick 候補

- プレハブから先頭 1 体のみ追加し親は常に解除（roadmap 590 行）

---

## セッション 2026-11-21 15:30（/loop 15m・roadmap tick）

### 完了タスク

- roadmap EntityEditor 節: `EntityEditorPrefabSpawnFirstInstanceRoot` でプレハブ JSON から **先頭 1 体のみ** 取得し `parentIndex = -1` でシーンへ追加
- シーン形式（`entities` 配列）と単体 `EntityDefinition` の両方に対応。インスタンス名はシーン内で一意化
- `AddInstanceFromPrefabPath` は上記モジュールへ委譲
- CI: `verify_entity_editor_prefab_spawn_first_instance_root.py`

### 推測した用途（§6.4）

- **制作:** プレハブをルートに 1 体ずつ配置し、保存済みの親子階層を誤ってシーンへ展開しない
- **Authoring:** 同一プレハブの繰り返し配置で名前衝突を避け、プレビュー／自動保存へつなぐ

### ビルド

- Cloud VM: `runCloudDevelopmentPreflight.sh` OK（MSBuild 未実行）

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（`・` 行） |
|--------|------|------|
| プレハブ先頭 1 体・親解除 | 有効 | エンティティ構成を json として編集しプレハブ化／配置する流れを明確化 |

### 次 tick 候補

- AddEmptyInstance が選択中定義を複製しない（roadmap 591 行）

---

## セッション 2026-11-21 15:45（/loop 15m・roadmap tick）

### 完了タスク

- `GAP-Editor-EntityEditor-008`（roadmap 行）: `EntityEditorAddEmptyInstanceFromSelection` で「オブジェクトを追加」時に **選択中の `EntityDefinition` を複製**（シーン空時のみ既定定義）
- 新規インスタンス名は従来どおり `Entity_<n>`、親はルート（`-1`）

### 推測した用途（§6.4）

- **制作:** 同一コンポーネント構成のバリエーションを連続配置し、複製ボタンなしでもテンプレ追加ができる
- **Authoring:** 配置反復の手順を減らし、プレビュー／自動保存へつなぐ

### ビルド

- Cloud VM: `runCloudDevelopmentPreflight.sh` OK（MSBuild 未実行）

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（`・` 行） |
|--------|------|------|
| 追加＝選択定義複製 | 有効 | エンティティのコンポーネント構成を json として編集する反復作業を短縮 |

### 次 tick 候補

- RemoveSelectedInstance が単一 index のみ（roadmap 592 行）

---

## セッション 2026-11-21 16:00（/loop 15m・roadmap tick）

### 完了タスク

- `GAP-Editor-EntityEditor-009`（roadmap 行）: `EntityEditorRemoveSelectedInstances` で **複数選択（`selectedInstanceIndices`）を一括削除**
- 親 index は `oldToNew` で再マップ。全選択時は主選択（`selectedInstanceIndex`）を残し最低 1 体を維持

### 推測した用途（§6.4）

- **制作:** Ctrl+複数選択後に試配置をまとめて整理
- **Authoring:** ヒエラルキー削除が複数選択 UI と一致

### ビルド

- Cloud VM: `runCloudDevelopmentPreflight.sh` OK（MSBuild 未実行）

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（`・` 行） |
|--------|------|------|
| 複数削除 | 有効 | シーン構成の json 編集における削除／整理を短縮 |

### 次 tick 候補

- 選択エンティティの複製が無い（roadmap 593 行・§6.2 で既存複製 UI の整合確認想定）

---

## セッション 2026-11-21 16:15（/loop 15m・roadmap tick）

### 完了タスク

- `GAP-Editor-EntityEditor-012`（roadmap 行）: `DuplicateSelectedInstanceInScene` を **複数選択対応**に拡張（`selectedInstanceIndices` 全件を `_Copy` 付きで複製）
- ヒエラルキー「複製」ボタン＋既存 verify を `EntityEditorAuthoringDocumentEditNotify` 基準に更新

### 推測した用途（§6.4）

- **制作:** Ctrl+複数選択後にワンクリックで試配置を複製
- **Authoring:** 一括削除・一括スケールと対になる複製導線

### ビルド

- Cloud VM: `runCloudDevelopmentPreflight.sh` OK（MSBuild 未実行）

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（`・` 行） |
|--------|------|------|
| 選択複製 | 有効 | エンティティのコンポーネント構成を編集する反復作業を短縮 |

### 次 tick 候補

- requestSaveToDisk が needApply を立てない（roadmap 594 行）

---

## セッション 2026-11-21 16:30（/loop 15m・roadmap tick）

### 完了タスク

- `GAP-Editor-EntityEditor-016`（roadmap 行）: `EntityEditorTickPendingRequests` で `requestSaveToDisk` 処理後に **`needApply = true`**（`SceneEntityEditor` の `RebuildPreviewScene` へ接続）
- パス空のときは保存をスキップ

### 推測した用途（§6.4）

- **制作:** 自動保存直後にランタイムプレビューがディスク内容と同期
- **デバッグ:** SceneBuildUp 前の明示保存とプレビュー再構築の挙動を揃える

### ビルド

- Cloud VM: `runCloudDevelopmentPreflight.sh` OK（MSBuild 未実行）

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（`・` 行） |
|--------|------|------|
| 保存後プレビュー | 有効 | シーン構成をエディタ上プレイ試験できる流れを維持 |

### 次 tick 候補

- LoadFromJsonPath が選択を先頭に戻す（roadmap 595 行）

---

## セッション 2026-11-21 16:45（/loop 15m・roadmap tick）

### 完了タスク

- `GAP-Editor-EntityEditor-014`（roadmap 行）: `EntityEditorRestoreSelectionAfterSceneLoad` でロード前の **instanceName ベース**に選択を復元（複数選択・コンポーネント index も可能な範囲で維持）
- `ApplySceneDocumentLoadedFromDisk` / 欠落 JSON 空シーン置換の両方で適用

### 推測した用途（§6.4）

- **制作:** ディスク再読込（自動保存後のリロード）でも編集対象インスタンスを維持
- **Authoring:** シーン切替で名前が一致すれば選択を引き継ぐ

### ビルド

- Cloud VM: `runCloudDevelopmentPreflight.sh` OK（MSBuild 未実行）

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（`・` 行） |
|--------|------|------|
| ロード後選択維持 | 有効 | シーン json の編集反復を短縮 |

### 次 tick 候補

- 削除後は remaining.front() のみ開く（roadmap 596 行）

---

## セッション 2026-11-21 17:00（/loop 15m・roadmap tick）

### 完了タスク

- `GAP-Editor-EntityEditor-013`（roadmap 行）: `EntityEditorDeleteSceneOpenSuccessor` で削除前一覧上の位置から **次／前のシーン** を開く（`remaining.front()` 固定を廃止）
- 残シーン無しは従来どおり `CreateNewSceneFile`

### 推測した用途（§6.4）

- **制作:** シーン整理の連続削除で意図しない先頭シーンへ飛ばない
- **Authoring:** 一覧順（ソート済みパス）に沿った作業コンテキスト維持

### ビルド

- Cloud VM: `runCloudDevelopmentPreflight.sh` OK（MSBuild 未実行）

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（`・` 行） |
|--------|------|------|
| 削除後シーン遷移 | 有効 | json シーンを作成／編集する流れの連続作業を維持 |

### 次 tick 候補

- プレハブ追加は instances[0]（roadmap 597 行・§6.2 既存実装との突合想定）

---

## セッション 2026-11-21 17:15（/loop 15m・roadmap tick）

### 完了タスク（§6.2 実装済み判定）

- `GAP-Editor-EntityEditor-021`（roadmap 597 行）: `EntityEditorPrefabSpawnFirstInstanceRoot` がシーン形式プレハブを **`instances.front()`（= [0]）のみ** 追加していることを確認
- 新規 verify: `verify_entity_editor_prefab_add_index_zero.py`（一括ループ禁止の静的検査）

### 推測した用途（§6.4）

- **制作:** マルチエントリプレハブ JSON を誤って全展開しない（1 クリック 1 体の契約を明示）
- **Authoring:** tick 590 の先頭 1 体・親解除と同じ経路の二重チェック

### ビルド

- Cloud VM: `runCloudDevelopmentPreflight.sh` OK（MSBuild 未実行）

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（`・` 行） |
|--------|------|------|
| プレハブ [0] のみ | 有効（実装済み） | プレハブ化／配置の json 編集方針を既存モジュールで満たす |

### 次 tick 候補

- 削除時に子を祖父へ昇格しない（roadmap 598 行）

---

## セッション 2026-11-21 17:30（/loop 15m・roadmap tick）

### 完了タスク

- `GAP-Editor-EntityEditor-018`（roadmap 行）: `EntityEditorRemoveSelectedInstances` で親削除時、子の `parentIndex` を **削除されていない祖先**へ再付け（ルートへのフラット化のみを廃止）

### 推測した用途（§6.4）

- **制作:** グループ親を消しても配下のヒエラルキーを維持
- **Authoring:** 複数選択削除と併せて親子 index を再マップ

### ビルド

- Cloud VM: `runCloudDevelopmentPreflight.sh` OK（MSBuild 未実行）

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（`・` 行） |
|--------|------|------|
| 削除時の親子維持 | 有効 | エンティティ構成の json 編集でヒエラルキー整理を非破壊に |

### 次 tick 候補

- 新規作成が requestSaveToDisk 必須（roadmap 599 行）

---

## セッション 2026-11-21 17:45（/loop 15m・roadmap tick）

### 完了タスク

- `GAP-Editor-EntityEditor-015`（roadmap 行）: `EntityEditorCreateNewSceneFile` で新規シーンは **`NotifyPreviewOnly`**（`requestSaveToDisk` を立てない）でメモリ草案として試せるように変更

### 推測した用途（§6.4）

- **制作:** 保存前に複数案をエディタプレビューだけで試す
- **Authoring:** Scenes 配下へ意図しない空 JSON が自動増殖しない

### ビルド

- Cloud VM: `runCloudDevelopmentPreflight.sh` OK（MSBuild 未実行）

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（`・` 行） |
|--------|------|------|
| 新規シーン草案 | 有効 | json シーンの作成／編集でファイル新規作成と内部編集の棲み分け |

### 次 tick 候補

- 起動設定前に SaveToJsonPath（roadmap 600 行）

---

## セッション 2026-11-21 18:00（/loop 15m・roadmap tick）

### 完了タスク

- `GAP-Editor-EntityEditor-019`（roadmap 行）: `EntityEditorStartupSceneSetWithoutAutosave` で起動シーン設定時の **暗黙 SaveToJsonPath を廃止**
- 「設定して BuildUp」は launch_config のみ即時更新し、編集中パス一致時だけ `requestSaveToDisk` をキュー

### 推測した用途（§6.4）

- **制作:** 未保存の試行内容を勝手にディスク確定せず Release 起動パスだけ切替
- **デバッグ:** 明示保存／SceneBuildUp ボタンとの操作対称

### ビルド

- Cloud VM: `runCloudDevelopmentPreflight.sh` OK（MSBuild 未実行）

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（`・` 行） |
|--------|------|------|
| launch と保存の分離 | 有効 | ファイル操作と json 編集の棲み分け |

### 次 tick 候補

- プレハブ出力が Definition のみ（roadmap 601 行）

---

## セッション 2026-11-21 18:15（/loop 15m・roadmap tick）

### 完了タスク

- roadmap / `GAP-Editor-EntityEditor-020` 相当: `EntityEditorSaveSelectedInstanceToPrefab` で **SceneEntityInstance**（instanceName + definition + initialTransform）をプレハブ JSON へ保存
- `TryBuildRootInstanceFromPrefabJson` は `definition` ラッパー形式と従来 **EntityDefinition のみ** JSON の双方を読込可能

### 推測した用途（§6.4）

- **制作:** 配置・名前・変形込みのプレハブをそのまま再配置
- **Authoring:** シーン JSON を手切り出しせずプレハブ循環

### ビルド

- Cloud VM: `runCloudDevelopmentPreflight.sh` OK（MSBuild 未実行）

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（`・` 行） |
|--------|------|------|
| プレハブ实例保存 | 有効 | エンティティをプレハブ化して json アセット保存する流れを強化 |

### 次 tick 候補

- ループ内で lastImported が上書き（roadmap 602 行）

---

## セッション 2026-11-21 18:30（/loop 15m・roadmap tick）

### 完了タスク

- `GAP-Editor-EntityEditor-017`（roadmap 行）: `EntityEditorDroppedImportPathQueue` で複数ドロップの **Imported パスを `recentImportedAssetPaths` に一括記録**（ループ内の `lastImported` 上書きを廃止）
- UI にドロップキュー一覧を表示。`lastImportedAssetPath` は末尾（最新）を維持

### 推測した用途（§6.4）

- **制作:** 同時ドロップした obj/png すべてを確認し、インスペクタの「直近ドロップ適用」と併用
- **Authoring:** AUT-AssetLoop 一括インポートの可視化

### ビルド

- Cloud VM: `runCloudDevelopmentPreflight.sh` OK（MSBuild 未実行）

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（`・` 行） |
|--------|------|------|
| ドロップキュー | 有効 | 外部ツール連携用アセット（obj/png）の取り込みを把握しやすくする |

### 次 tick 候補

- インポート後プレビュー再適用なし（roadmap 603 行）

---

## セッション 2026-11-21 18:45（/loop 15m・roadmap tick）

### 完了タスク

- `GAP-Editor-EntityEditor-020`（roadmap 603 行）: `EntityEditorDroppedImportRequestPreview` でドロップ処理後に **`requestApplyPreview` を必ずキュー**（spawn 済みは spawn 側 Notify、コピーのみは `NotifyPreviewOnly`）

### 推測した用途（§6.4）

- **制作:** D&D 直後にエディタプレビューへ反映（手動「エディタ内プレビュー」待ちを削減）
- **デバッグ:** 一括ドロップ後も `TickPendingRequests` → `RebuildPreviewScene` へ接続

### ビルド

- Cloud VM: `runCloudDevelopmentPreflight.sh` OK（MSBuild 未実行）

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（`・` 行） |
|--------|------|------|
| インポート後プレビュー | 有効 | シーン構成をエディタ上プレイ試験できる流れを維持 |

### 次 tick 候補

- EntityEditor 節は 603 まで完了。次はファイル出現順: SceneEntityEditor プレビューと本番 SceneBuildUp 経路の差分（roadmap 583 行）または EditorSceneRuntime 606 行

---

## セッション 2026-11-21 19:00（/loop 15m・roadmap tick）

### 完了タスク

- roadmap 583 行: `EntityEditorBuildPreviewSceneBuildUpParity` の実体生成を **`SceneBuildUpSceneDocumentLoad::BuildEntities` に統一**（本番 SceneBuildUp と同一関数）
- `SceneEntityEditorPreviewBuildUpParityRebuild` 経由で `RebuildPreviewScene` が BuildUp パリティ経路であることを明示
- verify: `verify_scene_entity_editor_preview_buildup_parity.py`、既存 parity verify を `BuildEntities` 参照に更新

### 推測した用途（§6.4）

- **制作:** エディタ内プレビューと BuildUp プレイでエンティティ構成の乖離を抑える
- **デバッグ:** シーン JSON 編集の見た目が本番ロード経路と一致しやすくする

### ビルド

- Cloud VM: `runCloudDevelopmentPreflight.sh` OK（MSBuild 未実行）

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（`・` 行） |
|--------|------|------|
| プレビュー／BuildUp 経路 | 有効 | エディタ上プレイとランタイム環境上プレイの流れを整合 |

### 次 tick 候補

- EntityHasComponentForSlot が Mesh／Billboard を認識しない（roadmap 606 行）

---

## セッション 2026-11-21 19:15（/loop 15m・roadmap tick）

### 完了タスク

- roadmap 606 行 / `GAP-Editor-EditorSceneRuntime-004`: **`EditorSceneRuntimeEntityHasComponentForSlot`** へスロット突合を切り出し、`CmpMesh` と `CmpEditorTextureBillboard` の `HasDrawCmp` 判定を検証で固定（§6.2: 本体ロジックは既存行にあったがモジュール化で契約を明示）

### 推測した用途（§6.4）

- **制作:** component_split_guidelines どおり JSON に Draw スロットを書いたとき、AssetPath 副作用なしで存在検出できる
- **Authoring:** 手編集 JSON の CmpMesh 再追加ループを防ぐ

### ビルド

- Cloud VM: `runCloudDevelopmentPreflight.sh` OK（MSBuild 未実行）

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（`・` 行） |
|--------|------|------|
| スロット突合 | 有効 | コンポーネント構成を json で作成/編集する流れを支援 |

### 次 tick 候補

- AssetPath プレビューは構築時1回（roadmap 607 行）

---

## セッション 2026-11-21 19:30（/loop 15m・roadmap tick）

### 完了タスク

- roadmap 607 行 / `GAP-Editor-EditorSceneRuntime-009`: **`EditorSceneRuntimeEditorAssetPathPreviewRelink`** で Initialize 後に再リンク、`SceneEntityEditor` プレビュー再構築後も再適用
- **`EntityEditorAuthoringEditorAssetPathEditNotify`**: CmpEditorAssetPath インスペクタ編集は `NotifyPreviewOnly` で即プレビュー再構築（不要な毎キーストローク保存を回避）

### 推測した用途（§6.4）

- **制作:** assetPath 変更のたびに Draw へ反映、手動「プレビューに適用」待ちを減らす
- **Authoring:** 一括ドロップ後の path 差し替えとプレビュー同期

### ビルド

- Cloud VM: `runCloudDevelopmentPreflight.sh` OK（MSBuild 未実行）

### 8. 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（`・` 行） |
|--------|------|------|
| AssetPath プレビュー再同期 | 有効 | json 上のアセット編集とエディタプレビュー確認の流れ |

### 次 tick 候補

- EnterScene が毎回ディスクから Load（roadmap 610 行）