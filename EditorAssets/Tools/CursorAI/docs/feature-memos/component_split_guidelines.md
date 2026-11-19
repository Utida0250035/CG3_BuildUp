# コンポーネント分割方針（Atrum）

`unity_style_entity_guidelines.md` の「小さく役割限定」を具体化する。新規・分割時は本メモと `EntityEditor` の登録表を更新する。

## 分割の判断

| 寄せない | 分ける |
|----------|--------|
| 描画＋当たり＋入力を 1 クラス | Draw / Upd / Data は別コンポーネント |
| エディタ専用とランタイムを同名 API | エディタ専用は `CmpEditor*` または Editor 名前空間 |
| 汎用 `ResourceLoader` 1 本 | obj 用・png 用・シーン用など用途別 Data |

## 既存の分割例

- **描画:** `CmpMeshSphere`（組み込み球）／`CmpMesh`（モデル）／`CmpEditorTextureBillboard`（インポート画像プレビュー）
- **当たり:** `CmpHitSphere`（Upd）＋ `CmpDrawHitSphere`（Draw・可視化）
- **データ:** `CmpEditorAssetPath`（パス JSON・プレビュー連携）

## エディタへの追加手順

1. `EntityEditorDefinition.cpp` … `IsKnownEntityEditorComponent` / `DefaultParamsForType`
2. `EntityEditorUi.cpp` … `kKnownComponents` / `DrawComponentParams`
3. `EditorSceneRuntime.cpp` … 追加・パラメータ反映・プレビュー専用ロード
4. ランタイム本番で使う場合は `EntityFactory` またはシーンビルド側を別タスクで接続
5. `python3 EditorAssets/Tools/CursorAI/verify_editor_component_registration.py`（三箇所同期。CI の BuildCoverageCheck でも実行）

## 命名

- **Upd:** 振る舞い・シミュレーション（`CmpRotator`, `CmpScroll`）
- **Draw:** 描画のみ（ライト・マテリアルはコンポーネント内パラメータ）
- **Data:** パス・設定の保持（毎フレーム処理なし）

## 次の分割候補（エディタ登録済み・ランタイム配線は別タスク）

- **Data:** `CmpRuntimeTexturePath`（`texturePath` のみ。プレビュー用ロードは `CmpEditorAssetPath` 経路）
- **Data:** `CmpColliderAabb`（`localCenter` / `halfExtents` / `isTrigger`。当たり描画は `CmpDrawHitSphere` 等と併用）
