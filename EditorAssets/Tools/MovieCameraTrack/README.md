# ムービーカメラトラック JSON

`EditorAssets/Camera/*.json` の `keyframes` を、ランタイムの `LoadMovieCameraTrackFromJson` と同様のルールでスモーク検証する。

## 検証

```bash
python3 verify_movie_camera_track_json.py
```

- 同梱 `sample_movie_track.json` が有効キーフレーム 2 件以上であること
- `EditorAssets/Camera/` 内のその他 `keyframes` 付き JSON も同条件
- 不正キーフレームのスキップ挙動をインラインテストで確認

CI: `.github/workflows/BuildCoverageCheck.yml`

設計: `EditorAssets/Tools/CursorAI/docs/feature-memos/movie_camera_controller.md`