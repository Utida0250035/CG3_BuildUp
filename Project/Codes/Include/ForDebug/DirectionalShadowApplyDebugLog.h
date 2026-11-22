#pragma once

#if defined(DEVELOPMENT)

#include "Engine/Renderer/Lighting/DirectionalShadowApplyDiagnostics.h"
#include "Engine/Renderer/Lighting/DirectionalShadow.h"

namespace Atrum::Debug {

void AppendDirectionalShadowApplySnapshotToLog(
    const Visual::DirectionalShadowSettings &settings,
    const Visual::DirectionalShadowApplyDiagnostics::ApplySnapshot &snapshot);

} // namespace Atrum::Debug

#endif