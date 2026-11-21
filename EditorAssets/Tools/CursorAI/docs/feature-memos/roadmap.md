# 実装ロードマップ（箇条書き）

正本。下書きは `roadmap_rough.md` から起こす（`2026-11-08` 取り込み済み、`2026-11-18` 再取り込み）。

**チェック規則:** `- [ ]` 未着手 / `- [~]` 進行中 / `- [x]` 完了 / `- [-]` 保留（`progress_report` に理由）

**ギャップ在庫:** `../implementation_gaps.md`（2026-11-11 時点・未解消 289。P0/P1 は各節「ギャップスキャン追補」、P2/P3 は在庫行＋正本参照）


---

## Audio

### Bgmのためのストリーミング再生
- [x] ストリーミング BGM（`AudioManager::SetupStreaming` / `PlaySceneBgm`）

### 再生時にピッチなどのステータス調整を行なう
- [x] ピッチ等ステータス API（BGM／SE）（`SetBgmLoopPitch` / `SetSeVolume` 他）

### Bgmの一時停止や再開
- [x] シーン BGM の一時停止・再開（`PauseBgmLoop` / `ResumeBgmLoop`）
- [x] 再生地点の記録／指定（`GetBgmLoopPlaybackSeconds` / `SetBgmLoopPlaybackSeconds`、ストリーミング／短尺）



### ギャップスキャン追補（P0/P1）
- [x] decoderBacklog01 のクランプで remaining 肥大が見えない（gap: `GAP-Audio-StreamingDebug-001`・Debug・P1）

### ギャップスキャン追補（P2/P3・在庫）
- 展開済み: シーン遷移時の BGM／SE 継続・クロスフェード・遷移 SE（CommandScene / ChangeSceneFade 系。在庫 45件）
- 展開済み: AudioManager ストリーミング／デコーダ／ボイスコールバック境界（在庫内の AudioManager 中心）
  - 正本: `../implementation_gaps.md` / `implementation_gaps_index.json`（Area=Audio）


### ChangeSceneFade
- [x] フェードイン開始時に遷移 SE が未再生（フラグのみ立つ）（gap: `GAP-Scene-ChangeSceneFade-001`）
- [x] timeRatio 満了で即 isSceneChange となり中間で EnterScene（gap: `GAP-Scene-ChangeSceneFade-002`）
- [x] フェード中盤で次 BGM が始まる（gap: `GAP-Scene-ChangeSceneFade-003`）

### PauseBgmLoop
- [x] Pause/Resume/Volume 付き BGM API がシーンから未使用（gap: `GAP-Audio-PauseBgmLoop-001`）

### StopBgmLoop
- [x] StopBgmLoop がシーン BGM 以外のストリーミングも停止（gap: `GAP-Audio-StopBgmLoop-001`）

### PlaySceneBgm
- [x] PlaySceneBgm 開始時に全ボイスを一括停止（gap: `GAP-Audio-PlaySceneBgm-001`）
- [x] Init 失敗で voice playId だけ巻き戻し（gap: `GAP-Audio-PlaySceneBgm-002`）

### PlayBgmLoop
- [x] PlayBgmLoop 失敗時に PlaySceneBgm を再実行（gap: `GAP-Audio-PlayBgmLoop-001`）

### ResumeBgmLoop
- [x] ResumeBgmLoop がシーンから呼ばれない（gap: `GAP-Audio-ResumeBgmLoop-001`、§6.2 コード確認）

### SetBgmLoopVolume
- [x] SetBgmLoopVolume が未使用（gap: `GAP-Audio-SetBgmLoopVolume-001`、§6.2 コード確認）

### PlaySeLoop
- [x] PlaySeLoop がコードベースで未呼び出し（gap: `GAP-Audio-PlaySeLoop-001`）

### AudioManager
- [x] 空き無しで全汎用ストリームをリセット（gap: `GAP-Audio-AudioManager-003`）
- [x] BGM 一時停止中は復帰判定が誤る（gap: `GAP-Audio-AudioManager-001`）
- [x] Resume は Start のみでデコード再開なし（gap: `GAP-Audio-AudioManager-004`）
- [x] StopAll は BGM 系ループのみ（gap: `GAP-Audio-AudioManager-002`）
- [x] Paused ストリームは Update をスキップ（gap: `GAP-Audio-AudioManager-005`）
- [x] Handle と voice の playId がずれる（gap: `GAP-Audio-AudioManager-007`）
- [x] Start 後に Refill を 1 回だけ（gap: `GAP-Audio-AudioManager-009`）
- [x] BGM フォールバックで短音ボイス解放（gap: `GAP-Audio-AudioManager-008`）
- [x] ストリーム BGM 停止は XAudio Stop（gap: `GAP-Audio-AudioManager-010`）
- [x] Resume は Start 後 Refill なし（gap: §6.2・`PrefillStreamingQueueAfterStart`）
- [x] 再生位置計算が 44100 固定
- [x] Waiting 非ループは Stop で終了
- [x] ボイス枯渇で assert
- [x] Init 失敗で playId 不一致
- [x] ストリーム PCM を 44.1k 固定
- [x] SetupStreaming が 44.1kHz 固定（gap: `GAP-Audio-AudioManager-011`）
- [x] Load ヒットで再判定なし（gap: `GAP-Audio-AudioManager-017`）
- [x] 不足バイトを 0 でパディング（gap: `GAP-Audio-AudioManager-015`）

### PlaySe
- [x] PlaySe が Play の Handle を捨てる（gap: `GAP-Audio-PlaySe-001`）

