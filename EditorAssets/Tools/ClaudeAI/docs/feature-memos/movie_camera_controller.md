# ムービー用カメラコントローラ

**クラス:** `MovieCameraController`  
**トラック:** `EditorAssets/Camera/*.json`（例: `sample_movie_track.json`）

## 用途

- **ランタイム:** カットシーン・ムービーシーンでキーフレーム再生（プレイヤー操作なし）
- **制作:** JSON で eye/pivot と時刻を編集（次: ムービー用シーンが `TryBeginFromJsonFile` で接続）

## API

```cpp
MovieCameraController movie;
movie.SetCamera(scene->GetCamera());
movie.TryBeginFromJsonFile("../EditorAssets/Camera/sample_movie_track.json");
// Update ループで movie.Update(); IsFinished() で終了判定
```

## JSON

| フィールド | 内容 |
|-----------|------|
| `time` | 秒（再生開始から） |
| `eye` / `pivot` | `Vector3` 配列 |
| `easeToNext` | 次キーフレームまでの補間（`PullBackReveal` / `FocusPush` / `LegacyInOutQuad`） |

不正なキーフレーム（`eye`/`pivot` 欠落・型不一致など）は **1 件ごとスキップ** し、有効なキーフレームが 2 件以上あれば再生する（`MovieCameraTrackKeyframeParsePolicy`）。

## 検証

```bash
python3 EditorAssets/Tools/MovieCameraTrack/verify_movie_camera_track_json.py
```

CI: `BuildCoverageCheck`（`sample_movie_track.json` および `EditorAssets/Camera/` の `keyframes` 付き JSON）。

## SceneMovie

```cpp
Scene *movie = SceneMovieRegistry::Instance().Launch(
    "../EditorAssets/Camera/sample_movie_track.json", returnScene);
CommandChangeScene::Instance()->Set(movie, std::make_unique<ChangeSceneFade>().release());
```

- `Escape` でスキップ（returnScene へフェード）
- トラック終了で自動遷移

---

## SceneMovie（ムービー用シーン）

**クラス:** `SceneMovie` / `SceneMovieRegistry`

### 用途

- **ランタイム:** 短いカットシーンを単独シーンとして再生し、完了後に呼び出し元へ戻る
- **制作:** トラック JSON を差し替えてリピート確認（スカイドームのみの簡易 3D 背景）

### ポリシー

- `IsStageViewCameraLocked() == true`
- カメラは `MovieCameraController` のみ（失敗時は即 return へ）
- オーディオ: `SceneMovieAudioPolicy`（Enter でループ BGM 無し、復帰フェード前に returnScene 向け遅延 BGM 登録）

> `scene_movie.md` は GAP／roadmap リンク互換用スタブ（本文は本節）。
