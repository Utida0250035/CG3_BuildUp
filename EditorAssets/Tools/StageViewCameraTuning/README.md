# StageViewCamera 調整ツール

オデッセイ寄りの戦闘カメラ（`StageViewCamera`）の eye / pivot を、エンジンなしで試す。

## 使い方

```bash
python3 preview_poses.py
python3 preview_poses.py --json ../../Camera/stage_view_camera_tuning.json
python3 preview_poses.py --focus 0 2 0 --focus-half 1.2 1.2
```

出力は `eye` / `pivot` の3成分。設計メモ: `EditorAssets/Tools/CursorAI/docs/feature-memos/odyssey_style_stage_camera.md`

## 次のステップ

- Development ビルドで JSON を読み込み ImGui からスライダー調整
- イージング式カスタム（roadmap 次行）と連携
