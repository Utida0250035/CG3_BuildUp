取り込み済み 2026-11-18(水)_12-24-54

# ロードマップ下書き（コピペ用）

「（以下に貼る）」の下に `[大]` / `〇中` / `・小` だけを貼る（本文行なし）。取り込み後、このファイルは `roadmap_rough_old.md` に退避され、空のテンプレが再生成される。

## 書き方（例）

```
[大見出し1]

[大見出し2]
〇中見出し2-1
・小見出し2-1-a
・小見出し2-1-b
```

| 下書き | 意味 | `roadmap.md` |
|--------|------|----------------|
| `[テキスト]` | 大見出し（角括弧。**`<>` は使わない**） | `## テキスト` |
| `〇テキスト` | 中見出し | `### テキスト` |
| `・テキスト` | 小見出し | `- [ ] テキスト` |

---

（以下に貼る）

[Authoring（レベル／アセット制作）]

〇StageViewCameraTuning
・カメラ tuning JSON のエディタ内プレビュー／ホットリロードが未実装（spec: feature-memos/odyssey_style_stage_camera.md）

〇EntityEditor
・シーン切替時に未保存確認・自動保存が無い
・選択エンティティの複製 UI が無く複製が多段手順
・インスペクタ変更はプレビューのみ更新し自動保存しない
・シーン JSON が開けないとき空シーンへ置換し通知しない
・ProcessDroppedFileImports が Entities 生成まで繋がない
・プレハブから先頭 1 体のみ追加し親は常に解除
・AddEmptyInstance が選択中定義を複製しない
・RemoveSelectedInstance が単一 index のみ
・選択エンティティの複製が無い
・requestSaveToDisk が needApply を立てない
・LoadFromJsonPath が選択を先頭に戻す
・削除後は remaining.front() のみ開く
・プレハブ追加は instances[0]
・削除時に子を祖父へ昇格しない
・新規作成が requestSaveToDisk 必須
・起動設定前に SaveToJsonPath
・プレハブ出力が Definition のみ
・ループ内で lastImported が上書き
・インポート後プレビュー再適用なし

〇EditorSceneRuntime
・スロット種別不一致のコンポーネントが無警告でスキップされる（spec: feature-memos/component_split_guidelines.md）
・CmpMesh／Billboard が TryAddEmptyComponent に無く分割例を直組みできない（spec: feature-memos/component_split_guidelines.md）
・AssetPath プレビューが Draw 分割を Data に内包（spec: feature-memos/component_split_guidelines.md）
・EntityHasComponentForSlot が Mesh／Billboard を認識しない（spec: feature-memos/component_split_guidelines.md）
・AssetPath プレビューは構築時1回

〇SceneEntityEditor
・エディットビューに配置用ギズモ／ドラッグ操作が無い
・エディタプレビューにフォグ／リム／ゴッドレイ／影が無い（spec: feature-memos/volumetric_fog.md）
・EnterScene が毎回ディスクから Load し編集中ドキュメントを上書き
・SetupPreviewCamera が自由位置のみで StageView 未使用（spec: feature-memos/player_friendly_camera_policy.md）

〇AtrumEngineVisual
・ライト・ポスト系パラメータのエディタ用 UI が無い（spec: feature-memos/volumetric_fog.md）

〇OperatorGuidanceSystem
・hints.message が保持されるが描画されず文言編集が無効（spec: feature-memos/operator_guidance_bubble.md）
・案内トリガー actionId のコード配線が parry 固定（spec: feature-memos/operator_guidance_bubble.md）
・LoadFromJsonFile が hints 必須で検証・代替が無い（spec: feature-memos/operator_guidance_bubble.md）
・actionId session は JSON にあるがコードから通知されない（spec: feature-memos/operator_guidance_bubble.md）
・body テクスチャ無しでも message フォールバック無し（spec: feature-memos/operator_guidance_bubble.md）
・navigator.displayName を読むが描画しない（spec: feature-memos/operator_guidance_bubble.md）
・operator_guidance.json をインスペクタで編集できない

〇UiState
・複数選択が無く一括調整できない

〇stage_view_camera_tuning
・tuning.json と StageViewCamera 定数の自動同期が無い
・tuning JSON の client 解像度が参照されない（spec: feature-memos/odyssey_style_stage_camera.md）

