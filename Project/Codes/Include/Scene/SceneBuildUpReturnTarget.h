#pragma once

namespace Atrum {

class Scene;

/// SceneBuildUp の returnScene 未指定時に F1 復帰で使う既定先
Scene *ResolveSceneBuildUpReturnTarget(Scene *returnScene);

} // namespace Atrum