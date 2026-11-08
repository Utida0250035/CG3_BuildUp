# 実装ロードマップ（箇条書き）

正本。下書きは `roadmap_rough.md` から起こす（`2026-11-08` 取り込み済み）。

**チェック規則:** `- [ ]` 未着手 / `- [~]` 進行中 / `- [x]` 完了 / `- [-]` 保留（`progress_report` に理由）

---

## Audio

### Bgmのためのストリーミング再生
- [x] ストリーミング BGM（`AudioManager::SetupStreaming` / `PlaySceneBgm`）

### 再生時にピッチなどのステータス調整を行なう
- [x] ピッチ等ステータス API（BGM／SE）（`SetBgmLoopPitch` / `SetSeVolume` 他）

### Bgmの一時停止や再開
- [x] シーン BGM の一時停止・再開（`PauseBgmLoop` / `ResumeBgmLoop`）
- [x] 再生地点の記録／指定（`GetBgmLoopPlaybackSeconds` / `SetBgmLoopPlaybackSeconds`、ストリーミング／短尺）

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

## Scene

### ムービー用カメラコントローラ
- [x] 実装（`MovieCameraController`・JSON トラック・`sample_movie_track.json`）

### ムービー用シーン
- [x] 実装（`SceneMovie`・Registry・sample トラック連携）

## Render

### オブジェクトの粒々透過
- [x] 実装（`DITHERED_ALPHA`・Bayer clip PS・`CmpMesh`）

### 自機のシルエット透視
- [x] 地形越しシルエット表示（`OCCLUDED_SILHOUETTE`・`PlayerTerrainSilhouette`）

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

## Lighting

### リムライト
- [x] 実装（`DirectionalLightData`・`Object3d` シェーダ・`FlushDirectionalLightConstantBuffer`）

### ボリュームフォグ／VolumetricFog（統合）
- [x] 実装（`VolumetricFog`・`Object3d.PS`・`Lighting`／`AtrumEngine` 統合）

### ゴッドレイ
- [x] 実装（`GodRays`・`Object3d.PS`・フォグ連動シャフト）

### シェード／シャドウ
- [ ] 実装

## Repository

### GitHubでのクラウドビルド
- [ ] 除外パスによるビルド穴の自動確認

## Editor

### ステージギミックエディタ
- [-] 一本化エディタは影響大・エンティティ json 安定後

### ゲームに必要なものを簡単に作れるエディター
- [-] 依存: json／プレハブ。子項目は後続で細分化
- [-] ビヘイビア／BehaviorTree（設計保留）
- [-] タイルマップ／オブジェクト位置（Editor 統合時）

### テキスト系（%変数名%テンプレ）
- [ ] テンプレファイル
- [ ] 作成・コピー・置換コード
- [ ] エディタ組み込み

## Script

### スクリプト(Lua)環境
- [-] 依存: エンティティ API 安定・バインド方針

### Luaを用いて実行中に動的にコード追加／変更／削除
- [-] 上記と一体・見送り

## UX

### Operator
- [ ] 案内吹き出しシステム
- [ ] チュートリアル置換・ナビキャラ

## メモ（旧プレースホルダ）

- エンティティエディタ／カメラ演出の細目は上記 [Editor] [Camera] [Scene] に統合済み。
