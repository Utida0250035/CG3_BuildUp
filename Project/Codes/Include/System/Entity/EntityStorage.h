#pragma once

#include "System/Entity/Entity.h"

#include <cassert>
#include <memory>
#include <string>
#include <unordered_map>

namespace Atrum {

struct EntityHandle;

class EntityStorage final {

private:
  static std::unique_ptr<EntityStorage> instance_;

  EntityStorage() { entitys_.reserve(65536); };

public:

  ~EntityStorage() = default;

private:

  std::vector<std::unique_ptr<Entity>> entitys_{};

  std::unordered_map<uint64_t, uint64_t> hashToIdMap_{};
  std::unordered_map<uint64_t, size_t> idToIndexMap_{};

  std::vector<EntityHandle> eraseRequestedHandles_{};

  uint64_t nextId_ = 1;

  EntityHandle Find(const uint64_t id);

  void EraseRequested();

  void Erase(EntityHandle &handle);

  struct Token {
      explicit Token() = default;
  };

public:
  void Register(const std::string &name, std::unique_ptr<Entity> &&pEntity);

  EntityHandle Find(const std::string &name);

  bool IsValid(const EntityHandle &handle);

  void RequestErase(EntityHandle &handle);

  void Update();

  explicit EntityStorage(Token) {}

  static EntityStorage *Instance() {

    if (!instance_) {

      instance_ = std::make_unique<EntityStorage>(Token{});
    }

    return instance_.get();
  }

  static void Destroy() {

    if (instance_) {
        instance_.reset();
    }
  }
};

struct EntityHandle {

  Entity *ptr_ = nullptr;
  uint64_t id_ = 0;
  uint64_t nameHash_ = 0;

  bool IsValid() const { return EntityStorage::Instance()->IsValid(*this); }

  Entity *operator->() const {
    assert(ptr_);
    return ptr_;
  }
};

} // namespace Atrum