# ムービー用シーン（SceneMovie）

**クラス:** `SceneMovie` / `SceneMovieRegistry`  
**依存:** `MovieCameraController`、`EditorAssets/Camera/*.json`

## 用途

- **ランタイム:** 短いカットシーンを単独シーンとして再生し、完了後に呼び出し元へ戻る
- **制作:** トラック JSON を差し替えてリピート確認（スカイドームのみの簡易3D背景）

## ポリシー

- `IsStageViewCameraLocked() == true`
- カメラは `MovieCameraController` のみ（失敗時は即 return へ）

## 起動例

`movie_camera_controller.md` 参照。
