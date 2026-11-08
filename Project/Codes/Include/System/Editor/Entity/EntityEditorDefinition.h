#pragma once

#include "Math/Transform.h"
#include <cstdint>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

namespace Atrum::EntityEditor {

/// コンポーネントの種別（Entity の追加先）
enum class ComponentSlotKind { Upd, Draw, Data };

/// 1 コンポーネント分の編集データ（型名＋初期パラメータ JSON）
struct ComponentSlot {
  ComponentSlotKind kind = ComponentSlotKind::Draw;
  std::string typeName = "CmpMeshSphere";
  nlohmann::json params = nlohmann::json::object();
};

/// エンティティ 1 体分の定義（初期トランスフォーム＋コンポーネント列）
struct EntityDefinition {
  std::string entityName = "entity_editor_preview";
  Math::TransformLH initialTransform{};
  /// タグ名（エディタ表示・改名用。保存時に tagHashes と同期）
  std::vector<std::string> tags{};
  /// 実行時 std::set<uint64_t> と同内容（JSON では昇順・重複なし）
  std::vector<uint64_t> tagHashes{};
  std::vector<ComponentSlot> components{};
};

/// シーン上の 1 インスタンス（ヒエラルキー表示名＋プレハブ相当の定義）
struct SceneEntityInstance {
  std::string instanceName = "Entity";
  /// -1 でルート。それ以外は instances 配列内のインデックス
  int parentIndex = -1;
  EntityDefinition definition{};
};

/// エディタシーン（複数オブジェクト）
struct EditorSceneDocument {
  std::string sceneName = "EditorScene";
  /// シーン全体で共有するタグ名一覧（エディタ検索・改名の基準）
  std::vector<std::string> tagCatalog{};
  std::vector<SceneEntityInstance> instances{};
};

/// 新規エンティティ用の既定定義（MeshSphere + Rotator）
EntityDefinition MakeDefaultEntityDefinition(const std::string &entityName);

/// 登録済みコンポーネント種別の既定パラメータ（JSON）
nlohmann::json DefaultParamsForType(const std::string &typeName);

void to_json(nlohmann::json &j, const ComponentSlot &slot);
void from_json(const nlohmann::json &j, ComponentSlot &slot);
void to_json(nlohmann::json &j, const EntityDefinition &definition);
void from_json(const nlohmann::json &j, EntityDefinition &definition);
void to_json(nlohmann::json &j, const SceneEntityInstance &instance);
void from_json(const nlohmann::json &j, SceneEntityInstance &instance);
void to_json(nlohmann::json &j, const EditorSceneDocument &document);
void from_json(const nlohmann::json &j, EditorSceneDocument &document);

} // namespace Atrum::EntityEditor
