# エンティティカメラ（要件・段階ロードマップ）

**正本（本ファイル）** — 他エージェントは `@entity_camera_components.md` で参照。  
**作業ブランチ:** `cursor/cmp-camera-entity-0f04`（`develop` 起点）

**関連（挙動・ポリシーはこちらに分散、重複記述しない）:**

| 文書 | 内容 |
|------|------|
| `odyssey_style_stage_camera.md` | StageView 数式・tuning JSON |
| `player_friendly_camera_policy.md` | 本編でカメラ操作を要求しない |
| `stage_view_camera_easing.md` | ブレンド／フォーカスイージング |
| `movie_camera_controller.md` | ムービートラック・SceneMovie |
| `component_split_guidelines.md` | Cmp 追加手順 |

**チェック規則:** `- [ ]` 未着手 / `- [~]` 進行中 / `- [x]` 完了

---

## 1. 目的

- カメラを **空エンティティ + Upd Cmp** で組み立てる（`CmpCamera` / `CmpDebugCamera` が `Camera` を **ラップ**）。
- **`DebugCamera` クラスは廃止**し、同等機能を `CmpDebugCamera` に移す（Development のみ）。
- **半自動カメラワーク**（StageView / Blend / Focus / Movie 等）は **アクティブカメラの Cmp のみ**に付与。
- **エンジン**がアクティブカメラを解決し、将来は **複数カメラ切替・複数画面描画**に拡張。

## 2. 合意した非機能要件

- [x] **複数カメラ:** 複数カメラエンティティ、1/2/3 切替・複数画面（同時マルチビューは段階 P2b で切り分け可）。
- [x] **解決:** アクティブカメラは **エンジン側**。
- [x] **エディタ:** `CmpDebugCamera` + **既存 UI 契約**（`SceneEntityEditor` 相当）。
- [x] **Sync:** `SyncDebugCameraFromStageViewPose` 相当は **`CmpDebugCamera` API**。**アクティブ 1 つだけ**同期。
- [x] **Release:** `CmpDebugCamera` はビルドから除外。プレハブに Debug 体があっても無視。不正参照はマクロ分岐 + Release で `static_assert(false)`。
- [x] **スポーン:** シーン起動時 **自動スポーン**（既定プレハブ `EditorAssets/Entities/MainCamera.json`）。
- [x] **更新順:** シーン `Update`（＋遷移 Update）**完了後**、**Draw 直前**にカメラ本体の Update（行列等）。
- [x] **GetCamera:** 最終的に **すべて Cmp 経由**（移行期は `Scene::GetCamera()` が primary へ委譲で可）。
- [x] **Done 条件:** 専用カメラテストシーンは **必須にしない**。段階終了時に **progress_report へ報告**（**未実施・未検証も書く**）。

## 3. 段階ロードマップ

### P0 — ドキュメント（本 tick）

- [x] 本ファイル作成（要件・段階・チェック）
- [x] `roadmap.md` の Camera 節から本 spec へリンク

### P1 — CmpCamera・自動スポーン・単一アクティブ

- [ ] `CmpCamera`（`Camera` 所有。DEVELOPMENT 暫定で内部 `DebugCamera` 可）
- [ ] `SceneCameraRig` + 既定 `MainCamera.json` 自動スポーン
- [ ] `ActiveCameraSystem` + `GameOrder` 描画直前 Prepare
- [ ] `Scene::GetCamera()` → primary `CmpCamera` 委譲
- [ ] エディタ登録（`EntityEditorDefinition` / `EditorSceneRuntime` / `kKnownComponents`）
- [ ] vcxproj 登録
- [ ] **報告:** 手動確認（実施 / 未実施）、未検証の有無

### P2 — エンジン複数カメラ（切替）

- [ ] エンジン: カメラ登録・**アクティブ index 切替**
- [ ] primary / viewport0 の契約明文化
- [ ] **報告**（同上）

### P2b — 同時マルチビュー（任意・後段）

- [ ] ビューポート／複数 Pass（スコープは着手前に再確認）
- [ ] **報告**（同上）

### P3 — CmpDebugCamera・UI 契約

- [ ] `CmpDebugCamera` 実装（Development のみビルド）
- [ ] `SceneEntityEditor` 相当 UI を Cmp 経由に移植
- [ ] `SyncFromStageViewPose`（アクティブ 1 つ）
- [ ] **報告**（同上）

### P4 — 半自動 Cmp（アクティブのみ）

- [ ] StageView / Blend / FocusPush / Movie 等を Cmp 化（シーン直結コントローラ縮小）
- [ ] **報告**（同上）

### P5 — DebugCamera クラス削除

- [ ] `ForDebug/DebugCamera` 削除・参照掃除
- [ ] Release 不正参照ガード
- [ ] **報告**（同上）

### P6 — 掃除・複数画面仕上げ

- [ ] 残存 `GetCamera()` 直呼びの Cmp 経路化
- [ ] P2b 未了なら計画更新
- [ ] **全段階完了報告**

## 4. 報告のお約束（progress_report）

各 Phase 終了時に追記:

1. Phase 番号・実施内容
2. **手動確認:** 実施したシーン／操作、または **未実施**
3. **段階末・未検証の有無**
4. 次 Phase の 1 行提示

## 5. 推測した用途（ゲーム開発）

- **ランタイム:** 主カメラ・デバッグカメラをエンティティ化し、複数視点・半自動演出を同じワークフローで試す。
- **制作:** `MainCamera` プレハブ＋エディタ Cmp 追加。Release では本編カメラのみ。
- **デバッグ:** Development で `CmpDebugCamera`、本編は `IsStageViewCameraLocked` 等と連動。