### StreamingVoiceCallback
- [x] OnStreamEnd が Waiting のみセット（gap: `GAP-Audio-StreamingVoiceCallback-001`）
- [x] OnBufferEnd が Streaming へ遷移（gap: `GAP-Audio-StreamingVoiceCallback-002`）

### AudioDecoder
- [x] EOF で break のみ（gap: `GAP-Audio-AudioDecoder-002`）
- [x] 4 バイト揃えで端数を次回へ（gap: `GAP-Audio-AudioDecoder-001`）

### IsBgmLoopPaused
- [x] IsBgmLoopPaused がシーンから未使用（gap: `GAP-Audio-IsBgmLoopPaused-001`）

### IsSuitableStreaming
- [x] ファイルサイズだけでストリーム判定（gap: `GAP-Audio-IsSuitableStreaming-001`）
- [x] 閾値 5MB がハードコード（gap: `GAP-Audio-IsSuitableStreaming-002`）

### GetBgmLoopPlaybackSeconds
- [x] 再生秒取得がゲームシーン未使用（gap: `GAP-Audio-GetBgmLoopPlaybackSeconds-001`）

### StopSe
- [x] ループ SE ハンドルの個別停止が無い（gap: `GAP-Audio-StopSe-001`）

### PlaySeDetail
- [x] キャッシュ命中で再 Load しない（gap: `GAP-Audio-PlaySeDetail-002`）
- [x] static cache がアンロードされない（gap: `GAP-Audio-PlaySeDetail-001`）

### SetBgmLoopPlaybackSeconds
- [x] 再生位置指定がゲームシーン未使用（gap: `GAP-Audio-SetBgmLoopPlaybackSeconds-001`）

### SetBgmLoopPitch
- [x] SetBgmLoopPitch がシーンから未呼び出し（gap: `GAP-Audio-SetBgmLoopPitch-001`）

### SetSeVolume
- [x] SetSeVolume がゲームロジックから未使用（gap: `GAP-Audio-SetSeVolume-001`）

### GetSeVolume
- [x] GetSeVolume がシーンから未呼び出し（gap: `GAP-Audio-GetSeVolume-001`）

### GetSeFrequencyRatio
- [x] GetSeFrequencyRatio が未使用（gap: `GAP-Audio-GetSeFrequencyRatio-001`）

### SetSeFrequencyRatio
- [x] SetSeFrequencyRatio がゲームから未使用（gap: `GAP-Audio-SetSeFrequencyRatio-001`）

## System

### コンポーネント指向
- [x] ImGui でエンティティへ Component 追加／削除（`EntityEditorUi.cpp`）
- [x] Unity っぽいオブジェクト指向の整理（方針メモ）（spec: `unity_style_entity_guidelines.md`）
- [-] 純粋 ECS（今はやらない・スコープ外）

### プレハブ機能
- [x] 設定済みエンティティを json 出力（`SaveSelectedInstanceToPrefabPath`／プロジェクトパネル）
- [x] json からエンティティ生成／設定（`AddInstanceFromPrefabPath`）

### 指定エンティティを作成／放出するエンティティ（factory 属性コンポーネント）
- [-] 依存: プレハブ／json 安定後

### エンティティ構成のアセット化
- [-] プレハブ機能と統合予定（重複）

### エンティティのjson出力
- [x] シーン／エンティティ定義の json 入出力（`EditorSceneDocument` 等）
- [x] コンポーネント型を文字列／選択式で記録（JSON `typeName`＋ImGui Combo）
- [-] FNV-1a ハッシュ registry（設計確定後）

### Colliderコンポーネント
- [x] 3次元球（`CmpHitSphere`）
- [-] AABB（判定順・Update 連動の設計確定後）
- [-] Update 順に追随する全体判定（アーキテクチャ未確定）

### 「何にでも使える」1つより、「これにだけ使える」を大量に
- [x] 方針に沿ったコンポーネント分割（継続）（spec: `component_split_guidelines.md`・ガイド第1版）

### SortComponents()関数
- [x] `AddComponent` 内処理の関数化（`AttachUpdComponent` / `AppendKnownComponentSlot`）
- [x] priority 操作後 `SortComponents()` で実行順変更（`SetUpdCmpPriority` / `SetDrawCmpPriority`）

### 任意フォルダから開発用GUIへドラッグ&ドロップ…
- [x] GUI 専用クラスで Assets へコピー（`EditorImportedAsset`・`EditorAssets/Imported`）
- [x] リソースコンポーネントがパス記録・読み込み（`CmpEditorAssetPath`・obj／png プレビュー）

### Entityの配列化
- [x] エディタで実行順を操作（`executionOrder`・プレビュー反映）
- [x] priority 入力または D&D（`InputInt` による priority。D&D は未）
- [x] 実行順の列表示（必須）（インスペクタ一覧 `順 | 型`）

### Jsonファイルの入出力
- [x] エディタ／シーン json（既存実装）


### ギャップスキャン追補（P2/P3・在庫）
- 展開済み: Entity／Runtime まわり残件（在庫 3件）
  - 正本: `../implementation_gaps.md`


### UiState
- [x] 複数選択が無く一括調整できない（gap: `GAP-Editor-UiState-001`）

### component_split_next
- [x] ランタイム用 Data／Collider 分割候補のエディタ登録が未着手（gap: `GAP-Authoring-component_split_next-001`）

### component_split_verify
- [x] コンポーネント登録の三箇所同期を検証する自動化が無い（gap: `GAP-Infra-component_split_verify-001`）

