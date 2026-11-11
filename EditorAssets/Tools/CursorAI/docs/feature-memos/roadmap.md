# 実装ロードマップ（箇条書き）

正本。下書きは `roadmap_rough.md` から起こす（`2026-11-08` 取り込み済み）。

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
- [ ] decoderBacklog01 のクランプで remaining 肥大が見えない（gap: `GAP-Audio-StreamingDebug-001`・Debug・P1）

### ギャップスキャン追補（P2/P3・在庫）
- [ ] シーン遷移時の BGM／SE 継続・クロスフェード・遷移 SE（CommandScene / ChangeSceneFade 系。在庫 45件）
- [ ] AudioManager ストリーミング／デコーダ／ボイスコールバック境界（在庫内の AudioManager 中心）
  - 正本: `../implementation_gaps.md` / `implementation_gaps_index.json`（Area=Audio）

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
- [ ] Entity／Runtime まわり残件（在庫 3件）
  - 正本: `../implementation_gaps.md`

## Loader

### ImGuiウィンドウにファイルをドロップしてobjやpngなどの読み込み
- [x] ドロップでパス保持・読み込み（SDL ドロップ→Imported・png ビルボードプレビュー）

### 別スレッドでのリソース読み込み
- [-] 安定化後（並行読み込みは影響大）

### Binaryやddsへのファイル変換ツール
- [x] BinaryConverter（`EditorAssets/Tools/BinaryConverter`・`.amesh` v1）
- [x] DdsConverter（`EditorAssets/Tools/DdsConverter`・RGBA8 DDS）

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
- [ ] StageView カメラが StageBounds 定数のみ参照しシーン毎の範囲に追従しない（gap: `GAP-Camera-StageBounds-001`・Play・P1）

### ギャップスキャン追補（P2/P3・在庫）
- [ ] tuning.json と StageViewCamera 定数の自動同期（在庫 1件）
  - 正本: `../implementation_gaps.md`

## Scene

### ムービー用カメラコントローラ
- [x] 実装（`MovieCameraController`・JSON トラック・`sample_movie_track.json`）

### ムービー用シーン
- [x] 実装（`SceneMovie`・Registry・sample トラック連携）


### ギャップスキャン追補（P0/P1）
- [ ] ムービーシーン起動がゲームプレイ経路に未接続（gap: `GAP-Scene-SceneMovieRegistry-001`・Play・P1）
- [ ] 最小ランタイム検証シーン起動がエディタから未配線（gap: `GAP-Scene-SceneBuildUpRegistry-001`・Debug・P1）
- [ ] returnScene が null だとムービー終了／失敗後もシーンが抜けられない（gap: `GAP-Scene-SceneMovie-004`・Play・P1）
- [ ] 遷移演出中はシーン Update が全面停止（gap: `GAP-Scene-GameOrder-001`・Play・P1）
- [ ] FindPrimaryCmpCamera が rig 配列のみ走査（gap: `GAP-Scene-SceneCameraRig-001`・Debug・P1）
- [ ] entities_ と GetCamera の cameraRig が未接続（gap: `GAP-Scene-SceneBuildUp-009`・Debug・P1）

### ギャップスキャン追補（P2/P3・在庫）
- [ ] SceneBuildUp 検証経路・レジストリ・cameraRig 接続の細部（在庫 65件）
- [ ] CommandScene 遷移要求・Development 起動シーン不一致
  - 正本: `../implementation_gaps.md`（Area=Scene）

## Render

### オブジェクトの粒々透過
- [x] 実装（`DITHERED_ALPHA`・Bayer clip PS・`CmpMesh`）

### 自機のシルエット透視
- [x] 地形越しシルエット表示（`OCCLUDED_SILHOUETTE`・`PlayerTerrainSilhouette`）


