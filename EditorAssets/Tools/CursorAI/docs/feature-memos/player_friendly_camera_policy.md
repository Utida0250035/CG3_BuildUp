# カメラ操作をユーザーに丸投げしない（方針）

**関連:** `odyssey_style_stage_camera.md`、`stage_view_camera_easing.md`

## 原則

1. **ゲームプレイ中**（本編・タイトル・リザルト・BuildUp プレイ）はカメラ操作入力を受け付けない。
2. **視点は演出側が誘導** … `StageViewCamera` + 各種 `Camera*Controller` で姿勢を決定。
3. **開発時のみ** `DebugCamera` を許可するシーンを限定（エンティティ／パーティクルエディタのビューポート内）。
4. **Release** では `Camera` 基底のみ（`Scene::InitializeCamera`）で、フリーカメラ型はリンクしない。

## コード上のガード

| 機構 | 場所 | 役割 |
|------|------|------|
| `IsStageViewCameraLocked()` | 各 `Scene` | `true` で戦闘・本編カメラ固定 |
| `GameOrder` 更新ループ | `GameOrder.h` | ロック時は `camera->Update()` を呼ばず行列のみ更新 |
| `AllowDebugCameraControl()` | エディタ系 `Scene` | `false` ならデバッグカメラ入力なし |
| Release ビルド | `Scene.cpp` | `DebugCamera` 非使用 |

## シーン一覧（正本はソース）

| シーン | `IsStageViewCameraLocked` | 備考 |
|--------|---------------------------|------|
| `SceneGame` | `true` | イントロ／撃破演出はコントローラ |
| `SceneBuildUp` | `true` | JSON シーンプレビューもステージカメラ |
| `SceneTitle` / `SceneResult` | `true` | UI のみ、視点固定 |
| `SceneEntityEditor` | `false` | エディットビュー hover 時のみ Debug |
| `SceneParticleEditor` | `false` | 制作用自由視点 |

## 新シーン追加時チェックリスト

1. プレイヤーが操作する本番シーンか？ → **`IsStageViewCameraLocked() const override { return true; }`**
2. `EnterScene` / 演出完了後に **`SetupStageViewCamera()`** またはコントローラで姿勢を確定しているか？
3. UI テキストに「カメラを動かす」系の説明を入れていないか？
4. 検証: `EditorAssets/Tools/StageViewCameraTuning/verify_camera_policy.py`

## 意図しない例（避ける）

- 本編でマウスドラッグ／スティック右で視点変更
- 難易度やボス戦だけプレイヤーにズーム調整をさせる
- Release で DebugCamera を残す
