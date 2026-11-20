#pragma once

#include <string>

namespace Atrum {

class AtrumEngine;

std::string BuildSceneBuildUpSceneLoadFailureMessage(
    const std::string &requestedPath);

#if defined(DEVELOPMENT) && defined(USE_IMGUI)
void DrawSceneBuildUpSceneLoadFailureNotice(
    const std::string &message);
#endif

void DrawSceneBuildUpSceneLoadFailureDimOverlay(AtrumEngine *engine);

} // namespace Atrum