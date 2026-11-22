#include "Scene/SceneBuildUpMissingPrimaryCameraFeedback.h"

#include "Engine/AtrumEngine.h"
#include "Engine/Blend/BlendMode.h"
#include "Engine/EngineAccess.h"
#include "Engine/Platform/Window/Window.h"
#include "Math/Transform.h"
#include "Math/Vector4.h"

#if defined(DEVELOPMENT) && defined(USE_IMGUI)
#include "ForDebug/ImGui.h"
#endif

namespace Atrum {

namespace {

#if defined(DEVELOPMENT) && defined(USE_IMGUI)
bool g_pendingMissingCameraNotice = false;
std::string g_missingCameraNoticeMessage;
#endif

} // namespace

std::string BuildSceneBuildUpMissingPrimaryCameraMessage(
    const std::string &sceneJsonPath) {
  return "プライマリカメラがありません\n"
         "シーンに MainCamera（CmpCamera）があるか、cameraRig の自動生成を確認してください\n"
         "パス: " +
         sceneJsonPath + "\n"
         "F1: エディタへ戻る（Development）";
}

void DrawSceneBuildUpMissingPrimaryCameraDimOverlay(AtrumEngine *engine) {
  if (engine == nullptr) {
    engine = ::Atrum::EngineInstance();
  }
  if (engine == nullptr) {
    return;
  }

  const uint32_t textureIndex =
      EngineGetTexture("./Assets/Images/white4x4.png");
  if (textureIndex == UINT32_MAX) {
    return;
  }

  engine->SetBlendModeForFrame(BlendMode::NORMAL);
  engine->PrepareSprite();

  Math::Transform rectTransform{};
  rectTransform.scale = {
      static_cast<float>(Window::kDefaultClientWidth),
      static_cast<float>(Window::kDefaultClientHeight),
      1.0f};

  const Math::Vector4 dimColor{0.05f, 0.08f, 0.22f, 0.75f};
  engine->DrawSpriteRect(textureIndex, dimColor, Math::Transform{},
                         rectTransform, Math::Vector2{1.0f, 1.0f},
                         Math::Vector2{0.0f, 0.0f});
  engine->SetBlendModeForFrame(BlendMode::NONE);
}

bool HandleSceneBuildUpDrawWithoutPrimaryCamera(
    AtrumEngine *engine, const std::string &sceneJsonPath) {
#if defined(DEVELOPMENT) && defined(USE_IMGUI)
  g_missingCameraNoticeMessage =
      BuildSceneBuildUpMissingPrimaryCameraMessage(sceneJsonPath);
  g_pendingMissingCameraNotice = true;
#else
  (void)sceneJsonPath;
#endif
  DrawSceneBuildUpMissingPrimaryCameraDimOverlay(engine);
  return true;
}

#if defined(DEVELOPMENT) && defined(USE_IMGUI)

void DrawSceneBuildUpMissingPrimaryCameraNoticeIfPending() {
  if (!g_pendingMissingCameraNotice) {
    return;
  }
  g_pendingMissingCameraNotice = false;
  if (g_missingCameraNoticeMessage.empty()) {
    return;
  }
  ImGui::SetNextWindowPos(ImVec2(24.0f, 96.0f), ImGuiCond_Always);
  ImGui::SetNextWindowSize(ImVec2(560.0f, 180.0f), ImGuiCond_FirstUseEver);
  if (ImGui::Begin("SceneBuildUp（カメラ未設定）", nullptr,
                    ImGuiWindowFlags_NoCollapse)) {
    ImGui::TextWrapped("%s", g_missingCameraNoticeMessage.c_str());
  }
  ImGui::End();
}

#endif

} // namespace Atrum