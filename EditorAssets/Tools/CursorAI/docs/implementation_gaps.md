# 実装ギャップ一覧（用途別・正本）

人間が読む正本。機械による重複判定・マージの正本は **`implementation_gaps_index.json`**。

<!-- gaps-schema: 2 -->

## メタ

- **schemaVersion:** 2
- **lastFullScanAt:** （未実施）
- **lastIncrementalScanAt:** （未実施）
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

（0 件のときは「なし」）

なし

---

## ギャップ一覧

（スキャン未実施。0 件）

---

## Archive

`addressed` / `wontfix` / `obsolete` / `merged_into` の要約をここへ（ID・1 行・日時）。

なし

---

## スキャン履歴（直近）

なし

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