### CmpCamera
- [x] Initialize が Orthographic を既定生成（gap: `GAP-Entity-CmpCamera-001`）

### CmpLhRigidBody
- [x] 物理演算が基本機能として独立せず、剛体コンポーネントの Update に閉じている（gap: `GAP-Runtime-CmpLhRigidBody-001`）

### EngineAccess
- [x] 描画への外部アクセスが EngineAccess に無く、AtrumEngine 経由になる（gap: `GAP-Engine-EngineAccess-001`）

### CmpBounceBall
- [x] ボールの運動・壁床バウンス・衝突演出が1クラスに同居している（gap: `GAP-Runtime-CmpBounceBall-001`）

## Loader

### ImGuiウィンドウにファイルをドロップしてobjやpngなどの読み込み
- [x] ドロップでパス保持・読み込み（SDL ドロップ→Imported・png ビルボードプレビュー）

### 別スレッドでのリソース読み込み
- [-] 安定化後（並行読み込みは影響大）

### Binaryやddsへのファイル変換ツール
- [x] BinaryConverter（`EditorAssets/Tools/BinaryConverter`・`.amesh` v1）
- [x] DdsConverter（`EditorAssets/Tools/DdsConverter`・RGBA8 DDS）


### BinaryConverter
- [x] メッシュ変換パイプラインのバッチ／CI フックが未整備（spec: feature-memos/atr_mesh_binary_format.md）（gap: `GAP-Infra-BinaryConverter-001`）

## Math

### Slerp(球面線形補間)(Quaternion)
- [x] `Quaternion::Slerp` / `Vector3::Slerp` 実装あり

## Organize

### 名前空間の作成と整理
- [-] 範囲が広い・別マイルストーンで計画

## Camera

### オデッセイ的カメラ設計
- [x] 設計＋調整ツール化（`odyssey_style_stage_camera.md`・`stage_view_camera_tuning.json`・`StageViewCameraTuning/`）
- [x] イージング式のカスタム（`StageViewCameraEasing.h`・blend/focus 接続）
- [x] ユーザーに操作を丸投げしない（`player_friendly_camera_policy.md`・検証スクリプト・Scene 注記）

### エンティティカメラ（CmpCamera / CmpDebugCamera）
- [~] 段階実装（正本・チェックリスト: **spec: `entity_camera_components.md`**）


### ギャップスキャン追補（P0/P1）
- [x] StageView カメラが StageBounds 定数のみ参照しシーン毎の範囲に追従しない（gap: `GAP-Camera-StageBounds-001`・Play・P1）

### ギャップスキャン追補（P2/P3・在庫）
- 展開済み: tuning.json と StageViewCamera 定数の自動同期（在庫 1件）
  - 正本: `../implementation_gaps.md`


### StageViewCameraTuning
- [x] カメラ tuning JSON のエディタ内プレビュー／ホットリロードが未実装（spec: feature-memos/odyssey_style_stage_camera.md）

### stage_view_camera_tuning
- [x] tuning.json と StageViewCamera 定数の自動同期が無い（gap: `GAP-Infra-stage_view_camera_tuning-001`）
- [x] tuning JSON の client 解像度が参照されない（spec: feature-memos/odyssey_style_stage_camera.md）

### StageViewCamera
- [x] ステージカメラのアスペクトが既定解像度固定

### verify_camera_policy
- [x] カメラ方針静的検証が CI／ビルド手順に未組み込み
- [x] verify_camera_policy が AllowDebugCameraControl を検査しない

## Scene

### ムービー用カメラコントローラ
- [x] 実装（`MovieCameraController`・JSON トラック・`sample_movie_track.json`）

### ムービー用シーン
- [x] 実装（`SceneMovie`・Registry・sample トラック連携）


### ギャップスキャン追補（P0/P1）
- [x] ムービーシーン起動がゲームプレイ経路に未接続（gap: `GAP-Scene-SceneMovieRegistry-001`・Play・P1）
- [x] 最小ランタイム検証シーン起動がエディタから未配線（gap: `GAP-Scene-SceneBuildUpRegistry-001`・Debug・P1）
- [x] returnScene が null だとムービー終了／失敗後もシーンが抜けられない（gap: `GAP-Scene-SceneMovie-004`・Play・P1）
- [x] 遷移演出中はシーン Update が全面停止（gap: `GAP-Scene-GameOrder-001`・Play・P1）
- [x] FindPrimaryCmpCamera が rig 配列のみ走査（gap: `GAP-Scene-SceneCameraRig-001`・Debug・P1）
- [x] entities_ と GetCamera の cameraRig が未接続（gap: `GAP-Scene-SceneBuildUp-009`・Debug・P1）

### ギャップスキャン追補（P2/P3・在庫）
- 展開済み: SceneBuildUp 検証経路・レジストリ・cameraRig 接続の細部（在庫 65件）
- [x] CommandScene 遷移要求・Development 起動シーン不一致
  - 正本: `../implementation_gaps.md`（Area=Scene）


### SceneMovie
- [x] movie トラックパスを EditorAssets 基準に解決していない（spec: feature-memos/movie_camera_controller.md）
- [x] トラック読込失敗時に初回 Update で即戻り（エラー表示なし）
- [x] SkyDome がトラック先頭姿勢前のカメラに紐づく（spec: feature-memos/scene_movie.md）
- [x] トラック開始失敗時に SetupStageViewCamera を呼ぶ（spec: feature-memos/scene_movie.md）
- [x] ムービーシーンに BGM/トラック音声の再生・復帰ポリシーがコード上見えない（spec: feature-memos/scene_movie.md）
- [x] 遷移予約後はカメラ／SkyDome 更新が停止（spec: feature-memos/scene_movie.md）
- [x] SceneMovie のライティングにリム・ボリュームフォグが無い（spec: feature-memos/rim_light.md）