〇EntityEditorDefinition
・登録コンポーネント一覧がガイドライン／プレビュー経路と同期していない（spec: feature-memos/component_split_guidelines.md）
・未登録コンポーネント種別が Draw 扱いになる
・コンポーネント設定のコピー／ペーストが無い
・CmpCamera 追加で params が空オブジェクト（spec: feature-memos/entity_camera_components.md）
・from_json が params を丸ごと代入

〇EntityEditorShortcuts
・エンティティ複製用キーボードショートカットが無い

〇UXAuthoring
・operator_guidance.json をエディタから編集・プレビューできない

〇EditorEntityWorkbench
・メニューにライト・ポスト調整ウィンドウが無い
・メニュー「プレイ」は保存・実行を呼ばない
・defaultLayoutBuilt が static で一回切り

〇EditorFrame
・FitCameraPerspectiveToViewport が FOV 0.5 固定（spec: feature-memos/odyssey_style_stage_camera.md）

〇EditorImportedAsset
・インポート直後パス表示のみで一括適用 UI が無い
・Imported 配下のファイル一覧 API が無い
・取り込みファイルはドロップコピーだけで、削除と名前変更が無い
・衝突解決が stem_N 連番のみ
・重複時に stem_N へ自動リネーム

〇PrefabAsset
・プレハブの複数一括配置 UI が無い
・プレハブ保存が EntityDefinition 単体 JSON
・シーンへ追加が毎回 1 インスタンス
・プレハブ JSON は新規出力だけで、削除と名前変更が無い
・出力が MakeUnique で上書き更新できない
・OpenScene が selectedPrefabPath をクリアしない
・衝突時 stem_N 連番

〇visual_lighting_profile
・ライト／フォグ用 EditorAssets プロファイル JSON が未定義（spec: feature-memos/volumetric_fog.md）

〇EntityEditorUi
・ヒエラルキーのドラッグ複製・並べ替えが無い
・DrawComponentParams が未知 typeName を未対応表示のみ（spec: feature-memos/component_split_guidelines.md）
・インスペクタでシーン JSON パスを自由編集できる
・instanceName と entityName を別々に編集しうる
・トランスフォームのコピー／一括適用 UI が無い
・タグをすべて解除が選択 1 体のみ
・コンポーネント削除が選択 1 件のみ
・実行順 InputInt が 1 スロットのみ
・png・obj・gltf を外部ツールで開く操作がエディタに無い
・CmpCamera が既知型だが未対応表示（spec: feature-memos/entity_camera_components.md）
・「オブジェクトを追加」は空のみ
・コンボ変更で DefaultParams に差し替え
・型 Combo で既定 params に上書き
・型 Combo 変更は params を既定に置換
・直近ドロップ適用が選択中 Cmp のみ
・シーン JSON パスを InputString 編集
・ヒット球は選択 1 球のみ編集
・未対応コンポーネントは警告表示のみ
・InputInt で実行順が 1 スロットのみ
・Selectable はパス選択のみで即開かない
・選択中コンポーネントのみ削除
・型 Combo で params 初期化
・複製ボタンが無い
・実行順 InputInt が単体
・Selectable が path 全文
・タグ全解除は選択 1 件

〇EntityEditorHierarchy
・ヒエラルキーがタグでフィルタ／一括選択できない
・Selectable が単一 instanceIndex のみ

〇SceneGame
・案内システム有効でも常時 howTo オーバーレイを描画（spec: feature-memos/operator_guidance_bubble.md）

〇component_split_next
・ランタイム用 Data／Collider 分割候補のエディタ登録が未着手（spec: feature-memos/component_split_guidelines.md）

〇component_split_verify
・コンポーネント登録の三箇所同期を検証する自動化が無い（spec: feature-memos/component_split_guidelines.md）

〇operator_guidance_verify
・operator_guidance.json の参照整合チェックが無い（spec: feature-memos/operator_guidance_bubble.md）

〇EditorSceneAsset
・シーン JSON は新規作成と削除だけで、ファイル名の変更が無い

〇EntityTagRegistry
・tagRegistry.json は読み込み専用で、インスペクタから保存できない

〇EditorLuaScriptScaffold
・生成した Lua や md を外部テキストエディタで開く操作が無い

〇EntityEditorTagCatalog
・登録タグから全インスタンス一括付与無し
・改名は選択 1 タグのみ一括反映
・改名は selectedCatalogTag のみ
・カタログ付与は 1 件ずつ

