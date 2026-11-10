# 用途シナリオ正本（ギャップスキャン入力）

実装ギャップスキャンでは、**先に UseCase を 1 つ選び**、ここに列挙したシナリオごとに「その用途でできないこと」を洗い出す。

- **正本:** 本ファイル（シナリオ文言・ID）
- **不足の蓄積:** `implementation_gaps.md` / `implementation_gaps_index.json`
- **用途 ID:** `UC-Play` / `UC-Authoring` / `UC-Debug` / `UC-Ship`

## 書式

各シナリオは次の形（箇条書き 1 件）:

```markdown
- **{scenarioId}** — {一文でできるべきこと}（spec: `feature-memos/….md` 任意）
```

`scenarioId` 形式: `{UseCase短}-{トピック}-{NNN}`（例: `DBG-Streaming-001`）

## ローテーション（推奨 tick 順）

`UC-Play` → `UC-Authoring` → `UC-Debug` → `UC-Ship` → Area またはトピックを変えて繰り返す。

---

## UC-Play（ランタイム・ゲームプレイ）

- **PLY-SceneLoad-001** — プレイヤーがシーンを開き、主要オブジェクトとカメラが安定して動作する。
- **PLY-Audio-001** — プレイ中に BGM／SE が途切れず再生され、シーン遷移後も期待どおり鳴り続ける。
- **PLY-Camera-001** — プレイ中カメラが地形・オブジェクトを見失わず、操作に追従する（spec: `feature-memos/player_friendly_camera_policy.md`）。
- **PLY-Visual-001** — ランタイムでライト・フォグ等の見た目がデータどおり反映される（spec: `feature-memos/rim_light.md`, `feature-memos/volumetric_fog.md`）。
- **PLY-Movie-001** — シーン内ムービー／カットシーンが再生完了まで破綻しない（spec: `feature-memos/scene_movie.md`）。

---

## UC-Authoring（レベル／アセット制作）

- **AUT-EditorPlace-001** — 制作者がエディタ上でオブジェクトを配置・保存し、再度開いて同じ状態を得られる。
- **AUT-VisualTune-001** — 制作者がライト・カメラ・ポストのパラメータを編集し、プレビューで結果を確認できる。
- **AUT-Guidance-001** — 初回利用者がオペレータ案内に沿って主要操作を完了できる（spec: `feature-memos/operator_guidance_bubble.md`）。
- **AUT-AssetLoop-001** — 反復作業（複製・一括調整）がエディタ専用 UI で過度な手順にならない。
- **AUT-Component-001** — エンティティ／コンポント分割方針に沿ってアセットを組み立てられる（spec: `feature-memos/component_split_guidelines.md`）。

---

## UC-Debug（デバッグ・プロファイリング）

- **DBG-Streaming-001** — 開発者がストリーミング再生中のメモリ・進捗を確認し、枯渇や remaining 肥大を切り分けられる（spec: `feature-memos/streaming_debug_imgui.md`）。
- **DBG-PlayArea-001** — 開発者がエディタ内のテスト再生エリアでランタイム挙動を素早く再現できる（spec: `feature-memos/debug_test_play_area.md`）。
- **DBG-FeatureScene-001** — 開発者がエディタ既定の機能検証シーンから、エディタ外ランタイム機能だけを最小構成で試せる。
- **DBG-RenderDiag-001** — 開発者がシャドウ・ゴッドレイ等の描画結果をログ／オーバーレイで確認できる（spec: `feature-memos/directional_shadow.md`, `feature-memos/god_rays.md`）。
- **DBG-Profiler-001** — Development ビルドでフレームコストの当たりを付け、本番相当設定と切り分けられる。

---

## UC-Ship（ビルド・配布）

- **SHP-DevBuild-001** — `Development | x64` ビルドが CI／ローカル手順で再現可能に成功する（spec: `feature-memos/cloud_build_vcxproj_check.md`）。
- **SHP-Release-001** — Release 相当設定で不要なデバッグ UI・開発専用コードパスが出荷物に漏れない。
- **SHP-AssetPack-001** — 配布に必要なアセットがパイプラインから漏れず同梱される。
- **SHP-Config-001** — 本番と開発で意図したコンパイル／リンク差分のみが有効になる。
- **SHP-Smoke-001** — 最小シーンのスモーク起動が配布ビルドで通る（自動化未整備でも手順が文書化されている）。

---

## シナリオ追加ルール

1. 新シナリオは **ID を新規採番**（既存 ID の文言変更のみのときは ID を維持）。
2. ゲーム開発用途が推測できないシナリオは **追加しない** — ユーザー確認後に追記。
3. roadmap の小見出しや feature-memo が増えたら、該当 UseCase 配下にシナリオを足す。
