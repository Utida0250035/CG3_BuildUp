#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace Atrum {

class AtrumEngine;
class Camera;

namespace OperatorGuidance {

struct HintDefinition {
  std::string id;
  std::string actionId;
  std::string message;
  std::vector<std::string> bodyTexturePaths;
  float idleBeforeShowSeconds = 15.0f;
  float displayDurationSeconds = 6.0f;
  float anchorX = 0.75f;
  float anchorY = 0.32f;
  float bodyScale = 0.42f;
};

class System {
public:
  static System &Instance();

  bool LoadFromJsonFile(const std::string &path);
  void ResetSession();

  void NotifyAction(const std::string &actionId);
  void Update(float deltaSeconds);
  void Draw(AtrumEngine *engine, Camera *camera) const;

  const std::string &ActiveMessage() const { return activeMessage_; }

private:
  std::vector<HintDefinition> hints_{};
  std::unordered_map<std::string, float> secondsSinceLastAction_{};
  std::string activeHintId_{};
  std::string activeMessage_{};
  float activeDisplayRemaining_ = 0.0f;
  mutable std::unordered_map<std::string, uint32_t> textureCache_{};
};

} // namespace OperatorGuidance
} // namespace Atrum