### GameOrder
- [x] フレームループが音声を管理しない

### SceneGame
- [x] 見た目パラメータがデータではなく SetupGameLighting 直書き（spec: feature-memos/volumetric_fog.md）
- [x] OnBossDefeated で BGM を演出完了前に停止
- [x] シーン入場時の operator_guidance 読込失敗を検知・代替しない
- [x] EnterScene が本編見た目プリセットを適用しない（spec: feature-memos/rim_light.md）
- [x] SpawnEntities がイントロカメラ設定より先に実行される
- [x] 撃破演出中は MaintainSceneBgmIfNeeded が無効
- [x] OnPlayerDefeated は BGM を止めず撃破は即停止
- [x] イントロ中の Update 早期 return で弾幕 SE 無音
- [x] イントロ中に Maintain が PlaySceneBgm
- [x] pendingDefeat 中は Maintain が return
- [x] フェード中も Exit で StopBgmLoop
- [x] 遷移演出中は MaintainSceneBgm を呼ばない

### CommandScene
- [x] Development 起動の初期シーンがプレイ用シーンロード経路と一致しない
- [x] シーン切替で BGM が瞬断し、クロスフェード／継続再生ポリシーが無い

### CommandChangeScene
- [x] 遷移実行中の Set は無視され次シーン要求が失われる

### SceneBuildUp
- [x] BuildUp 描画に地形越し自機シルエットが無い（spec: feature-memos/player_terrain_silhouette.md）
- [x] BuildUp ランタイム描画にフォグ・リム・ゴッドレイ・シャドウが無い
- [x] BuildUp の sceneJsonPath_ を基準パス解決しない
- [x] SceneBuildUp のカメラがシーン JSON の境界情報を読まない
- [x] シーン JSON 読込失敗時にデフォルト 1 体へ置換しプレイヤーに通知しない
- [x] SceneBuildUp に BGM／シーン SE の入退場が無い

### SceneTitle
- [x] Title の BGM 途切れを検知・再開する処理が無い
- [x] SceneTitle のライティングにリム・フォグ・ゴッドレイが無い（spec: feature-memos/rim_light.md）
- [x] ExitScene がフェード完了前に StopBgmLoop
- [x] Submit 直後にフェード遷移

### SceneResult
- [x] SceneResult に BGM 維持・再開処理が無い
- [x] SceneResult が毎フレーム簡易平行光のみ適用（spec: feature-memos/rim_light.md）
- [x] ExitScene が即 StopBgmLoop

### Scene
- [x] AllowDebugCameraControl が既定 true でガードが二重依存（spec: feature-memos/player_friendly_camera_policy.md）
- [x] 新規シーンのステージカメラロックはドキュメント依存（spec: feature-memos/player_friendly_camera_policy.md）

### SceneParticleEditor
- [x] SceneParticleEditor が AllowDebugCameraControl を限定していない（spec: feature-memos/player_friendly_camera_policy.md）

### MovieCameraTrackLoader
- [x] キーフレーム 1 件の不正でトラック全体を拒否（spec: feature-memos/movie_camera_controller.md）
- [x] ムービートラック JSON のスモーク検証が自動化されていない（spec: feature-memos/movie_camera_controller.md）

## Render

### オブジェクトの粒々透過
- [x] 実装（`DITHERED_ALPHA`・Bayer clip PS・`CmpMesh`）

### 自機のシルエット透視
- [x] 地形越しシルエット表示（`OCCLUDED_SILHOUETTE`・`PlayerTerrainSilhouette`）


### ギャップスキャン追補（P0/P1）
- [x] Lighting::ImGui（フォグ／ゴッドレイ／影）が未配線（gap: `GAP-Render-Lighting-001`・Debug・P1）
- [x] DrawScene 毎に見た目リテラルを再適用しデータ反映が上書きされる（gap: `GAP-Render-SceneGame-002`・Play・P1）
- [x] Visual::Lighting と AtrumEngine の VolumetricFog が非共有（gap: `GAP-Render-Lighting-002`・Play・P1）
- [x] SetupGameLighting が Fog 設定後に Flush しない（gap: `GAP-Render-SceneGame-003`・Play・P1）

### ギャップスキャン追補（P2/P3・在庫）
- 展開済み: SceneGame 描画／ライティング反映の残件（在庫 6件）
  - 正本: `../implementation_gaps.md`（Area=Render）

## Motion

### モーションブレンディング
- [x] 実装（`MotionBlender`・`CmpMotionBlend`・sample JSON）

### モーション間の補間
- [x] 実装（`MotionCrossfade`・`CmpMotionCrossfade`）

## Debug

### デバッグ用テストプレイエリア
- [x] 生成ツール／テンプレート（`DebugPlayArea/`・`scaffold_play_area.py`）

### ストリーミング再生のデバッグ(ImGui)
- [x] メモリ使用量表示（`AudioStreamingDebugImGui`・`CollectStreamingMemoryDebug`）
- [x] ストリーミング進捗表示（再生秒・キュー／デコーダ progress bar）


