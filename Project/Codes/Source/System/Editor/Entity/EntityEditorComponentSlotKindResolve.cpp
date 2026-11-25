#include "System/Editor/Entity/EntityEditorComponentSlotKindResolve.h"

#include "System/Editor/Entity/EntityEditorKnownComponentRegistry.h"

namespace Atrum::EntityEditor {

std::optional<ComponentSlotKind>
TryDefaultKindForRegisteredType(const std::string &typeName) {
  if (!EntityEditorKnownComponentRegistry::Contains(typeName)) {
    return std::nullopt;
  }
  if (typeName == "CmpMeshSphere" || typeName == "CmpMesh" ||
      typeName == "CmpEditorTextureBillboard" || typeName == "CmpDrawHitSphere" ||
      typeName == "CmpBillboardInstancedGrid") {
    return ComponentSlotKind::Draw;
  }
  if (typeName == "CmpRotator" || typeName == "CmpScroll" ||
      typeName == "CmpHitSphere" || typeName == "CmpCamera" ||
      typeName == "CmpParticleEmitter" || typeName == "CmpBounceBall" ||
      typeName == "CmpBallKinematics" || typeName == "CmpBallAxisAlignedBounce" ||
      typeName == "CmpBallBounceImpactFx" || typeName == "CmpScriptUpd") {
    return ComponentSlotKind::Upd;
  }
  if (typeName == "CmpEditorAssetPath" || typeName == "CmpRuntimeTexturePath" ||
      typeName == "CmpColliderAabb") {
    return ComponentSlotKind::Data;
  }
  return std::nullopt;
}

} // namespace Atrum::EntityEditor