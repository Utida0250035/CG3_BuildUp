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

> `scene_movie.md` は GAP／roadmap リンク互換用スタブ（本文は本節）。