### ギャップスキャン追補（P0/P1）
- [x] 描画診断用ライティングパネルが EditorFrame に無い（gap: `GAP-Engine-AtrumEngineLighting-001`・Debug・P1）
- [x] 実行ビューがプレビュー Entity のみで機能単体試験にならない（gap: `GAP-Debug-SceneViewTab-001`・Debug・P1）

### ギャップスキャン追補（P2/P3・在庫）
- 展開済み: フレーム時間オーバーレイ・ストリーミングデバッグメニュー導線（在庫 2件）
  - 正本: `../implementation_gaps.md`（Area=Debug）


### SceneBuildUpRegistry
- [x] Launch は生成してポインタ返却のみ（spec: feature-memos/debug_test_play_area.md）
- [x] Launch は Scene 生成のみ
- [x] Launch が同一パス重複を防がない

### StreamingDebug
- [x] ストリーミング診断が ImGui 表示のみでログに残らない（spec: feature-memos/streaming_debug_imgui.md）

### EntityEditorUi
- [x] Release 起動パスを表示するだけ（spec: feature-memos/debug_test_play_area.md）

### EditorScenePlayRequest
- [x] EditorScenePlayRequest::Path が参照されない（spec: feature-memos/debug_test_play_area.md）
- [x] EditorScenePlayRequest::Clear が呼ばれない（spec: feature-memos/debug_test_play_area.md）
- [x] Set 後に BuildUp 起動が無い（spec: feature-memos/debug_test_play_area.md）

### SceneEntityEditor
- [x] シーン実行が本番相当 SceneBuildUp 経路にならない
- [x] F2 はパーティクル遷移のみで機能検証シーン未割当（spec: feature-memos/debug_test_play_area.md）
- [x] シーン実行後は Runtime タブフォーカスのみ（spec: feature-memos/debug_test_play_area.md）
- [x] SetupPreviewCamera が GetCamera のみ更新（spec: feature-memos/entity_camera_components.md）
- [x] Runtime タブで SetupStageViewCamera に差し替え（spec: feature-memos/entity_camera_components.md）
- [x] 実行は Runtime タブ切替のみ
- [x] Edit ホバー時に Draw 内で camera->Update
- [x] シーンビュー RT が GameOrder 後に SetCamera
- [x] DrawPreview が親 root のみ
- [x] Exit はプレビュー解放のみ

### FrameDeltaTime
- [x] フレーム時間のオーバーレイ／ヒストグラムが無い

### StreamingMenu
- [x] ストリーミングデバッグ ON が SceneBuildUp 経路で開けない（spec: feature-memos/streaming_debug_imgui.md）

### scaffold_play_area
- [x] テストエリア新規作成が scaffold スクリプト手動実行のみ
- [x] 生成後は Launch のみ案内（spec: feature-memos/debug_test_play_area.md）
- [x] 生成は sceneName 差替のみ

### CommandScene
- [x] 起動時シーンが SceneEntityEditor に固定（spec: feature-memos/debug_test_play_area.md）
- [x] startup 空なら BuildUp ではなく Title（spec: feature-memos/debug_test_play_area.md）
- [x] Launch 前に TryLoad を呼ばない
- [x] sceneNow 確定後 Enter を無条件

### SceneBuildUp
- [x] returnScene が null だと機能検証シーンから抜けられない（spec: feature-memos/debug_test_play_area.md）
- [x] SceneBuildUp::Draw が EditorFrame ワークベンチを常時描画（spec: feature-memos/debug_test_play_area.md）
- [x] SceneBuildUp が EditorSceneRuntime を参照しない（spec: feature-memos/debug_test_play_area.md）
- [x] ロード失敗時に空の Entity_0 を生成（spec: feature-memos/debug_test_play_area.md）
- [x] cameraRig 実体は Update されない（spec: feature-memos/entity_camera_components.md）
- [x] ロード直後 SetupStageViewCamera を呼ぶ（spec: feature-memos/entity_camera_components.md）
- [x] DrawRuntimeViewContent が Prepare 後に SetupStageViewCamera（spec: feature-memos/entity_camera_components.md）
- [x] IsStageViewCameraLocked が常時 true（spec: feature-memos/debug_test_play_area.md）
- [x] 遷移演出中 F1 復帰が処理されない（spec: feature-memos/debug_test_play_area.md）
- [x] BuildUp が tagCatalog マージを飛ばす（spec: feature-memos/debug_test_play_area.md）
- [x] AllowDebugCameraControl をオーバーライドしない（spec: feature-memos/entity_camera_components.md）
- [x] ExitScene が SceneViewTab を戻さない（spec: feature-memos/debug_test_play_area.md）
- [x] ビュー描画ごとに SetupRuntimeLighting
- [x] 専用ランタイムウィンドウが汎用文言のみ（spec: feature-memos/debug_test_play_area.md）
- [x] 子付きインスタンスは親 Draw 任せ
- [x] 描画毎に半ランバート固定
- [x] EnterScene が Runtime 固定
- [x] camera==nullptr で無言 return
- [x] ctor で displayName を仮設定
- [x] Draw で ChangeScene 常時描画

### LoopTick13Demo
- [x] LoopTick13Demo が機能検証シーンの正本として未整備

### combat_bounds_sandbox
- [x] combat_bounds_sandbox がコード／既定導線から参照されない（spec: feature-memos/debug_test_play_area.md）

### DebugPlayAreaGenerator
- [x] ランタイムプレビュー手順がコード断片のみ（spec: feature-memos/debug_test_play_area.md）

