#pragma once
#include "Math/Transform.h"
#include <string>
#include <vector>

namespace Atrum {

class InspectorEntity {
private:
  InspectorEntity() = default;
  ~InspectorEntity() = default;

  struct EntityInfo {
    std::string entityName = "";
    Math::TransformLH transform{};
    std::vector<std::string> componentUpdNames{};
    std::vector<std::string> componentDrawNames{};
    std::vector<std::string> componentDataNames{};
  };

public:
  static InspectorEntity *Instance() {
    static InspectorEntity instance;
    return &instance;
  }

  void Initialize();
  void Update();
  void Draw();

  InspectorEntity(const InspectorEntity &source) = delete;
  InspectorEntity operator=(const InspectorEntity &source) = delete;
};

} // namespace Atrum