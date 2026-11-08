#pragma once

#include <algorithm>
#include <cassert>
#include <concepts>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "System/Hash64.h"
#include "Math/Transform.h"
#include "System/Entity/ComponentData.h"
#include "System/Entity/ComponentDraw.h"
#include "System/Entity/ComponentUpd.h"

namespace Atrum {

// コンテナとなるクラス
class Entity {
public:
  enum class State { Initialize, Sleep, Active, Destroy };

private:
  Entity *parent_ = nullptr;

  std::vector<Entity *> childs_{};

  uint64_t nameHash_ = 0;

  /// タグ名の FNV-1a ハッシュ列（検知は find で行う）
  std::set<uint64_t> tagHashes_{};

  uint64_t id_ = 0;

  struct UpdCmpBox {
    uint64_t priority = 0;

    ComponentUpd *component = nullptr;

    UpdCmpBox(uint64_t p, ComponentUpd *c) : priority(p), component(c) {}

    ComponentUpd *operator->() { return component; }
    operator bool() { return component; }
  };

  struct DrawCmpBox {
    uint64_t priority = 0;

    ComponentDraw *component = nullptr;

    DrawCmpBox(uint64_t p, ComponentDraw *c) : priority(p), component(c) {}

    ComponentDraw *operator->() { return component; }
    operator bool() { return component; }
  };

  Math::TransformLH transform_{};
  Math::TransformLH initialTransform_{};

  bool isInheritParentPositionOnly_ = false;

  std::vector<std::unique_ptr<ComponentUpd>> updComponents_{};
  std::vector<std::unique_ptr<ComponentDraw>> drawComponents_{};

  std::vector<UpdCmpBox> updCmpBoxes_{};
  std::vector<DrawCmpBox> drawCmpBoxes_{};

  std::vector<std::unique_ptr<ComponentData>> dataComponents_{};

  State state_ = State::Initialize;

public:
  Entity() = default;
  ~Entity();

  void Initialize();
  void Update();
  void Draw();
  void Finalize();

  template <typename T> UpdCmpBox &RefUpdCmpBox() {
    for (const auto &cmpBox : updCmpBoxes_) {
      if (auto p = dynamic_cast<T *>(cmpBox.component)) {
        return cmpBox;
      }
    }

    assert(false && "UpdCmpBox not found, can't get");

    return nullptr;
  }

  template <typename T> T *GetUpdCmp() {
    for (const auto &component : updComponents_) {
      if (auto p = dynamic_cast<T *>(component.get())) {
        return p;
      }
    }

    assert(false && "UpdCmp not found, can't get");

    return nullptr;
  }

  template <typename T> bool HasUpdCmp() const {
    for (const auto &component : updComponents_) {
      if (auto p = dynamic_cast<T *>(component.get())) {
        return true;
      }
    }

    return false;
  }

  template <typename T> void AddUpdCmp() {
    if (HasUpdCmp<T>()) {
      assert(false && "ComponentUpd is already added to entity");

      return;
    }

    std::unique_ptr<ComponentUpd> component = std::make_unique<T>();

    AttachUpdComponent(std::move(component));
  }

  template <typename T> DrawCmpBox &RefDrawCmpBox() {
    for (const auto &cmpBox : drawCmpBoxes_) {
      if (auto p = dynamic_cast<T *>(cmpBox.component)) {
        return cmpBox;
      }
    }

    assert(false && "DrawCmpBox not found, can't get");

    return nullptr;
  }

  template <typename T> T *GetDrawCmp() {
    for (const auto &component : drawComponents_) {
      if (auto p = dynamic_cast<T *>(component.get())) {
        return p;
      }
    }

    assert(false && "DrawCmp not found, can't get");

    return nullptr;
  }

  template <typename T> bool HasDrawCmp() const {
    for (const auto &component : drawComponents_) {
      if (auto p = dynamic_cast<T *>(component.get())) {
        return true;
      }
    }

    return false;
  }

  template <typename T> void AddDrawCmp() {
    if (HasDrawCmp<T>()) {
      assert(false && "ComponentDraw is already added to entity");

      return;
    }

    std::unique_ptr<ComponentDraw> component = std::make_unique<T>();

    AttachDrawComponent(std::move(component));
  }