### EditorSceneRuntime
- [x] 親なしインスタンスだけ初期化
- [x] Entity 名が definition のみ

### launch_config
- [x] startup が Feature 検証シーンではない（spec: feature-memos/debug_test_play_area.md）
- [x] 設定読込失敗を区別できない（spec: feature-memos/debug_test_play_area.md）
- [x] startupScenePath 空で false
- [x] JSON は startupScenePath のみ（spec: feature-memos/debug_test_play_area.md）

### EditorSceneAsset
- [x] ListSceneJson は単一 Dir
- [x] startupScenePath を生文字列で保存
- [x] PlayRequest がパスだけ保持

## Lighting

### リムライト
- [x] 実装（`DirectionalLightData`・`Object3d` シェーダ・`FlushDirectionalLightConstantBuffer`）

### ボリュームフォグ／VolumetricFog（統合）
- [x] 実装（`VolumetricFog`・`Object3d.PS`・`Lighting`／`AtrumEngine` 統合）

### ゴッドレイ
- [x] 実装（`GodRays`・`Object3d.PS`・フォグ連動シャフト）

### シェード／シャドウ
- [x] 実装（`DirectionalShadow`・PS 接触シェード・`CmpMesh` 平面影）


### visual_lighting_profile
- [x] ライト／フォグ用 EditorAssets プロファイル JSON が未定義（spec: feature-memos/volumetric_fog.md）

### DirectionalShadow
- [x] シャドウ／シェード適用状態のログ・可視化が無い（spec: feature-memos/directional_shadow.md）

## Repository

### GitHubでのクラウドビルド
- [x] 除外パスによるビルド穴の自動確認（`verify_vcxproj_coverage.py`・CI 前置ジョブ）


### ギャップスキャン追補（P0/P1）
- [x] PostBuild が Assets のみコピーしランタイム参照の EditorAssets が同梱されない（gap: `GAP-Ship-CG2_BuildUp-001`・Ship・P0）
- [x] サブシステム別フレームプロファイラが未実装（gap: `GAP-Infra-frame_profiler-001`・Debug・P1）
- [x] Debug 構成も DEVELOPMENT+USE_IMGUI で本番用コンパイル差分が不明瞭（gap: `GAP-Ship-CG2_BuildUp-002`・Ship・P1）
- [x] buildDevelopment.cmd が CI と異なる入口で前検証も無い（gap: `GAP-Infra-buildDevelopment-001`・Ship・P1）
- [x] Release ビルド後の exe 起動スモークが CI に無い（gap: `GAP-Infra-Release_smoke-001`・Ship・P1）
- [x] Release でも Editor 系ソースが ExcludedFromBuild されずリンクされる（gap: `GAP-Ship-EditorSources-001`・Ship・P1）
- [x] 同梱アセットとランタイム参照パスの整合を検証する仕組みが無い（gap: `GAP-Infra-asset_pack_verify-001`・Ship・P1）
- [x] エディタ既定の機能検証シーン（FeatureTest）が定義されていない（gap: `GAP-Infra-FeatureTestScene-001`・Debug・P1）
- [x] buildDevelopment.cmd に NuGet 復元ステップが無い（gap: `GAP-Infra-buildDevelopment-002`・Ship・P1）
- [x] Release 向け最小起動スモークの手順書が無い（gap: `GAP-Ship-smoke_runbook-001`・Ship・P1）
- [x] ForDebug 系 cpp が Release でコンパイル・リンクされる（gap: `GAP-Ship-ForDebugSources-001`・Ship・P1）
- [x] EditorScenePlayRequest がどこからも Set されない（gap: `GAP-Infra-EditorScenePlayRequest-001`・Debug・P1）

### ギャップスキャン追補（P2/P3・在庫）
- 展開済み: Release 構成・同梱パス・vcxproj 除外の細部（在庫 25件）
  - 正本: `../implementation_gaps.md`（Area=Infra / Ship）


### Development_opt
- [x] 本番相当プロファイル計測用の構成／手順が未定義

### CG2_BuildUp
- [x] Release リンク設定で GenerateDebugInformation が true

### verify_config_matrix
- [x] コンパイル定義と Release 除外の期待マトリクス検証が無い（spec: feature-memos/cloud_build_vcxproj_check.md）

### JsonTest
- [x] JsonTest デバッグパネル用 TU が Release ビルドから除外されていない

### cloud_dev_build
- [x] Cloud Linux 上で Development ビルド再現手段が未定義（spec: feature-memos/cloud_build_vcxproj_check.md）

### main
- [x] スモーク起動用 CLI（例: フレーム数上限）が main に無い

## Editor

### ステージギミックエディタ
- [-] 一本化エディタは影響大・エンティティ json 安定後

### ゲームに必要なものを簡単に作れるエディター
- [-] 依存: json／プレハブ。子項目は後続で細分化
- [-] ビヘイビア／BehaviorTree（設計保留）
- [-] タイルマップ／オブジェクト位置（Editor 統合時）

### テキスト系（%変数名%テンプレ）
- [x] Lua テンプレファイル（`EditorAssets/Templates/Lua/`・`manifest.json`）
- [x] エディタ組み込み（`EditorLuaScriptScaffold`・ドック「スクリプト」・`Project/Assets/Scripts/` へ出力）
- [-] ランタイム読込（Phase 2・Script 節）


