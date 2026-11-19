# StageViewCamera 調整ツール

オデッセイ寄りの戦闘カメラ（`StageViewCamera`）の eye / pivot を、エンジンなしで試す。

## 使い方

```bash
python3 preview_poses.py
python3 preview_poses.py --json ../../Camera/stage_view_camera_tuning.json
python3 preview_poses.py --focus 0 2 0 --focus-half 1.2 1.2
```

出力は `eye` / `pivot` の3成分。設計メモ: `EditorAssets/Tools/CursorAI/docs/feature-memos/odyssey_style_stage_camera.md`

## ポリシー検証

```bash
python3 verify_camera_policy.py
```

## C++ 定数との同期

数値の正本は C++（`StageViewCamera.h` / `.cpp`・`StageBounds.h`・`Window.h`）。  
JSON は制作プレビュー用コピー。

```bash
python3 verify_stage_view_tuning_sync.py
python3 sync_stage_view_tuning_json.py --write   # C++ 変更後に JSON を更新
```

CI: `BuildCoverageCheck` が `verify_stage_view_tuning_sync.py` を実行。

## Development エディタ内プレビュー

エンティティエディタの **Stage camera tuning** ドック:

- `stage_view_camera_tuning.json` を読み込み eye / pivot を表示
- **JSON preview on Game view** で Game ビューの戦闘カメラに JSON を即反映
- **Auto hot-reload** でファイル保存を検知して再読込

パス定数: `EditorAssetsPaths::kStageViewCameraTuningJson`