〇FileDropQueue
・Drain で pending 全消去

[Debug（デバッグ・プロファイリング）]

〇Lighting
・Lighting::ImGui（フォグ／ゴッドレイ／影）が未配線（spec: feature-memos/god_rays.md）

〇SceneBuildUpRegistry
・最小ランタイム検証シーン起動がエディタから未配線（spec: feature-memos/debug_test_play_area.md）
・Launch は生成してポインタ返却のみ（spec: feature-memos/debug_test_play_area.md）
・Launch は Scene 生成のみ
・Launch が同一パス重複を防がない

〇EditorAssetsPaths
・DebugPlayArea 用の EditorAssets パス定数が無い（spec: feature-memos/debug_test_play_area.md）

〇EditorEntityWorkbench
・エディタメニューに機能検証シーン起動が無い

〇StreamingDebug
・~~decoderBacklog01 のクランプで remaining 肥大が見えない（spec: feature-memos/streaming_debug_imgui.md）~~
・ストリーミング診断が ImGui 表示のみでログに残らない（spec: feature-memos/streaming_debug_imgui.md）

〇frame_profiler
・サブシステム別フレームプロファイラが未実装

〇AtrumEngineLighting
・描画診断用ライティングパネルが EditorFrame に無い

〇EntityEditorUi
・テスト再生エリア JSON がエディタのシーン一覧に出ない
・Release 起動パスを表示するだけ（spec: feature-memos/debug_test_play_area.md）

〇FeatureTestScene
・エディタ既定の機能検証シーン（FeatureTest）が定義されていない

〇SceneViewTab
・実行ビューがプレビュー Entity のみで機能単体試験にならない

〇EditorScenePlayRequest
・~~EditorScenePlayRequest がどこからも Set されない（spec: feature-memos/debug_test_play_area.md）~~
・EditorScenePlayRequest::Path が参照されない（spec: feature-memos/debug_test_play_area.md）
・EditorScenePlayRequest::Clear が呼ばれない（spec: feature-memos/debug_test_play_area.md）
・Set 後に BuildUp 起動が無い（spec: feature-memos/debug_test_play_area.md）

〇SceneEntityEditor
・シーン実行が本番相当 SceneBuildUp 経路にならない
・F2 はパーティクル遷移のみで機能検証シーン未割当（spec: feature-memos/debug_test_play_area.md）
・シーン実行後は Runtime タブフォーカスのみ（spec: feature-memos/debug_test_play_area.md）
・SetupPreviewCamera が GetCamera のみ更新（spec: feature-memos/entity_camera_components.md）
・Runtime タブで SetupStageViewCamera に差し替え（spec: feature-memos/entity_camera_components.md）
・実行は Runtime タブ切替のみ
・Edit ホバー時に Draw 内で camera->Update
・シーンビュー RT が GameOrder 後に SetCamera
・DrawPreview が親 root のみ
・Exit はプレビュー解放のみ

〇FrameDeltaTime
・フレーム時間のオーバーレイ／ヒストグラムが無い

〇StreamingMenu
・ストリーミングデバッグ ON が SceneBuildUp 経路で開けない（spec: feature-memos/streaming_debug_imgui.md）

〇scaffold_play_area
・テストエリア新規作成が scaffold スクリプト手動実行のみ
・生成後は Launch のみ案内（spec: feature-memos/debug_test_play_area.md）
・生成は sceneName 差替のみ

〇Development_opt
・本番相当プロファイル計測用の構成／手順が未定義

〇DirectionalShadow
・シャドウ／シェード適用状態のログ・可視化が無い（spec: feature-memos/directional_shadow.md）

〇CommandScene
・起動時シーンが SceneEntityEditor に固定（spec: feature-memos/debug_test_play_area.md）
・startup 空なら BuildUp ではなく Title（spec: feature-memos/debug_test_play_area.md）
・Launch 前に TryLoad を呼ばない
・sceneNow 確定後 Enter を無条件

