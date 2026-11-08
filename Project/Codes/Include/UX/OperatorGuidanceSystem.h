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
  bool showOnBattleReady = false;
};

struct NavigatorProfile {
  std::string displayName = "ナビ";
  std::vector<std::string> portraitTexturePaths;
  float portraitAnchorX = 0.1f;
  float portraitAnchorY = 0.78f;
  float portraitScale = 0.28f;
};

struct TutorialReplacementConfig {
  bool skipIntroHowToOverlay = false;
  bool replaceWithNavigatorHints = true;
  std::string welcomeHintId;
};

class System {
public:
  static System &Instance();

  bool LoadFromJsonFile(const std::string &path);
  void ResetSession();

  void NotifyAction(const std::string &actionId);
  void OnBattleReady();
  void Update(float deltaSeconds);
  void Draw(AtrumEngine *engine, Camera *camera) const;

  const std::string &ActiveMessage() const { return activeMessage_; }
  bool ShouldSkipIntroHowToOverlay() const {
    return tutorialReplacement_.skipIntroHowToOverlay;
  }
  const NavigatorProfile &GetNavigator() const { return navigator_; }

private:
  void ShowHintById(const std::string &hintId);
  void DrawNavigatorPortrait(AtrumEngine *engine, Camera *camera) const;

  std::vector<HintDefinition> hints_{};
  std::unordered_map<std::string, float> secondsSinceLastAction_{};
  NavigatorProfile navigator_{};
  TutorialReplacementConfig tutorialReplacement_{};
  std::string activeHintId_{};
  std::string activeMessage_{};
  float activeDisplayRemaining_ = 0.0f;
  bool battleReadyHintShown_ = false;
  mutable std::unordered_map<std::string, uint32_t> textureCache_{};
  mutable uint32_t navigatorPortraitIndex_ = UINT32_MAX;
};

} // namespace OperatorGuidance
} // namespace Atrum