### ギャップスキャン追補（P0/P1）
- [ ] Lighting::ImGui（フォグ／ゴッドレイ／影）が未配線（gap: `GAP-Render-Lighting-001`・Debug・P1）
- [ ] DrawScene 毎に見た目リテラルを再適用しデータ反映が上書きされる（gap: `GAP-Render-SceneGame-002`・Play・P1）
- [ ] Visual::Lighting と AtrumEngine の VolumetricFog が非共有（gap: `GAP-Render-Lighting-002`・Play・P1）
- [ ] SetupGameLighting が Fog 設定後に Flush しない（gap: `GAP-Render-SceneGame-003`・Play・P1）

### ギャップスキャン追補（P2/P3・在庫）
- [ ] SceneGame 描画／ライティング反映の残件（在庫 6件）
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
- [ ] 描画診断用ライティングパネルが EditorFrame に無い（gap: `GAP-Engine-AtrumEngineLighting-001`・Debug・P1）
- [ ] 実行ビューがプレビュー Entity のみで機能単体試験にならない（gap: `GAP-Debug-SceneViewTab-001`・Debug・P1）

### ギャップスキャン追補（P2/P3・在庫）
- [ ] フレーム時間オーバーレイ・ストリーミングデバッグメニュー導線（在庫 2件）
  - 正本: `../implementation_gaps.md`（Area=Debug）

## Lighting

### リムライト
- [x] 実装（`DirectionalLightData`・`Object3d` シェーダ・`FlushDirectionalLightConstantBuffer`）

### ボリュームフォグ／VolumetricFog（統合）
- [x] 実装（`VolumetricFog`・`Object3d.PS`・`Lighting`／`AtrumEngine` 統合）

### ゴッドレイ
- [x] 実装（`GodRays`・`Object3d.PS`・フォグ連動シャフト）

### シェード／シャドウ
- [x] 実装（`DirectionalShadow`・PS 接触シェード・`CmpMesh` 平面影）

## Repository

### GitHubでのクラウドビルド
- [x] 除外パスによるビルド穴の自動確認（`verify_vcxproj_coverage.py`・CI 前置ジョブ）


### ギャップスキャン追補（P0/P1）
- [ ] PostBuild が Assets のみコピーしランタイム参照の EditorAssets が同梱されない（gap: `GAP-Ship-CG2_BuildUp-001`・Ship・P0）
- [ ] サブシステム別フレームプロファイラが未実装（gap: `GAP-Infra-frame_profiler-001`・Debug・P1）
- [ ] Debug 構成も DEVELOPMENT+USE_IMGUI で本番用コンパイル差分が不明瞭（gap: `GAP-Ship-CG2_BuildUp-002`・Ship・P1）
- [ ] buildDevelopment.cmd が CI と異なる入口で前検証も無い（gap: `GAP-Infra-buildDevelopment-001`・Ship・P1）
- [ ] Release ビルド後の exe 起動スモークが CI に無い（gap: `GAP-Infra-Release_smoke-001`・Ship・P1）
- [ ] Release でも Editor 系ソースが ExcludedFromBuild されずリンクされる（gap: `GAP-Ship-EditorSources-001`・Ship・P1）
- [ ] 同梱アセットとランタイム参照パスの整合を検証する仕組みが無い（gap: `GAP-Infra-asset_pack_verify-001`・Ship・P1）
- [ ] エディタ既定の機能検証シーン（FeatureTest）が定義されていない（gap: `GAP-Infra-FeatureTestScene-001`・Debug・P1）
- [ ] buildDevelopment.cmd に NuGet 復元ステップが無い（gap: `GAP-Infra-buildDevelopment-002`・Ship・P1）
- [ ] Release 向け最小起動スモークの手順書が無い（gap: `GAP-Ship-smoke_runbook-001`・Ship・P1）
- [ ] ForDebug 系 cpp が Release でコンパイル・リンクされる（gap: `GAP-Ship-ForDebugSources-001`・Ship・P1）
- [ ] EditorScenePlayRequest がどこからも Set されない（gap: `GAP-Infra-EditorScenePlayRequest-001`・Debug・P1）

