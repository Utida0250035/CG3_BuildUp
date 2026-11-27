#include "System/Scripting/EntityLuaBinder.h"

#include "Math/Quaternion.h"
#include "Math/Vector3.h"
#include "System/AtomF/AtomFSceneEntityAdoptQueue.h"
#include "System/Editor/Entity/EntityEditorDefinition.h"
#include "System/Editor/Entity/EntityEditorKnownComponentRegistry.h"
#include "System/Editor/Scene/EditorSceneRuntime.h"
#include "System/Editor/Scene/EditorSceneRuntimeEntityHasComponentForSlot.h"
#include "System/Entity/Components/CmpHealthHit.h"
#include "System/Entity/Entity.h"
#include "System/Entity/EntityTagRegistry.h"
#include "System/Hash64.h"
#include "System/Scripting/LuaSystem.h"
#include "System/Scripting/ScriptSceneContext.h"

#include <array>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_set>
#include <vector>

namespace Atrum::Script {

namespace {

using Math::Quaternion;
using Math::Vector3;
using EntityList = std::vector<std::unique_ptr<Entity>>;

// ---------------------------------------------------------------------------
// 有効性判定
// ---------------------------------------------------------------------------

/// spawn 直後でまだシーン所有リストへ採用されていないエンティティ。
/// 採用（FlushSceneEntityAdoptQueue）後にシーン走査でヒットした時点で外す。
std::unordered_set<const Entity *> &PendingSpawned() {
  static std::unordered_set<const Entity *> pending;
  return pending;
}

bool ContainsRecursive(const Entity *root, const Entity *target) {
  if (root == target) {
    return true;
  }
  for (const Entity *child : root->GetChilds()) {
    if (child != nullptr && ContainsRecursive(child, target)) {
      return true;
    }
  }
  return false;
}

/// 解放済みポインタを触らないよう、シーン所有リスト（と子孫・採用待ち）に居るかだけで判定する。
bool IsAlive(const LuaEntityHandle &handle) {
  if (handle.ptr == nullptr) {
    return false;
  }
  if (const EntityList *const entities = SceneContext::Entities()) {
    for (const std::unique_ptr<Entity> &entity : *entities) {
      if (entity != nullptr && ContainsRecursive(entity.get(), handle.ptr)) {
        PendingSpawned().erase(handle.ptr);
        return true;
      }
    }
  }
  return PendingSpawned().count(handle.ptr) != 0;
}

Entity *Live(const LuaEntityHandle &handle) {
  return IsAlive(handle) ? handle.ptr : nullptr;
}

// ---------------------------------------------------------------------------
// 検索
// ---------------------------------------------------------------------------

template <typename Pred> std::vector<LuaEntityHandle> Collect(Pred pred) {
  std::vector<LuaEntityHandle> result;
  const EntityList *const entities = SceneContext::Entities();
  if (entities == nullptr) {
    return result;
  }
  for (const std::unique_ptr<Entity> &entity : *entities) {
    if (entity != nullptr && pred(*entity)) {
      result.push_back({entity.get()});
    }
  }
  return result;
}

sol::optional<LuaEntityHandle> FirstOrNil(std::vector<LuaEntityHandle> found) {
  if (found.empty()) {
    return sol::nullopt;
  }
  return found.front();
}

std::vector<LuaEntityHandle> FindAllByName(const std::string &name) {
  const uint64_t hash = Hash64(name);
  return Collect([hash](const Entity &e) { return e.GetHash() == hash; });
}

std::vector<LuaEntityHandle> FindAllByTag(const std::string &tag) {
  return Collect([&tag](const Entity &e) { return e.HasTag(tag); });
}

std::vector<LuaEntityHandle> FindAllByRole(const std::string &role) {
  EntityTagRegistry &tags = EntityTagRegistry::Instance();
  tags.EnsureLoaded();
  return Collect(
      [&tags, &role](const Entity &e) { return tags.EntityHasRole(e, role); });
}

// ---------------------------------------------------------------------------
// Lua テーブル -> JSON（コンポーネントパラメータの上書き用）
// ---------------------------------------------------------------------------

nlohmann::json ToJson(const sol::object &object, const int depth) {
  if (depth > 16) {
    return nullptr;
  }
  switch (object.get_type()) {
  case sol::type::boolean:
    return object.as<bool>();
  case sol::type::number: {
    const double value = object.as<double>();
    if (std::floor(value) == value && std::fabs(value) < 9.0e15) {
      return static_cast<int64_t>(value);
    }
    return value;
  }
  case sol::type::string:
    return object.as<std::string>();
  case sol::type::table: {
    const sol::table table = object.as<sol::table>();
    size_t count = 0;
    for (const auto &kv : table) {
      (void)kv;
      ++count;
    }
    if (count > 0 && table.size() == count) {
      nlohmann::json array = nlohmann::json::array();
      for (size_t i = 1; i <= count; ++i) {
        const sol::object element = table[i];
        array.push_back(ToJson(element, depth + 1));
      }
      return array;
    }
    nlohmann::json obj = nlohmann::json::object();
    for (const auto &kv : table) {
      if (kv.first.get_type() == sol::type::string) {
        obj[kv.first.as<std::string>()] = ToJson(kv.second, depth + 1);
      }
    }
    return obj;
  }
  default:
    return nullptr;
  }
}

// ---------------------------------------------------------------------------
// 生成
// ---------------------------------------------------------------------------

std::optional<EntityEditor::SceneEntityInstance>
LoadInstanceFromJsonFile(const std::string &path) {
  namespace fs = std::filesystem;
  const std::array<std::string, 3> candidates = {
      path, "./" + path, "../Project/" + path};
  for (const std::string &candidate : candidates) {
    if (!fs::exists(candidate)) {
      continue;
    }
    std::ifstream file(candidate);
    if (!file.is_open()) {
      continue;
    }
    try {
      nlohmann::json json{};
      file >> json;
      if (json.contains("entities") && json.at("entities").is_array()) {
        const auto document = json.get<EntityEditor::EditorSceneDocument>();
        if (document.instances.empty()) {
          return std::nullopt;
        }
        return document.instances.front();
      }
      if (json.contains("definition")) {
        return json.get<EntityEditor::SceneEntityInstance>();
      }
      EntityEditor::SceneEntityInstance instance{};
      instance.definition = json.get<EntityEditor::EntityDefinition>();
      instance.instanceName = instance.definition.entityName;
      return instance;
    } catch (const std::exception &e) {
      std::cerr << "[EntityLuaBinder] spawn: json parse failed: " << candidate
                << "\n"
                << e.what() << "\n";
      return std::nullopt;
    }
  }
  std::cerr << "[EntityLuaBinder] spawn: file not found: " << path << "\n";
  return std::nullopt;
}

void ApplyComponentOverrides(EntityEditor::SceneEntityInstance &instance,
                             const sol::table &overrides) {
  for (const auto &kv : overrides) {
    if (kv.first.get_type() != sol::type::string) {
      continue;
    }
    const std::string typeName = kv.first.as<std::string>();
    const nlohmann::json patch = ToJson(kv.second, 0);

    bool found = false;
    for (EntityEditor::ComponentSlot &slot : instance.definition.components) {
      if (slot.typeName == typeName) {
        if (patch.is_object()) {
          slot.params.merge_patch(patch);
        }
        found = true;
        break;
      }
    }
    if (found) {
      continue;
    }
    if (!EntityEditor::IsKnownEditorRuntimeComponentTypeName(typeName)) {
      std::cerr << "[EntityLuaBinder] spawn: unknown component: " << typeName
                << "\n";
      continue;
    }
    EntityEditor::ComponentSlot slot{};
    slot.typeName = typeName;
    slot.kind = EntityEditor::DefaultKindForTypeName(typeName);
    slot.params = EntityEditor::DefaultParamsForType(typeName);
    if (patch.is_object()) {
      slot.params.merge_patch(patch);
    }
    instance.definition.components.push_back(std::move(slot));
  }
}

/// opts: { name=, x=,y=,z=（ローカル位置）, position=Vec3, rotation=Quat, scale=Vec3,
///         parent=Entity, tags={...}, components={ CmpXxx={...}, ... } }
sol::optional<LuaEntityHandle>
SpawnFromPrefab(const std::string &prefabPath,
                const sol::optional<sol::table> options) {
  if (SceneContext::Entities() == nullptr) {
    std::cerr << "[EntityLuaBinder] spawn: no active scene\n";
    return sol::nullopt;
  }
  std::optional<EntityEditor::SceneEntityInstance> loaded =
      LoadInstanceFromJsonFile(prefabPath);
  if (!loaded) {
    return sol::nullopt;
  }
  EntityEditor::SceneEntityInstance instance = std::move(*loaded);
  instance.parentIndex = -1;

  Entity *parent = nullptr;
  std::vector<std::string> extraTags;
  if (options) {
    const sol::table &t = *options;
    Math::TransformLH &transform = instance.definition.initialTransform;

    if (const sol::optional<std::string> name = t["name"]) {
      instance.instanceName = *name;
    }
    if (const sol::optional<Vector3> position = t["position"]) {
      transform.translate = *position;
    }
    transform.translate.x = t.get_or("x", transform.translate.x);
    transform.translate.y = t.get_or("y", transform.translate.y);
    transform.translate.z = t.get_or("z", transform.translate.z);
    if (const sol::optional<Quaternion> rotation = t["rotation"]) {
      transform.quaternion = *rotation;
    }
    if (const sol::optional<Vector3> scale = t["scale"]) {
      transform.scale = *scale;
    }
    if (const sol::optional<sol::table> tags = t["tags"]) {
      for (const auto &kv : *tags) {
        if (kv.second.get_type() == sol::type::string) {
          extraTags.push_back(kv.second.as<std::string>());
        }
      }
    }
    if (const sol::optional<LuaEntityHandle> parentHandle = t["parent"]) {
      parent = Live(*parentHandle);
    }
    if (const sol::optional<sol::table> components = t["components"]) {
      ApplyComponentOverrides(instance, *components);
    }
  }

  std::unique_ptr<Entity> entity =
      EntityEditor::BuildEntityForRuntimeSpawn(instance);
  if (entity == nullptr) {
    return sol::nullopt;
  }
  for (const std::string &tag : extraTags) {
    entity->AddTag(tag);
  }
  if (parent != nullptr) {
    entity->SetParent(parent);
  }
  const LuaEntityHandle handle{entity.get()};
  PendingSpawned().insert(handle.ptr);
  // 更新ループ中に呼ばれうるため、走査後にシーンへ採用される（SceneBuildUp::Update）。
  AtomF::EnqueueSceneEntityAdopt(std::move(entity));
  return handle;
}

// ---------------------------------------------------------------------------
// Entity メンバ
// ---------------------------------------------------------------------------

const char *StateName(const Entity::State state) {
  switch (state) {
  case Entity::State::Initialize:
    return "initialize";
  case Entity::State::Sleep:
    return "sleep";
  case Entity::State::Active:
    return "active";
  case Entity::State::Destroy:
    return "destroy";
  }
  return "unknown";
}

bool HasComponentNamed(const Entity &entity, const std::string &typeName) {
  using EntityEditor::ComponentSlot;
  using EntityEditor::ComponentSlotKind;
  for (const ComponentSlotKind kind :
       {ComponentSlotKind::Upd, ComponentSlotKind::Draw,
        ComponentSlotKind::Data}) {
    ComponentSlot slot{};
    slot.kind = kind;
    slot.typeName = typeName;
    if (EntityEditor::EntityHasComponentForSlot(entity, slot)) {
      return true;
    }
  }
  return false;
}

std::vector<std::string> ComponentNamesOf(const Entity &entity) {
  std::vector<std::string> names;
  for (const std::string_view type :
       EntityEditor::EntityEditorKnownComponentRegistry::TypeNames()) {
    const std::string name(type);
    if (HasComponentNamed(entity, name)) {
      names.push_back(name);
    }
  }
  return names;
}

std::vector<std::string> TagNamesOf(const Entity &entity) {
  EntityTagRegistry &tags = EntityTagRegistry::Instance();
  tags.EnsureLoaded();
  std::vector<std::string> names;
  for (const std::string &tag : tags.TagCatalog()) {
    if (entity.HasTag(tag)) {
      names.push_back(tag);
    }
  }
  return names;
}

std::vector<LuaEntityHandle> ChildrenOf(const Entity &entity) {
  std::vector<LuaEntityHandle> result;
  for (Entity *child : entity.GetChilds()) {
    if (child != nullptr) {
      result.push_back({child});
    }
  }
  return result;
}

CmpHealthHit *HealthOf(Entity &entity) {
  return entity.HasUpdCmp<CmpHealthHit>() ? entity.GetUpdCmp<CmpHealthHit>()
                                          : nullptr;
}

void RegisterMathTypes(sol::state &lua) {
  lua.new_usertype<Vector3>(
      "Vec3", sol::call_constructor,
      sol::factories([]() { return Vector3{0.0f, 0.0f, 0.0f}; },
                     [](const float x, const float y, const float z) {
                       return Vector3{x, y, z};
                     }),
      "x", &Vector3::x, "y", &Vector3::y, "z", &Vector3::z, "length",
      [](const Vector3 &v) { return v.Length(); }, "normalized",
      [](const Vector3 &v) { return v.Normalized(); }, "dot",
      [](const Vector3 &a, const Vector3 &b) {
        return a.x * b.x + a.y * b.y + a.z * b.z;
      },
      sol::meta_function::addition,
      [](const Vector3 &a, const Vector3 &b) { return a + b; },
      sol::meta_function::subtraction,
      [](const Vector3 &a, const Vector3 &b) { return a - b; },
      sol::meta_function::multiplication,
      [](const Vector3 &a, const float s) { return a * s; },
      sol::meta_function::unary_minus, [](const Vector3 &a) { return -a; },
      sol::meta_function::to_string, [](const Vector3 &v) {
        return "Vec3(" + std::to_string(v.x) + ", " + std::to_string(v.y) +
               ", " + std::to_string(v.z) + ")";
      });

  lua.new_usertype<Quaternion>(
      "Quat", sol::call_constructor,
      sol::factories([]() { return Quaternion::Identity(); },
                     [](const float x, const float y, const float z,
                        const float w) {
                       Quaternion q{};
                       q.x = x;
                       q.y = y;
                       q.z = z;
                       q.w = w;
                       return q;
                     }),
      "x", &Quaternion::x, "y", &Quaternion::y, "z", &Quaternion::z, "w",
      &Quaternion::w, "fromEuler",
      [](const Vector3 &euler) { return Quaternion::FromEulerXYZ(euler); },
      "fromAxisAngle",
      [](const Vector3 &axis, const float angle) {
        return Quaternion::FromAxisAngle(axis.Normalized(), angle);
      },
      "lookRotation",
      [](const Vector3 &forward) {
        return Quaternion::FromLhLookAt(forward, Vector3::UpLh());
      },
      "slerp",
      [](const Quaternion &a, const Quaternion &b, const float t) {
        return Quaternion::Slerp(a, b, t);
      });
}

void RegisterEntityType(sol::state &lua) {
  lua.new_usertype<LuaEntityHandle>(
      "Entity", sol::no_constructor,
      // --- 状態 ---
      "isValid", [](const LuaEntityHandle &h) { return IsAlive(h); }, "state",
      [](const LuaEntityHandle &h) -> std::string {
        const Entity *e = Live(h);
        return e != nullptr ? StateName(e->GetState()) : "invalid";
      },
      "isActive",
      [](const LuaEntityHandle &h) {
        const Entity *e = Live(h);
        return e != nullptr && e->IsActive();
      },
      "id",
      [](const LuaEntityHandle &h) -> double {
        const Entity *e = Live(h);
        return e != nullptr ? static_cast<double>(e->GetId()) : 0.0;
      },
      "isNamed",
      [](const LuaEntityHandle &h, const std::string &name) {
        const Entity *e = Live(h);
        return e != nullptr && e->GetHash() == Hash64(name);
      },
      "destroy",
      [](const LuaEntityHandle &h) {
        if (Entity *e = Live(h)) {
          e->RequestDestroy();
        }
      },
      // --- トランスフォーム（ローカル）---
      "getPosition",
      [](const LuaEntityHandle &h) {
        Entity *e = Live(h);
        return e != nullptr ? e->RefTransform().translate : Vector3::Zero();
      },
      "setPosition",
      [](const LuaEntityHandle &h, const Vector3 &p) {
        if (Entity *e = Live(h)) {
          e->RefTransform().translate = p;
        }
      },
      "translate",
      [](const LuaEntityHandle &h, const Vector3 &d) {
        if (Entity *e = Live(h)) {
          e->RefTransform().translate += d;
        }
      },
      "getScale",
      [](const LuaEntityHandle &h) {
        Entity *e = Live(h);
        return e != nullptr ? e->RefTransform().scale : Vector3::One();
      },
      "setScale",
      [](const LuaEntityHandle &h, const Vector3 &s) {
        if (Entity *e = Live(h)) {
          e->RefTransform().scale = s;
        }
      },
      "getRotation",
      [](const LuaEntityHandle &h) {
        Entity *e = Live(h);
        return e != nullptr ? e->RefTransform().quaternion
                            : Quaternion::Identity();
      },
      "setRotation",
      [](const LuaEntityHandle &h, const Quaternion &q) {
        if (Entity *e = Live(h)) {
          e->RefTransform().quaternion = q;
        }
      },
      "setRotationEuler",
      [](const LuaEntityHandle &h, const Vector3 &euler) {
        if (Entity *e = Live(h)) {
          e->RefTransform().quaternion = Quaternion::FromEulerXYZ(euler);
        }
      },
      "lookDirection",
      [](const LuaEntityHandle &h, const Vector3 &forward) {
        if (Entity *e = Live(h)) {
          e->RefTransform().quaternion =
              Quaternion::FromLhLookAt(forward, Vector3::UpLh());
        }
      },
      // --- ワールド座標 ---
      "getWorldPosition",
      [](const LuaEntityHandle &h) {
        const Entity *e = Live(h);
        return e != nullptr ? e->GetWorldPosition() : Vector3::Zero();
      },
      "localToWorld",
      [](const LuaEntityHandle &h, const Vector3 &local) {
        const Entity *e = Live(h);
        return e != nullptr ? e->TransformLocalPointToWorld(local) : local;
      },
      // --- 親子 ---
      "getParent",
      [](const LuaEntityHandle &h) -> sol::optional<LuaEntityHandle> {
        Entity *e = Live(h);
        if (e == nullptr || e->GetParent() == nullptr) {
          return sol::nullopt;
        }
        return LuaEntityHandle{const_cast<Entity *>(e->GetParent())};
      },
      "setParent",
      [](const LuaEntityHandle &h, const sol::optional<LuaEntityHandle> parent) {
        Entity *e = Live(h);
        if (e == nullptr) {
          return;
        }
        Entity *newParent = parent ? Live(*parent) : nullptr;
        // 自分の子孫を親にすると循環するので拒否する
        if (newParent != nullptr && ContainsRecursive(e, newParent)) {
          return;
        }
        e->SetParent(newParent);
      },
      "children",
      [](const LuaEntityHandle &h) {
        const Entity *e = Live(h);
        return sol::as_table(e != nullptr ? ChildrenOf(*e)
                                          : std::vector<LuaEntityHandle>{});
      },
      // --- タグ ---
      "hasTag",
      [](const LuaEntityHandle &h, const std::string &tag) {
        const Entity *e = Live(h);
        return e != nullptr && e->HasTag(tag);
      },
      "addTag",
      [](const LuaEntityHandle &h, const std::string &tag) {
        if (Entity *e = Live(h)) {
          e->AddTag(tag);
        }
      },
      "removeTag",
      [](const LuaEntityHandle &h, const std::string &tag) {
        if (Entity *e = Live(h)) {
          e->RemoveTag(tag);
        }
      },
      "tags",
      [](const LuaEntityHandle &h) {
        const Entity *e = Live(h);
        return sol::as_table(e != nullptr ? TagNamesOf(*e)
                                          : std::vector<std::string>{});
      },
      "hasRole",
      [](const LuaEntityHandle &h, const std::string &role) {
        const Entity *e = Live(h);
        if (e == nullptr) {
          return false;
        }
        EntityTagRegistry &tags = EntityTagRegistry::Instance();
        tags.EnsureLoaded();
        return tags.EntityHasRole(*e, role);
      },
      // --- コンポーネント ---
      "hasComponent",
      [](const LuaEntityHandle &h, const std::string &typeName) {
        const Entity *e = Live(h);
        return e != nullptr && HasComponentNamed(*e, typeName);
      },
      "components",
      [](const LuaEntityHandle &h) {
        const Entity *e = Live(h);
        return sol::as_table(e != nullptr ? ComponentNamesOf(*e)
                                          : std::vector<std::string>{});
      },
      // --- CmpHealthHit ---
      "hasHealth",
      [](const LuaEntityHandle &h) {
        Entity *e = Live(h);
        return e != nullptr && HealthOf(*e) != nullptr;
      },
      "getHealth",
      [](const LuaEntityHandle &h) -> sol::optional<int> {
        Entity *e = Live(h);
        const CmpHealthHit *health = e != nullptr ? HealthOf(*e) : nullptr;
        if (health == nullptr) {
          return sol::nullopt;
        }
        return static_cast<int>(health->GetHealthCount());
      },
      "getMaxHealth",
      [](const LuaEntityHandle &h) -> sol::optional<int> {
        Entity *e = Live(h);
        const CmpHealthHit *health = e != nullptr ? HealthOf(*e) : nullptr;
        if (health == nullptr) {
          return sol::nullopt;
        }
        return static_cast<int>(health->GetInitialHealthCount());
      },
      "setHealth",
      [](const LuaEntityHandle &h, const int value) {
        Entity *e = Live(h);
        if (CmpHealthHit *health = e != nullptr ? HealthOf(*e) : nullptr) {
          health->SetHealthCount(static_cast<uint32_t>(value < 0 ? 0 : value));
        }
      },
      "damage",
      [](const LuaEntityHandle &h) {
        Entity *e = Live(h);
        CmpHealthHit *health = e != nullptr ? HealthOf(*e) : nullptr;
        return health != nullptr && health->TryTakeDamageUnconditional();
      },
      "isInvincible",
      [](const LuaEntityHandle &h) {
        Entity *e = Live(h);
        const CmpHealthHit *health = e != nullptr ? HealthOf(*e) : nullptr;
        return health != nullptr && health->IsInvincible();
      },
      sol::meta_function::equal_to,
      [](const LuaEntityHandle &a, const LuaEntityHandle &b) {
        return a.ptr == b.ptr;
      });
}

} // namespace

LuaEntityHandle MakeEntityHandle(Entity *const entity,
                                 const bool mayBeUnadopted) {
  if (entity != nullptr && mayBeUnadopted) {
    std::unordered_set<const Entity *> &pending = PendingSpawned();
    // 採用されないまま消えた実体のポインタが溜まり続けないよう上限で掃除する
    constexpr size_t kMaxPending = 1024;
    if (pending.size() >= kMaxPending) {
      pending.clear();
    }
    pending.insert(entity);
  }
  return LuaEntityHandle{entity};
}

void EntityLuaBinder::Register(sol::state &lua) {
  RegisterMathTypes(lua);
  RegisterEntityType(lua);

  sol::table entities = lua.create_named_table("Entities");
  entities.set_function("find", [](const std::string &name) {
    return FirstOrNil(FindAllByName(name));
  });
  entities.set_function("findAll", [](const std::string &name) {
    return sol::as_table(FindAllByName(name));
  });
  entities.set_function("findByTag", [](const std::string &tag) {
    return sol::as_table(FindAllByTag(tag));
  });
  entities.set_function("findFirstByTag", [](const std::string &tag) {
    return FirstOrNil(FindAllByTag(tag));
  });
  entities.set_function("findByRole", [](const std::string &role) {
    return sol::as_table(FindAllByRole(role));
  });
  entities.set_function("findFirstByRole", [](const std::string &role) {
    return FirstOrNil(FindAllByRole(role));
  });
  entities.set_function("all", []() {
    return sol::as_table(Collect([](const Entity &) { return true; }));
  });
  entities.set_function("count", []() {
    return static_cast<int>(Collect([](const Entity &e) {
                              return e.IsActive();
                            }).size());
  });
  entities.set_function("spawn", &SpawnFromPrefab);
  entities.set_function("destroy", [](const LuaEntityHandle &h) {
    if (Entity *e = Live(h)) {
      e->RequestDestroy();
    }
  });
  entities.set_function("componentTypes", []() {
    std::vector<std::string> names;
    for (const std::string_view type :
         EntityEditor::EntityEditorKnownComponentRegistry::TypeNames()) {
      names.emplace_back(type);
    }
    return sol::as_table(names);
  });
  entities.set_function("tagCatalog", []() {
    EntityTagRegistry &tags = EntityTagRegistry::Instance();
    tags.EnsureLoaded();
    return sol::as_table(tags.TagCatalog());
  });
}

} // namespace Atrum::Script
