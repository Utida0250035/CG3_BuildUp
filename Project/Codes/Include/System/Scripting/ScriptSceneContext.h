#pragma once

#include <memory>
#include <vector>

namespace Atrum {

class Entity;
class Scene;

namespace Script {

/// Lua（SceneFlow.*）が「いま実行中の SceneBuildUp」を参照するための橋渡し。
/// SceneBuildUp が EnterScene で Bind、ExitScene で Unbind する。
namespace SceneContext {

void Bind(Scene *scene, const std::vector<std::unique_ptr<Entity>> *entities,
          Scene *returnScene);

/// scene が現在 Bind されているものと同一のときだけ解除する
void Unbind(const Scene *scene);

const std::vector<std::unique_ptr<Entity>> *Entities();
Scene *ReturnScene();

} // namespace SceneContext

} // namespace Script
} // namespace Atrum
