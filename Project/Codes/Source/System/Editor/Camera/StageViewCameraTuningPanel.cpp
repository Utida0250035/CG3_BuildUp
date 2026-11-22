#include "System/Editor/Camera/StageViewCameraTuning.h"

#if defined(DEVELOPMENT) && defined(USE_IMGUI)

#include "Camera/StageViewFramingBounds.h"
#include "ForDebug/ImGui.h"
#include "System/Editor/EditorAssetsPaths.h"

namespace Atrum::Editor {

void DrawStageViewCameraTuningPanel() {
  StageViewCameraTuning &tuning = StageViewCameraTuning::Instance();

  if (!ImGui::Begin("Stage camera tuning")) {
    ImGui::End();
    return;
  }

  bool preview = tuning.IsGameViewPreviewEnabled();
  if (ImGui::Checkbox("JSON preview on Game view", &preview)) {
    tuning.SetGameViewPreviewEnabled(preview);
    if (preview && !tuning.IsLoaded()) {
      tuning.ReloadFromDisk();
    }
  }

  bool autoReload = tuning.IsAutoHotReloadEnabled();
  if (ImGui::Checkbox("Auto hot-reload", &autoReload)) {
    tuning.SetAutoHotReloadEnabled(autoReload);
  }

  if (ImGui::Button("Reload JSON now")) {
    tuning.ReloadFromDisk();
  }
  ImGui::SameLine();
  if (ImGui::Button("Load default path")) {
    tuning.SetJsonPath(EditorAssets::kStageViewCameraTuningJson);
  }

  std::string path = tuning.JsonPath();
  if (path.empty()) {
    path = EditorAssets::kStageViewCameraTuningJson;
  }
  if (ImGui::InputText("JSON path", &path)) {
    tuning.SetJsonPath(path);
  }

  ImGui::TextWrapped("%s", tuning.LastLoadMessage().c_str());

  const StageViewFramingBounds framing =
      StageViewFramingBounds::DefaultFromStageConstants();
  const StageViewCameraPose pose = tuning.ComputeBattlePose(framing);
  ImGui::Separator();
  ImGui::Text("Battle pose (default framing)");
  ImGui::Text("pivot: (%.3f, %.3f, %.3f)", pose.pivot.x, pose.pivot.y,
              pose.pivot.z);
  ImGui::Text("eye:   (%.3f, %.3f, %.3f)", pose.eye.x, pose.eye.y, pose.eye.z);

  ImGui::End();
}

} // namespace Atrum::Editor

#else

namespace Atrum::Editor {

void DrawStageViewCameraTuningPanel() {}

} // namespace Atrum::Editor

#endif