〇SceneBuildUp
・returnScene が null だと機能検証シーンから抜けられない（spec: feature-memos/debug_test_play_area.md）
・SceneBuildUp::Draw が EditorFrame ワークベンチを常時描画（spec: feature-memos/debug_test_play_area.md）
・SceneBuildUp が EditorSceneRuntime を参照しない（spec: feature-memos/debug_test_play_area.md）
・entities_ と GetCamera の cameraRig が未接続（spec: feature-memos/entity_camera_components.md）
・ロード失敗時に空の Entity_0 を生成（spec: feature-memos/debug_test_play_area.md）
・cameraRig 実体は Update されない（spec: feature-memos/entity_camera_components.md）
・ロード直後 SetupStageViewCamera を呼ぶ（spec: feature-memos/entity_camera_components.md）
・DrawRuntimeViewContent が Prepare 後に SetupStageViewCamera（spec: feature-memos/entity_camera_components.md）
・IsStageViewCameraLocked が常時 true（spec: feature-memos/debug_test_play_area.md）
・遷移演出中 F1 復帰が処理されない（spec: feature-memos/debug_test_play_area.md）
・BuildUp が tagCatalog マージを飛ばす（spec: feature-memos/debug_test_play_area.md）
・AllowDebugCameraControl をオーバーライドしない（spec: feature-memos/entity_camera_components.md）
・ExitScene が SceneViewTab を戻さない（spec: feature-memos/debug_test_play_area.md）
・ビュー描画ごとに SetupRuntimeLighting
・専用ランタイムウィンドウが汎用文言のみ（spec: feature-memos/debug_test_play_area.md）
・子付きインスタンスは親 Draw 任せ
・描画毎に半ランバート固定
・EnterScene が Runtime 固定
・camera==nullptr で無言 return
・ctor で displayName を仮設定
・Draw で ChangeScene 常時描画

〇LoopTick13Demo
・LoopTick13Demo が機能検証シーンの正本として未整備

〇combat_bounds_sandbox
・combat_bounds_sandbox がコード／既定導線から参照されない（spec: feature-memos/debug_test_play_area.md）

〇DebugPlayAreaGenerator
・ランタイムプレビュー手順がコード断片のみ（spec: feature-memos/debug_test_play_area.md）

〇SceneCameraRig
・FindPrimaryCmpCamera が rig 配列のみ走査（spec: feature-memos/entity_camera_components.md）

〇EditorSceneRuntime
・BuildEntities が camera.main 欠落を補わない（spec: feature-memos/entity_camera_components.md）
・親なしインスタンスだけ初期化
・Entity 名が definition のみ

〇CmpCamera
・Initialize が Orthographic を既定生成（spec: feature-memos/entity_camera_components.md）

〇EntityEditor
・BuildPreviewScene がエディタ内ベクタのみ

〇launch_config
・startup が Feature 検証シーンではない（spec: feature-memos/debug_test_play_area.md）
・設定読込失敗を区別できない（spec: feature-memos/debug_test_play_area.md）
・startupScenePath 空で false
・JSON は startupScenePath のみ（spec: feature-memos/debug_test_play_area.md）

〇EditorSceneAsset
・ListSceneJson は単一 Dir
・startupScenePath を生文字列で保存
・PlayRequest がパスだけ保持

[Play（ランタイムのゲームプレイ）]

〇SceneMovieRegistry
・ムービーシーン起動がゲームプレイ経路に未接続（spec: feature-memos/scene_movie.md）

〇EditorSceneRuntime
・親付きインスタンスが Initialize を呼ばれない

〇SceneMovie
・returnScene が null だとムービー終了／失敗後もシーンが抜けられない（spec: feature-memos/scene_movie.md）
・movie トラックパスを EditorAssets 基準に解決していない（spec: feature-memos/movie_camera_controller.md）
・トラック読込失敗時に初回 Update で即戻り（エラー表示なし）
・SkyDome がトラック先頭姿勢前のカメラに紐づく（spec: feature-memos/scene_movie.md）
・トラック開始失敗時に SetupStageViewCamera を呼ぶ（spec: feature-memos/scene_movie.md）
・ムービーシーンに BGM/トラック音声の再生・復帰ポリシーがコード上見えない（spec: feature-memos/scene_movie.md）
・遷移予約後はカメラ／SkyDome 更新が停止（spec: feature-memos/scene_movie.md）
・SceneMovie のライティングにリム・ボリュームフォグが無い（spec: feature-memos/rim_light.md）

〇GameOrder
・遷移演出中はシーン Update が全面停止
・フレームループが音声を管理しない

