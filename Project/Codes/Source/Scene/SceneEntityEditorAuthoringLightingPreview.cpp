#include "Scene/SceneEntityEditorAuthoringLightingPreview.h"

#include "Scene/SceneGameLightingProfile.h"

namespace Atrum::SceneEntityEditorAuthoringLightingPreview {

void ApplyOnEnterEntityEditor() { ApplySceneGameLightingFromProfile(); }

} // namespace Atrum::SceneEntityEditorAuthoringLightingPreview