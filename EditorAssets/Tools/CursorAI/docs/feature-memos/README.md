# 実装したい機能メモ（ロードマップ運用）

配置: **`EditorAssets/Tools/CursorAI/docs/feature-memos/`**（Cursor 半自律用。リポジトリ直下の `docs/` は使わない）。

正本は **`engineRoadmap.md`**（エンジン／ツール）と **`gameRoadmap.md`**（ゲーム本体）の2系統。箇条書きとチェックで進捗を管理する。

| ファイル | 用途 |
|----------|------|
| **`engineRoadmap_rough.md`** | エンジン下書き（テンプレ＋貼付欄） |
| **`engineRoadmap_rough.template.md`** | エンジン再生成用テンプレ |
| **`engineRoadmap_rough_old.md`** | エンジン・直近1回分の取り込み退避 |
| **`engineRoadmap.md`** | エンジン正本（`##` / `###` / `- [ ]`・`- [x]`） |
| **`gameRoadmap_rough.md`** | ゲーム下書き（テンプレ＋貼付欄） |
| **`gameRoadmap_rough.template.md`** | ゲーム再生成用テンプレ |
| **`gameRoadmap_rough_old.md`** | ゲーム・直近1回分の取り込み退避（初回取り込み前は無くてよい） |
| **`gameRoadmap.md`** | ゲーム正本 |
| **`entity_camera_components.md`** | エンティティカメラの要件・段階ロードマップ（チェック付き正本） |

旧ファイル名 `roadmap.md` / `roadmap_rough.md` は **`engineRoadmap*`** に改名済み。

## 書き方

**下書き** … `[大見出し]` / `〇中見出し` / `・小見出し`（本文行なし。大見出しは `<>` ではなく `[]`）

**正本（`engineRoadmap.md` / `gameRoadmap.md`）**

- `##` 大見出し、`###` 中見出し、`- [ ]` 小見出し（実装・チェック単位）
- 1行に足りないときは **その行の下にメモをインデント**するか、同じフォルダに spec md を追加し、行末に `（spec: …）` と書く（任意）

## 下書きから取り込む

対象がエンジンかゲームかで、対になる `*_rough.md` → `*.md` を使う（手順は同じ）。

1. `engineRoadmap_rough.md` または `gameRoadmap_rough.md` に `[]` / `〇` / `・` で貼る
2. 半自律開始時、エージェントが **下書きを解釈** → 対応する正本に起こす（§6.1：追加・細分化・統合）
3. 取り込み後: 先頭に `取り込み済み yyyy-mm-dd(ddd)_HH-MM-SS` を付けたうえで **`*_rough_old.md` に改名** → `*_rough.template.md` から **新しい `*_rough.md` を生成**

既存の正本と **マージ**する（完了 `[x]` は消さない）。

## 半自律での流れ

1. `task_backlog.md` があれば最優先
2. 対象系統の `*_rough.md` に貼付があれば **先に起こし** → 続いて対応する正本を Read
3. エンジン正本で未完了が無ければ **ゲーム正本**を同様に扱う（`cursorInstructions.md` §6）
4. **解釈**し、必要なら正本を **追加・細分化・統合**（理由を `progress_report.md` に）
5. 曖昧 → **ユーザーへ質問**。まだ曖昧 → **要件の再定義を要請**（実装しない）
6. 実装済みと判断した小見出しは **`[x]`** → **`progress_report.md`** に根拠
7. 見送りは **`- [-]`**（理由は `progress_report` §保留）
8. 明確になったら **`- [ ]` 1件** だけ実装 → `buildDevelopment.cmd` → `[x]`

手入れ案だけ確認: **「roadmap の手入れまで承認して」**（エンジン／ゲームのどちらかをチャットで指定）

`EditorAssets/Tools/CursorAI/task_backlog.md` は **優先指示用**（空なら正本の上から。どちらの正本かはバックログまたはチャットで指定）。

## チェック記号

| 記号 | 意味 |
|------|------|
| `- [ ]` | 未着手 |
| `- [~]` | 進行中（任意） |
| `- [x]` | 完了 |
| `- [-]` | 保留 |

## Google ドキュメント

下書き用に使い、**実装の正本は `engineRoadmap.md` / `gameRoadmap.md`** にコピーする（エージェントが読めるようにする）。