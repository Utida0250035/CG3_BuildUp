# Unity 風エンティティ方針（Atrum）

制作（エディタ）とランタイムの両方で共有する **用語と責務の切り方**。実装の正本は `Entity` / `ComponentUpd` / `ComponentDraw` / `ComponentData` と `EntityEditor`。

## 用語対応

| Unity 寄り | Atrum（現状） |
|------------|----------------|
| GameObject | `Entity` |
| Component | `ComponentUpd` / `ComponentDraw` / `ComponentData` |
| Transform | `Entity` の `TransformLH`（親子は `SetParent`） |
| Prefab | `EditorAssets/Entities/*.json` ＋ `AddInstanceFromPrefabPath` |
| Scene | `EditorSceneDocument`（`EditorAssets/Scenes`） |
| Tag | `Entity::SetTagsFromNames` / `tagHashes`（`tagRegistry.json`） |
| Script execution order | `executionOrder` → `SortComponents` / priority |

## 原則

1. **更新と描画を分ける** … ゲームロジックは `ComponentUpd`、描画は `ComponentDraw`。エディタ専用のパス記録などは `ComponentData`。
2. **「何にでも使える」1 基クラスに寄せない** … 役割ごとに小さいコンポーネントを増やす（roadmap: コンポーネント分割）。
3. **シーン定義は JSON** … ランタイムはコードで組み立てるか、将来シーン JSON からビルドする。エディタのプレビューは `EditorSceneRuntime` が定義から `Entity` を生成。
4. **エディタ専用コンポーネント** … 名前に `Editor` を含め、Release ビルドから切るか Data のみで保持し Draw/Upd に載せない。
5. **親子** … `parentIndex` でヒエラルキー。ワールド行列は `GetWorldMatrix`。位置のみ継承は `isInheritParentPositionOnly`。

## 制作フロー（エディタ）

1. シーン JSON を開く → インスタンス追加 → コンポーネント追加（Combo）。
2. 実行順・priority をインスペクタで編集 → プレビュー適用。
3. 必要ならプレハブ JSON 出力、または外部ファイルを Imported へドロップ → `CmpEditorAssetPath` でプレビュー。

## ランタイム（ゲームプレイ）

- `EntityFactory` / シーンコードで `Entity` を構築し、`Initialize` → 毎フレーム `Update` / `Draw`。
- 新規コンポーネントは **フレームコスト** と **Initialize 時のメモリ確保** を意識する。

## 今後の整理（未実装）

- シーン JSON から本番ビルドへの完全ロード
- ファクトリコンポーネント（プレハブ依存）
- AABB 等 Collider 拡張（設計確定後）