  template <typename T> T *GetDataCmp() {
    for (const auto &component : dataComponents_) {
      if (auto p = dynamic_cast<T *>(component.get())) {
        return p;
      }
    }

    assert(false && "DataCmp not found, can't get");

    return nullptr;
  }

  template <typename T> bool HasDataCmp() const {
    for (const auto &component : dataComponents_) {
      if (auto p = dynamic_cast<T *>(component.get())) {
        return true;
      }
    }

    return false;
  }

  template <typename T> void AddDataCmp() {
    if (HasDataCmp<T>()) {
      assert(false && "ComponentData is already added to entity");

      return;
    }

    std::unique_ptr<ComponentData> component = std::make_unique<T>();

    component->SetOwner(this);

    dataComponents_.emplace_back(std::move(component));
  }

  void ResolveDependencies();

  /// Upd／Draw コンポーネントボックスを優先度昇順に並べ替える
  void SortComponents();

  void SetUpdCmpPriority(ComponentUpd *component, uint64_t priority);
  void SetDrawCmpPriority(ComponentDraw *component, uint64_t priority);

  explicit Entity(const std::string &name) : nameHash_(Hash64(name)) {}

private:
  void AttachUpdComponent(std::unique_ptr<ComponentUpd> component);
  void AttachDrawComponent(std::unique_ptr<ComponentDraw> component);
  void SortUpdCmpBoxes();
  void SortDrawCmpBoxes();

public:

  Math::TransformLH &RefTransform() { return transform_; }
  const Math::TransformLH &GetTransform() { return transform_; }

  Math::TransformLH GetInitialTransform() { return initialTransform_; }
  void SetInitialTransform(const Atrum::Math::TransformLH &initialTransform) {
    initialTransform_ = initialTransform;
  }

  uint64_t GetHash() const { return nameHash_; }

  uint64_t GetId() const { return id_; }

  State GetState() const { return state_; }

  void SetState(const State &state) { state_ = state; }

  Math::Matrix4x4 GetWorldMatrix() const;
  Math::Vector3 GetWorldPosition() const;

  /// ローカル座標点をワールド座標へ変換する
  Math::Vector3 TransformLocalPointToWorld(const Math::Vector3 &localPoint) const;

  bool IsActive() const { return state_ == State::Active; }

  void RemoveChild(Entity *target) { std::erase(childs_, target); }

  void SetParent(Entity *newParent);
  const Entity *GetParent() { return parent_; };
  const std::vector<Entity *> &GetChilds() const { return childs_; };

  void SetHash(const uint64_t nameHash) { nameHash_ = nameHash; }

  void SetHash(const std::string &name) { nameHash_ = Hash64(name); }

  void SetId(const uint64_t id) { id_ = id; }

  void RequestDestroy() { state_ = State::Destroy; }

  void SetIsInheritParentPositionOnly(const bool isInheritPositionOnly) {
    isInheritParentPositionOnly_ = isInheritPositionOnly;
  }

  /// タグ名を Hash64 で登録する
  void AddTag(const std::string &tagName);
  /// タグハッシュを登録する
  void AddTagHash(const uint64_t tagHash) { tagHashes_.insert(tagHash); }
  /// タグ名のハッシュで削除する
  void RemoveTag(const std::string &tagName);
  void RemoveTagHash(const uint64_t tagHash) { tagHashes_.erase(tagHash); }
  void ClearTags() { tagHashes_.clear(); }

  /// タグ名を Hash64 に変換して保持集合を find する
  bool HasTag(const std::string &tagName) const;
  bool HasTagHash(const uint64_t tagHash) const {
    return tagHashes_.find(tagHash) != tagHashes_.end();
  }

  const std::set<uint64_t> &GetTagHashes() const { return tagHashes_; }

  /// エディタ定義どおりタグ名列からタグ集合を再構築する
  void SetTagsFromNames(const std::vector<std::string> &tagNames);

  /// JSON の tagHashes 列をそのまま std::set に反映する
  void SetTagHashes(const std::vector<uint64_t> &tagHashes);
};

} // namespace Atrum