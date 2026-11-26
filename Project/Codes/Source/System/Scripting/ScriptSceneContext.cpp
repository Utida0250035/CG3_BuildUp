#include "System/Scripting/ScriptSceneContext.h"

namespace Atrum::Script::SceneContext {

namespace {

struct State {
  const Scene *scene = nullptr;
  const std::vector<std::unique_ptr<Entity>> *entities = nullptr;
  Scene *returnScene = nullptr;
};

State &Current() {
  static State state;
  return state;
}

} // namespace

void Bind(Scene *scene, const std::vector<std::unique_ptr<Entity>> *entities,
          Scene *returnScene) {
  State &state = Current();
  state.scene = scene;
  state.entities = entities;
  state.returnScene = returnScene;
}

void Unbind(const Scene *scene) {
  State &state = Current();
  if (state.scene != scene) {
    return;
  }
  state = State{};
}

const std::vector<std::unique_ptr<Entity>> *Entities() {
  return Current().entities;
}

Scene *ReturnScene() { return Current().returnScene; }

} // namespace Atrum::Script::SceneContext