### ギャップスキャン追補（P0/P1）
- [x] カメラ tuning JSON のエディタ内プレビュー／ホットリードが未実装（gap: `GAP-Editor-StageViewCameraTuning-001`・Authoring・P1）
- [x] DebugPlayArea 用の EditorAssets パス定数が無い（gap: `GAP-Editor-EditorAssetsPaths-001`・Debug・P1）
- [x] シーン切替時に未保存確認・自動保存が無い（gap: `GAP-Editor-EntityEditor-002`・Authoring・P1）
- [x] エディタメニューに機能検証シーン起動が無い（gap: `GAP-Editor-EditorEntityWorkbench-001`・Debug・P1）
- [x] テスト再生エリア JSON がエディタのシーン一覧に出ない（gap: `GAP-Editor-EntityEditorUi-004`・Debug・P1）
- [x] スロット種別不一致のコンポーネントが無警告でスキップされる（gap: `GAP-Editor-EditorSceneRuntime-002`・Authoring・P1）
- [x] エディットビューに配置用ギズモ／ドラッグ操作が無い（gap: `GAP-Editor-SceneEntityEditor-002`・Authoring・P1）
- [x] 選択エンティティの複製 UI が無く複製が多段手順（gap: `GAP-Editor-EntityEditor-001`・Authoring・P1）
- [x] 親付きインスタンスが Initialize を呼ばれない（gap: `GAP-Editor-EditorSceneRuntime-006`・Play・P1）
- [x] ライト・ポスト系パラメータのエディタ用 UI が無い（gap: `GAP-Editor-AtrumEngineVisual-001`・Authoring・P1）
- [x] エディタプレビューにフォグ／リム／ゴッドレイ／影が無い（gap: `GAP-Editor-SceneEntityEditor-004`・Authoring・P1）
- [x] CmpMesh／Billboard が TryAddEmptyComponent に無く分割例を直組みできない（gap: `GAP-Editor-EditorSceneRuntime-003`・Authoring・P1）
- [x] インスペクタ変更はプレビューのみ更新し自動保存しない（gap: `GAP-Editor-EntityEditor-004`・Authoring・P1）
- [x] AssetPath プレビューが Draw 分割を Data に内包（gap: `GAP-Editor-EditorSceneRuntime-005`・Authoring・P1）
- [x] BuildEntities が camera.main 欠落を補わない（gap: `GAP-Editor-EditorSceneRuntime-007`・Debug・P1）
- [x] BuildPreviewScene がエディタ内ベクタのみ（gap: `GAP-Editor-EntityEditor-011`・Debug・P1）

### ギャップスキャン追補（P2/P3・在庫）
- 展開済み: EntityEditorUi／インスペクタ・プレハブ・タグカタログ導線（在庫 92件・Authoring 中心）
- [ ] SceneEntityEditor プレビューと本番 SceneBuildUp 経路の差分
  - 正本: `../implementation_gaps.md`（Area=Editor）


### EntityEditor
- [x] シーン JSON が開けないとき空シーンへ置換し通知しない
- [x] ProcessDroppedFileImports が Entities 生成まで繋がない
- [x] プレハブから先頭 1 体のみ追加し親は常に解除
- [x] AddEmptyInstance が選択中定義を複製しない
- [x] RemoveSelectedInstance が単一 index のみ
- [x] 選択エンティティの複製が無い
- [x] requestSaveToDisk が needApply を立てない
- [x] LoadFromJsonPath が選択を先頭に戻す
- [x] 削除後は remaining.front() のみ開く
- [x] プレハブ追加は instances[0]
- [x] 削除時に子を祖父へ昇格しない
- [x] 新規作成が requestSaveToDisk 必須
- [ ] 起動設定前に SaveToJsonPath
- [ ] プレハブ出力が Definition のみ
- [ ] ループ内で lastImported が上書き
- [ ] インポート後プレビュー再適用なし

### EditorSceneRuntime
- [ ] EntityHasComponentForSlot が Mesh／Billboard を認識しない（spec: feature-memos/component_split_guidelines.md）
- [ ] AssetPath プレビューは構築時1回

### SceneEntityEditor
- [ ] EnterScene が毎回ディスクから Load し編集中ドキュメントを上書き
- [ ] SetupPreviewCamera が自由位置のみで StageView 未使用（spec: feature-memos/player_friendly_camera_policy.md）

### EntityEditorDefinition
- [ ] 登録コンポーネント一覧がガイドライン／プレビュー経路と同期していない（spec: feature-memos/component_split_guidelines.md）
- [ ] 未登録コンポーネント種別が Draw 扱いになる
- [ ] コンポーネント設定のコピー／ペーストが無い
- [ ] CmpCamera 追加で params が空オブジェクト（spec: feature-memos/entity_camera_components.md）
- [ ] from_json が params を丸ごと代入

### EntityEditorShortcuts
- [ ] エンティティ複製用キーボードショートカットが無い

### EditorEntityWorkbench
- [ ] メニューにライト・ポスト調整ウィンドウが無い
- [ ] メニュー「プレイ」は保存・実行を呼ばない
- [ ] defaultLayoutBuilt が static で一回切り

### EditorFrame
- [ ] FitCameraPerspectiveToViewport が FOV 0.5 固定（spec: feature-memos/odyssey_style_stage_camera.md）

### EditorImportedAsset
- [ ] インポート直後パス表示のみで一括適用 UI が無い
- [ ] Imported 配下のファイル一覧 API が無い
- [ ] 取り込みファイルはドロップコピーだけで、削除と名前変更が無い
- [ ] 衝突解決が stem_N 連番のみ
- [ ] 重複時に stem_N へ自動リネーム