〇SceneGame
・DrawScene 毎に見た目リテラルを再適用しデータ反映が上書きされる（spec: feature-memos/volumetric_fog.md）
・SetupGameLighting が Fog 設定後に Flush しない（spec: feature-memos/volumetric_fog.md）
・見た目パラメータがデータではなく SetupGameLighting 直書き（spec: feature-memos/volumetric_fog.md）
・OnBossDefeated で BGM を演出完了前に停止
・シーン入場時の operator_guidance 読込失敗を検知・代替しない
・EnterScene が本編見た目プリセットを適用しない（spec: feature-memos/rim_light.md）
・SpawnEntities がイントロカメラ設定より先に実行される
・撃破演出中は MaintainSceneBgmIfNeeded が無効
・OnPlayerDefeated は BGM を止めず撃破は即停止
・イントロ中の Update 早期 return で弾幕 SE 無音
・イントロ中に Maintain が PlaySceneBgm
・pendingDefeat 中は Maintain が return
・フェード中も Exit で StopBgmLoop
・遷移演出中は MaintainSceneBgm を呼ばない

〇StageBounds
・StageView カメラが StageBounds 定数のみ参照しシーン毎の範囲に追従しない（spec: feature-memos/player_friendly_camera_policy.md）

〇Lighting
・Visual::Lighting と AtrumEngine の VolumetricFog が非共有（spec: feature-memos/volumetric_fog.md）

〇ChangeSceneFade
・フェードイン開始時に遷移 SE が未再生（フラグのみ立つ）
・timeRatio 満了で即 isSceneChange となり中間で EnterScene
・フェード中盤で次 BGM が始まる

〇CommandScene
・Development 起動の初期シーンがプレイ用シーンロード経路と一致しない
・シーン切替で BGM が瞬断し、クロスフェード／継続再生ポリシーが無い

〇CommandChangeScene
・遷移実行中の Set は無視され次シーン要求が失われる

〇SceneBuildUp
・BuildUp 描画に地形越し自機シルエットが無い（spec: feature-memos/player_terrain_silhouette.md）
・BuildUp ランタイム描画にフォグ・リム・ゴッドレイ・シャドウが無い
・BuildUp の sceneJsonPath_ を基準パス解決しない
・SceneBuildUp のカメラがシーン JSON の境界情報を読まない
・シーン JSON 読込失敗時にデフォルト 1 体へ置換しプレイヤーに通知しない
・SceneBuildUp に BGM／シーン SE の入退場が無い

〇PauseBgmLoop
・Pause/Resume/Volume 付き BGM API がシーンから未使用

〇StopBgmLoop
・StopBgmLoop がシーン BGM 以外のストリーミングも停止

〇PlaySceneBgm
・PlaySceneBgm 開始時に全ボイスを一括停止
・Init 失敗で voice playId だけ巻き戻し

〇SceneTitle
・Title の BGM 途切れを検知・再開する処理が無い
・SceneTitle のライティングにリム・フォグ・ゴッドレイが無い（spec: feature-memos/rim_light.md）
・ExitScene がフェード完了前に StopBgmLoop
・Submit 直後にフェード遷移

〇StageViewCamera
・ステージカメラのアスペクトが既定解像度固定

〇verify_camera_policy
・カメラ方針静的検証が CI／ビルド手順に未組み込み
・verify_camera_policy が AllowDebugCameraControl を検査しない

〇SceneResult
・SceneResult に BGM 維持・再開処理が無い
・SceneResult が毎フレーム簡易平行光のみ適用（spec: feature-memos/rim_light.md）
・ExitScene が即 StopBgmLoop

〇Scene
・AllowDebugCameraControl が既定 true でガードが二重依存（spec: feature-memos/player_friendly_camera_policy.md）
・新規シーンのステージカメラロックはドキュメント依存（spec: feature-memos/player_friendly_camera_policy.md）

〇SceneParticleEditor
・SceneParticleEditor が AllowDebugCameraControl を限定していない（spec: feature-memos/player_friendly_camera_policy.md）

〇MovieCameraTrackLoader
・キーフレーム 1 件の不正でトラック全体を拒否（spec: feature-memos/movie_camera_controller.md）
・ムービートラック JSON のスモーク検証が自動化されていない（spec: feature-memos/movie_camera_controller.md）

〇PlayBgmLoop
・PlayBgmLoop 失敗時に PlaySceneBgm を再実行