### ギャップスキャン追補（P2/P3・在庫）
- [ ] Release 構成・同梱パス・vcxproj 除外の細部（在庫 25件）
  - 正本: `../implementation_gaps.md`（Area=Infra / Ship）

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
- [ ] カメラ tuning JSON のエディタ内プレビュー／ホットリードが未実装（gap: `GAP-Editor-StageViewCameraTuning-001`・Authoring・P1）
- [ ] DebugPlayArea 用の EditorAssets パス定数が無い（gap: `GAP-Editor-EditorAssetsPaths-001`・Debug・P1）
- [ ] シーン切替時に未保存確認・自動保存が無い（gap: `GAP-Editor-EntityEditor-002`・Authoring・P1）
- [ ] エディタメニューに機能検証シーン起動が無い（gap: `GAP-Editor-EditorEntityWorkbench-001`・Debug・P1）
- [ ] テスト再生エリア JSON がエディタのシーン一覧に出ない（gap: `GAP-Editor-EntityEditorUi-004`・Debug・P1）
- [ ] スロット種別不一致のコンポーネントが無警告でスキップされる（gap: `GAP-Editor-EditorSceneRuntime-002`・Authoring・P1）
- [ ] エディットビューに配置用ギズモ／ドラッグ操作が無い（gap: `GAP-Editor-SceneEntityEditor-002`・Authoring・P1）
- [ ] 選択エンティティの複製 UI が無く複製が多段手順（gap: `GAP-Editor-EntityEditor-001`・Authoring・P1）
- [ ] 親付きインスタンスが Initialize を呼ばれない（gap: `GAP-Editor-EditorSceneRuntime-006`・Play・P1）
- [ ] ライト・ポスト系パラメータのエディタ用 UI が無い（gap: `GAP-Editor-AtrumEngineVisual-001`・Authoring・P1）
- [ ] エディタプレビューにフォグ／リム／ゴッドレイ／影が無い（gap: `GAP-Editor-SceneEntityEditor-004`・Authoring・P1）
- [ ] CmpMesh／Billboard が TryAddEmptyComponent に無く分割例を直組みできない（gap: `GAP-Editor-EditorSceneRuntime-003`・Authoring・P1）
- [ ] インスペクタ変更はプレビューのみ更新し自動保存しない（gap: `GAP-Editor-EntityEditor-004`・Authoring・P1）
- [ ] AssetPath プレビューが Draw 分割を Data に内包（gap: `GAP-Editor-EditorSceneRuntime-005`・Authoring・P1）
- [ ] BuildEntities が camera.main 欠落を補わない（gap: `GAP-Editor-EditorSceneRuntime-007`・Debug・P1）
- [ ] BuildPreviewScene がエディタ内ベクタのみ（gap: `GAP-Editor-EntityEditor-011`・Debug・P1）

### ギャップスキャン追補（P2/P3・在庫）
- [ ] EntityEditorUi／インスペクタ・プレハブ・タグカタログ導線（在庫 92件・Authoring 中心）
- [ ] SceneEntityEditor プレビューと本番 SceneBuildUp 経路の差分
  - 正本: `../implementation_gaps.md`（Area=Editor）

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
- [ ] OperatorGuidance の Authoring 細部（在庫 5件）
  - 正本: `../implementation_gaps.md`（Area=UX）

## GapScan（索引）

- 機械正本: `../implementation_gaps_index.json`
- 人間向け一覧: `../implementation_gaps.md`（tier 降順・UseCase 束ね）
- 半自律 tick: `cursorInstructions.md` §6.6（「ギャップスキャンを実行して」）
- **取り込み方針:** 本ファイルは P0/P1 を実装タスク化。P2/P3 は節末在庫＋正本で管理（重複 ID は `scopeKey` マージ済み）

## メモ（旧プレースホルダ）

- エンティティエディタ／カメラ演出の細目は上記 [Editor] [Camera] [Scene] に統合済み。
