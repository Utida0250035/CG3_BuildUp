#include "Scene/CommandSceneDevelopmentBootApply.h"

#include "Scene/CommandSceneDevelopmentEditorBootPolicy.h"

namespace Atrum {

Scene *CreateInitialSceneForDevelopmentBoot(const DevelopmentBootTarget &target) {
  return ApplyDevelopmentEditorBootScene(target);
}

} // namespace Atrum