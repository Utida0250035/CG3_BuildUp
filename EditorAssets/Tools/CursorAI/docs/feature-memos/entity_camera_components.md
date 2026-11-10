# エンティティカメラ（CmpCamera / CmpDebugCamera）実装計画

**関連:** `odyssey_style_stage_camera.md`, `player_friendly_camera_policy.md`, `component_split_guidelines.md`

## 目的

- カメラを空エンティティ＋Upd Cmp で組み立て、エンジンがアクティブカメラを解決する。
- `DebugCamera` クラスは廃止し `CmpDebugCamera` に置換（段階実施）。
- 半自動カメラワークはアクティブカメラの Cmp のみに付与。

## 段階表

| Phase | 内容 | Done 目安 |
|-------|------|-----------|
| **P1** | feature memo、`CmpCamera`、`SceneCameraRig` 自動スポーン、既定プレハブ `MainCamera.json`、`ActiveCameraSystem`、`GetCamera()` 委譲、`GameOrder` 描画直前カメラ更新 | 本編シーンが従来どおり起動（挙動は P1 報告参照） |
| **P2** | エンジン複数カメラ登録・切替 API（同時マルチビューは P2b） | アクティブ index 切替が Engine 経由で動作 |
| **P3** | `CmpDebugCamera`、エディタ UI 契約維持、`SyncDebugCameraFromStageViewPose` を Cmp API 化 | `SceneEntityEditor` 相当の操作が可能 |
| **P4** | 半自動 Cmp（StageView / Blend / Focus / Movie）アクティブのみ | 既存 `SceneGame` / `SceneMovie` 経路が Cmp 経由 |
| **P5** | `DebugCamera` クラス削除、Release で Debug プレハブ無視＋不正参照検知 | Development のみデバッグ Cmp |
| **P6** | 全 `GetCamera()` Cmp 経由の掃除、複数画面（任意） | 残タスク整理 |

## 報告のお約束

- 各 Phase 終了時: 実施内容、**手動確認（実施／未実施）**、**未検証の有無**、次 Phase の提示。
- カメラ専用テストシーンは必須としない。

## P1 技術メモ

- `Scene::GetCamera()` → `SceneCameraRig` の primary `CmpCamera` が保持する `Camera`。
- DEVELOPMENT では P3 まで `CmpCamera` 内で暫定 `DebugCamera` サブクラスを使用（挙動維持）。
- 自動スポーン: `../EditorAssets/Entities/MainCamera.json` → 失敗時は `EntityFactory::MainCameraEntity()`。
- カメラ Update: シーン `Update` 完了後・`Draw` 直前（`Scene::PrepareCamerasForDraw`）。
