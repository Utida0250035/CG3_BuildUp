#pragma once

namespace Atrum {

class Scene;

/// SceneMovie の returnScene 未指定時に使う復帰先を解決する
Scene *ResolveSceneMovieReturnTarget(Scene *returnScene);

} // namespace Atrum