### PrefabAsset
- [ ] プレハブの複数一括配置 UI が無い
- [ ] プレハブ保存が EntityDefinition 単体 JSON
- [ ] シーンへ追加が毎回 1 インスタンス
- [ ] プレハブ JSON は新規出力だけで、削除と名前変更が無い
- [ ] 出力が MakeUnique で上書き更新できない
- [ ] OpenScene が selectedPrefabPath をクリアしない
- [ ] 衝突時 stem_N 連番

### EntityEditorUi
- [ ] ヒエラルキーのドラッグ複製・並べ替えが無い
- [ ] DrawComponentParams が未知 typeName を未対応表示のみ（spec: feature-memos/component_split_guidelines.md）
- [ ] インスペクタでシーン JSON パスを自由編集できる
- [ ] instanceName と entityName を別々に編集しうる
- [ ] トランスフォームのコピー／一括適用 UI が無い
- [ ] タグをすべて解除が選択 1 体のみ
- [ ] コンポーネント削除が選択 1 件のみ
- [ ] 実行順 InputInt が 1 スロットのみ
- [ ] png・obj・gltf を外部ツールで開く操作がエディタに無い
- [ ] CmpCamera が既知型だが未対応表示（spec: feature-memos/entity_camera_components.md）
- [ ] 「オブジェクトを追加」は空のみ
- [ ] コンボ変更で DefaultParams に差し替え
- [ ] 型 Combo で既定 params に上書き
- [ ] 型 Combo 変更は params を既定に置換
- [ ] 直近ドロップ適用が選択中 Cmp のみ
- [ ] シーン JSON パスを InputString 編集
- [ ] ヒット球は選択 1 球のみ編集
- [ ] 未対応コンポーネントは警告表示のみ
- [ ] InputInt で実行順が 1 スロットのみ
- [ ] Selectable はパス選択のみで即開かない
- [ ] 選択中コンポーネントのみ削除
- [ ] 型 Combo で params 初期化
- [ ] 複製ボタンが無い
- [ ] 実行順 InputInt が単体
- [ ] Selectable が path 全文
- [ ] タグ全解除は選択 1 件

### EntityEditorHierarchy
- [ ] ヒエラルキーがタグでフィルタ／一括選択できない
- [ ] Selectable が単一 instanceIndex のみ

### EditorSceneAsset
- [ ] シーン JSON は新規作成と削除だけで、ファイル名の変更が無い

### EntityTagRegistry
- [ ] tagRegistry.json は読み込み専用で、インスペクタから保存できない

### EditorLuaScriptScaffold
- [ ] 生成した Lua や md を外部テキストエディタで開く操作が無い

### EntityEditorTagCatalog
- [ ] 登録タグから全インスタンス一括付与無し
- [ ] 改名は選択 1 タグのみ一括反映
- [ ] 改名は selectedCatalogTag のみ
- [ ] カタログ付与は 1 件ずつ

### FileDropQueue
- [ ] Drain で pending 全消去

## Script

### スクリプト(Lua)環境
- [-] 依存: エンティティ API 安定・バインド方針

### Luaを用いて実行中に動的にコード追加／変更／削除
- [-] 上記と一体・見送り

## UX

### Operator
- [x] 案内吹き出しシステム（`OperatorGuidance::System`・UX JSON・SceneGame）
- [x] チュートリアル置換・ナビキャラ（`navigator`・`tutorialReplacement`・OnBattleReady）


### ギャップスキャン追補（P0/P1）
- [ ] hints.message が保持されるが描画されず文言編集が無効（gap: `GAP-UX-OperatorGuidanceSystem-001`・Authoring・P1）

### ギャップスキャン追補（P2/P3・在庫）
- 展開済み: OperatorGuidance の Authoring 細部（在庫 5件）
  - 正本: `../implementation_gaps.md`（Area=UX）


### OperatorGuidanceSystem
- [ ] 案内トリガー actionId のコード配線が parry 固定（spec: feature-memos/operator_guidance_bubble.md）
- [ ] LoadFromJsonFile が hints 必須で検証・代替が無い（spec: feature-memos/operator_guidance_bubble.md）
- [ ] actionId session は JSON にあるがコードから通知されない（spec: feature-memos/operator_guidance_bubble.md）
- [ ] body テクスチャ無しでも message フォールバック無し（spec: feature-memos/operator_guidance_bubble.md）
- [ ] navigator.displayName を読むが描画しない（spec: feature-memos/operator_guidance_bubble.md）
- [ ] operator_guidance.json をインスペクタで編集できない

### UXAuthoring
- [ ] operator_guidance.json をエディタから編集・プレビューできない

### SceneGame
- [ ] 案内システム有効でも常時 howTo オーバーレイを描画（spec: feature-memos/operator_guidance_bubble.md）

### operator_guidance_verify
- [ ] operator_guidance.json の参照整合チェックが無い（spec: feature-memos/operator_guidance_bubble.md）

## GapScan（索引）

- 機械正本: `../implementation_gaps_index.json`
- 人間向け一覧: `../implementation_gaps.md`（tier 降順・UseCase 束ね）
- 半自律 tick: `cursorInstructions.md` §6.6（「ギャップスキャンを実行して」）
- **取り込み方針:** 本ファイルは P0/P1 を実装タスク化。P2/P3 は節末在庫＋正本で管理（重複 ID は `scopeKey` マージ済み）

## メモ（旧プレースホルダ）

- エンティティエディタ／カメラ演出の細目は上記 [Editor] [Camera] [Scene] に統合済み。
