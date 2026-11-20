#include "Scene/SceneBuildUpSceneLoadFailure.h"

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

std::string BuildSceneBuildUpSceneLoadFailureMessage(
    const std::string &requestedPath) {
  return "シーン JSON の読込に失敗しました\n"
         "代替のデフォルト 1 体は生成しません\n"
         "パス: " +
         requestedPath + "\n"
         "ファイルの存在と JSON 形式（entities 配列）を確認してください\n"
         "F1: エディタへ戻る（Development）";
}

#if defined(DEVELOPMENT) && defined(USE_IMGUI)

void DrawSceneBuildUpSceneLoadFailureNotice(const std::string &message) {
  if (message.empty()) {
    return;
  }
  ImGui::SetNextWindowPos(ImVec2(24.0f, 96.0f), ImGuiCond_Always);
  ImGui::SetNextWindowSize(ImVec2(560.0f, 200.0f), ImGuiCond_FirstUseEver);
  if (ImGui::Begin("SceneBuildUp（シーン読込失敗）", nullptr,
                     ImGuiWindowFlags_NoCollapse)) {
    ImGui::TextWrapped("%s", message.c_str());
  }
  ImGui::End();
}

#endif

void DrawSceneBuildUpSceneLoadFailureDimOverlay(AtrumEngine *engine) {
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

  const Math::Vector4 dimColor{0.15f, 0.02f, 0.02f, 0.72f};
  engine->DrawSpriteRect(textureIndex, dimColor, Math::Transform{},
                         rectTransform, Math::Vector2{1.0f, 1.0f},
                         Math::Vector2{0.0f, 0.0f});
  engine->SetBlendModeForFrame(BlendMode::NONE);
}

} // namespace Atrum