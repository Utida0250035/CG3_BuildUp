#include "System/Editor/Scene/EditorSceneRuntimeEntityHasComponentForSlot.h"

#include "System/Editor/Entity/EntityEditorDefinition.h"
#include "System/Entity/Components/CmpBallAxisAlignedBounce.h"
#include "System/Entity/Components/CmpBallBounceImpactFx.h"
#include "System/Entity/Components/CmpBallKinematics.h"
#include "System/Entity/Components/CmpBillboardInstancedGrid.h"
#include "System/Entity/Components/CmpBounceBall.h"
#include "System/Entity/Components/CmpCamera.h"
#include "System/Entity/Components/CmpColliderAabb.h"
#include "System/Entity/Components/CmpDrawHitSphere.h"
#include "System/Entity/Components/CmpEditorAssetPath.h"
#include "System/Entity/Components/CmpEditorTextureBillboard.h"
#include "System/Entity/Components/CmpHitSphere.h"
#include "System/Entity/Components/CmpMesh.h"
#include "System/Entity/Components/CmpMeshSphere.h"
#include "System/Entity/Components/CmpParticleEmitter.h"
#include "System/Entity/Components/CmpRotator.h"
#include "System/Entity/Components/CmpRuntimeTexturePath.h"
#include "System/Entity/Components/CmpChildEntitys.h"
#include "System/Entity/Components/CmpDirection.h"
#include "System/Entity/Components/CmpHealthHit.h"
#include "System/Entity/Components/CmpLhRigidBody.h"
#include "System/Entity/Components/CmpLuaDraw.h"
#include "System/Entity/Components/CmpLuaUpd.h"
#include "System/Entity/Components/CmpParry.h"
#include "System/Entity/Components/CmpSwing.h"
#include "System/Playable/CmpDrawPlayerParryField.h"
#include "System/Playable/CmpMoveByInput.h"
#include "System/Entity/Components/CmpScroll.h"
#include "System/Entity/Entity.h"

namespace Atrum::EntityEditor {

bool EntityHasComponentForSlot(const Entity &entity, const ComponentSlot &slot) {
  if (slot.kind == ComponentSlotKind::Draw) {
    if (slot.typeName == "CmpMeshSphere") {
      return entity.HasDrawCmp<CmpMeshSphere>();
    }
    if (slot.typeName == "CmpBillboardInstancedGrid") {
      return entity.HasDrawCmp<CmpBillboardInstancedGrid>();
    }
    if (slot.typeName == "CmpMesh") {
      return entity.HasDrawCmp<CmpMesh>();
    }
    if (slot.typeName == "CmpEditorTextureBillboard") {
      return entity.HasDrawCmp<CmpEditorTextureBillboard>();
    }
    if (slot.typeName == "CmpDrawHitSphere") {
      return entity.HasDrawCmp<CmpDrawHitSphere>();
    }
    if (slot.typeName == "CmpLuaDraw") {
      return entity.HasDrawCmp<CmpLuaDraw>();
    }
    if (slot.typeName == "CmpDrawPlayerParryField") {
      return entity.HasDrawCmp<Player::CmpDrawPlayerParryField>();
    }
  }
  if (slot.kind == ComponentSlotKind::Upd) {
    if (slot.typeName == "CmpRotator") {
      return entity.HasUpdCmp<CmpRotator>();
    }
    if (slot.typeName == "CmpScroll") {
      return entity.HasUpdCmp<CmpScroll>();
    }
    if (slot.typeName == "CmpHitSphere") {
      return entity.HasUpdCmp<CmpHitSphere>();
    }
    if (slot.typeName == "CmpCamera") {
      return entity.HasUpdCmp<CmpCamera>();
    }
    if (slot.typeName == "CmpParticleEmitter") {
      return entity.HasUpdCmp<CmpParticleEmitter>();
    }
    if (slot.typeName == "CmpBounceBall") {
      return entity.HasUpdCmp<CmpBounceBall>();
    }
    if (slot.typeName == "CmpBallKinematics") {
      return entity.HasUpdCmp<CmpBallKinematics>();
    }
    if (slot.typeName == "CmpBallAxisAlignedBounce") {
      return entity.HasUpdCmp<CmpBallAxisAlignedBounce>();
    }
    if (slot.typeName == "CmpBallBounceImpactFx") {
      return entity.HasUpdCmp<CmpBallBounceImpactFx>();
    }
    if (slot.typeName == "CmpLuaUpd") {
      return entity.HasUpdCmp<CmpLuaUpd>();
    }
    if (slot.typeName == "CmpMoveByInput2D") {
      return entity.HasUpdCmp<Player::CmpMoveByInput2D>();
    }
    if (slot.typeName == "CmpLhRigidBody") {
      return entity.HasUpdCmp<CmpLhRigidBody>();
    }
    if (slot.typeName == "CmpHealthHit") {
      return entity.HasUpdCmp<CmpHealthHit>();
    }
    if (slot.typeName == "CmpSwing") {
      return entity.HasUpdCmp<CmpSwing>();
    }
    if (slot.typeName == "CmpChildEntitys") {
      return entity.HasUpdCmp<CmpChildEntitys>();
    }
    if (slot.typeName == "CmpInput") {
      return false;
    }
  }
  if (slot.kind == ComponentSlotKind::Data) {
    if (slot.typeName == "CmpEditorAssetPath") {
      return entity.HasDataCmp<CmpEditorAssetPath>();
    }
    if (slot.typeName == "CmpRuntimeTexturePath") {
      return entity.HasDataCmp<CmpRuntimeTexturePath>();
    }
    if (slot.typeName == "CmpColliderAabb") {
      return entity.HasDataCmp<CmpColliderAabb>();
    }
    if (slot.typeName == "CmpDirection") {
      return entity.HasDataCmp<CmpDirection>();
    }
    if (slot.typeName == "CmpParry") {
      return entity.HasDataCmp<CmpParry>();
    }
  }
  return false;
}

} // namespace Atrum::EntityEditor