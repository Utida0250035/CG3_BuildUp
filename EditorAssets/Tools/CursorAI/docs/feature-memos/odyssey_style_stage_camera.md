# オデッセイ的ステージカメラ（設計メモ）

**正本（ランタイム）:** `StageViewCamera.cpp` / `StageViewCamera.h`  
**調整データ（制作）:** `EditorAssets/Camera/stage_view_camera_tuning.json`  
**プレビュー:** `EditorAssets/Tools/StageViewCameraTuning/preview_poses.py`

## 目的（ゲーム開発での用途）

- **ランタイム:** ステージ全体とボス／演出フォーカスが切れずに見える **固定寄りの戦闘カメラ**（プレイヤーにカメラ操作を要求しない）
- **制作:** 数値を JSON で触り、エンジン起動なしで eye / pivot を確認してから C++ 定数へ反映

## マリオ オデッセイから取る方針（要約）

1. **カメラは演出の一部** … プレイの主役は操作じゃなくステージ上の行動。視点はゲーム側が誘導する。
2. **斜め見下ろし＋横オフセット** … 真横一本ではなく、手前・やや上からパラメータ化された `StageViewCameraPose`。
3. **フレーミング優先** … `viewHalfHeight/Width` と FOV から必要距離を出し、ステージ AABB（`StageBounds`）にフィット。
4. **フォーカス時は戦闘より寄る** … `ComputeFocusViewPose` は距離スケール・高さ・横オフセットを戦闘より弱める（現行係数は tuning JSON に記載）。

## 既存コントローラとの関係

| 要素 | 役割 |
|------|------|
| `StageViewCameraPose` | eye + pivot |
| `CameraPoseBlendController` | 姿勢ブレンド（時間付き） |
| `CameraFocusPushController` | ボス撃破などのプッシュイン |
| `DebugCamera` | DEVELOPMENT の自由視点（**本番プレイ体験とは分離**） |

## 調整ツール化（本 tick）

- JSON に battle / focus / 演出用のスカラを集約
- Python で `ComputeStageBattleViewPose` 相当の数式を再現し CLI 出力
- **未実装（次）:** DEVELOPMENT 時に JSON ホットリード or ImGui スライダー → 即プレビュー

## C++ 定数との同期

`tuning.json` を変更したら、`StageViewCamera.h` の `kStageBattleView*` 等へ手動反映する（自動同期は別タスク）。プレビュー出力とゲーム内がずれたら JSON か定数のどちらが古いか確認する。

## プレイヤー操作の禁止

本編ではカメラ操作を要求しない。実装ガードは `player_friendly_camera_policy.md` と `IsStageViewCameraLocked`。
