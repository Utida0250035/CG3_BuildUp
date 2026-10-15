#pragma once
#include "Hash/Hash64.h"
#include "System/Entity/ComponentData.h"
#include "System/Entity/ComponentDraw.h"
#include "System/Entity/ComponentUpd.h"
#include "System/Entity/Entity.h"
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

namespace Atrum {

class RegistryCmp final {
private:
  using MyRegistry = std::function<void(Entity *)>;

  std::unordered_map<uint64_t, MyRegistry> registryCmpData_{};
  std::unordered_map<uint64_t, MyRegistry> registryCmpDraw_{};
  std::unordered_map<uint64_t, MyRegistry> registryCmpUpd_{};

  std::unordered_map<uint64_t, std::string> hashToNameMap_{};

  RegistryCmp() = default;
  ~RegistryCmp() = default;

public:
  static RegistryCmp *Instance() {
    static RegistryCmp instance;
    return &instance;
  }

  RegistryCmp(const RegistryCmp &source) = delete;
  RegistryCmp operator=(const RegistryCmp &source) = delete;

  template <typename T> void RegisterUpd(const std::string name) {
    registryCmpUpd_[Hash64(name)] = [](Entity *entity) {
      entity->AddUpdCmp<T>();
    };

    hashToNameMap_[Hash64(name)] = name;
  }

  template <typename T> void RegisterDraw(const std::string name) {
    registryCmpDraw_[Hash64(name)] = [](Entity *entity) {
      entity->AddDrawCmp<T>();
    };

    hashToNameMap_[Hash64(name)] = name;
  }

  template <typename T> void RegisterData(const std::string name) {
    registryCmpData_[Hash64(name)] = [](Entity *entity) {
      entity->AddDataCmp<T>();
    };

    hashToNameMap_[Hash64(name)] = name;
  }

  void GiveCmpUpd(const std::string cmpName, Entity *entity) {
    auto it = registryCmpUpd_.find(Hash64(cmpName));
    if (it != registryCmpUpd_.end()) {
      it->second(entity);
    }
  }

  void GiveCmpDraw(const std::string cmpName, Entity *entity) {
    auto it = registryCmpDraw_.find(Hash64(cmpName));
    if (it != registryCmpDraw_.end()) {
      it->second(entity);
    }
  }

  void GiveCmpData(const std::string cmpName, Entity *entity) {
    auto it = registryCmpData_.find(Hash64(cmpName));
    if (it != registryCmpData_.end()) {
      it->second(entity);
    }
  }

  const auto &GetRegistryCmpUpd() const { return registryCmpUpd_; }
  const auto &GetRegistryCmpDraw() const { return registryCmpDraw_; }
  const auto &GetRegistryCmpData() const { return registryCmpData_; }
};

} // namespace Atrum