〇ResumeBgmLoop
・ResumeBgmLoop がシーンから呼ばれない

〇SetBgmLoopVolume
・SetBgmLoopVolume が未使用

〇PlaySeLoop
・PlaySeLoop がコードベースで未呼び出し

〇CmpLhRigidBody
・物理演算が基本機能として独立せず、剛体コンポーネントの Update に閉じている

〇EngineAccess
・描画への外部アクセスが EngineAccess に無く、AtrumEngine 経由になる

〇CmpBounceBall
・ボールの運動・壁床バウンス・衝突演出が1クラスに同居している

〇AudioManager
・空き無しで全汎用ストリームをリセット
・BGM 一時停止中は復帰判定が誤る
・Resume は Start のみでデコード再開なし
・StopAll は BGM 系ループのみ
・Paused ストリームは Update をスキップ
・Handle と voice の playId がずれる
・Start 後に Refill を 1 回だけ
・BGM フォールバックで短音ボイス解放
・ストリーム BGM 停止は XAudio Stop
・Resume は Start 後 Refill なし
・再生位置計算が 44100 固定
・Waiting 非ループは Stop で終了
・ボイス枯渇で assert
・Init 失敗で playId 不一致
・ストリーム PCM を 44.1k 固定
・SetupStreaming が 44.1kHz 固定
・Load ヒットで再判定なし
・不足バイトを 0 でパディング

〇PlaySe
・PlaySe が Play の Handle を捨てる

〇StreamingVoiceCallback
・OnStreamEnd が Waiting のみセット
・OnBufferEnd が Streaming へ遷移

〇AudioDecoder
・EOF で break のみ
・4 バイト揃えで端数を次回へ

〇IsBgmLoopPaused
・IsBgmLoopPaused がシーンから未使用

〇IsSuitableStreaming
・ファイルサイズだけでストリーム判定
・閾値 5MB がハードコード

〇GetBgmLoopPlaybackSeconds
・再生秒取得がゲームシーン未使用

〇StopSe
・ループ SE ハンドルの個別停止が無い

〇PlaySeDetail
・キャッシュ命中で再 Load しない
・static cache がアンロードされない

〇SetBgmLoopPlaybackSeconds
・再生位置指定がゲームシーン未使用

〇SetBgmLoopPitch
・SetBgmLoopPitch がシーンから未呼び出し

〇SetSeVolume
・SetSeVolume がゲームロジックから未使用

〇GetSeVolume
・GetSeVolume がシーンから未呼び出し

〇GetSeFrequencyRatio
・GetSeFrequencyRatio が未使用

〇SetSeFrequencyRatio
・SetSeFrequencyRatio がゲームから未使用

[Ship（ビルド・配布）]

〇CG2_BuildUp
・~~PostBuild が Assets のみコピーしランタイム参照の EditorAssets が同梱されない~~
・Debug 構成も DEVELOPMENT+USE_IMGUI で本番用コンパイル差分が不明瞭
・Release リンク設定で GenerateDebugInformation が true

〇buildDevelopment
・buildDevelopment.cmd が CI と異なる入口で前検証も無い（spec: feature-memos/cloud_build_vcxproj_check.md）
・buildDevelopment.cmd に NuGet 復元ステップが無い

〇Release_smoke
・Release ビルド後の exe 起動スモークが CI に無い

〇EditorSources
・Release でも Editor 系ソースが ExcludedFromBuild されずリンクされる

〇asset_pack_verify
・同梱アセットとランタイム参照パスの整合を検証する仕組みが無い

〇smoke_runbook
・Release 向け最小起動スモークの手順書が無い

〇ForDebugSources
・ForDebug 系 cpp が Release でコンパイル・リンクされる

〇verify_config_matrix
・コンパイル定義と Release 除外の期待マトリクス検証が無い（spec: feature-memos/cloud_build_vcxproj_check.md）

〇JsonTest
・JsonTest デバッグパネル用 TU が Release ビルドから除外されていない

〇BinaryConverter
・メッシュ変換パイプラインのバッチ／CI フックが未整備（spec: feature-memos/atr_mesh_binary_format.md）

〇cloud_dev_build
・Cloud Linux 上で Development ビルド再現手段が未定義（spec: feature-memos/cloud_build_vcxproj_check.md）

〇main
・スモーク起動用 CLI（例: フレーム数上限）が main に無い
