#include "System/Editor/Entity/EntityEditorKnownComponentRegistry.h"

#include <array>
#include <string_view>

namespace Atrum::EntityEditor::EntityEditorKnownComponentRegistry {

namespace {

constexpr std::array<std::string_view, 18> kEditorRuntimeComponentTypeNames = {
    "CmpMeshSphere",
    "CmpMesh",
    "CmpEditorTextureBillboard",
    "CmpDrawHitSphere",
    "CmpRotator",
    "CmpScroll",
    "CmpHitSphere",
    "CmpEditorAssetPath",
    "CmpRuntimeTexturePath",
    "CmpColliderAabb",
    "CmpCamera",
    "CmpParticleEmitter",
    "CmpBounceBall",
    "CmpBallKinematics",
    "CmpBallAxisAlignedBounce",
    "CmpBallBounceImpactFx",
    "CmpBillboardInstancedGrid",
    "CmpScriptUpd",
};

constexpr std::array<std::string_view, 3> kGuidelineDrawExampleTypeNames = {
    "CmpMeshSphere",
    "CmpMesh",
    "CmpEditorTextureBillboard",
};

} // namespace

bool Contains(const std::string &typeName) {
  for (const std::string_view registered : kEditorRuntimeComponentTypeNames) {
    if (typeName == registered) {
      return true;
    }
  }
  return false;
}

bool GuidelineDrawExamplesAreRegistered() {
  for (const std::string_view example : kGuidelineDrawExampleTypeNames) {
    if (!Contains(std::string(example))) {
      return false;
    }
  }
  return true;
}

} // namespace Atrum::EntityEditor::EntityEditorKnownComponentRegistry