#include "System/Playable/PlayerParryRuntimeAssembly.h"

#include "Math/Quaternion.h"
#include "Math/Vector4.h"
#include "System/Entity/Components/CmpChildEntitys.h"
#include "System/Entity/Components/CmpDirection.h"
#include "System/Entity/Components/CmpMeshSphere.h"
#include "System/Entity/Components/CmpParry.h"
#include "System/Entity/Components/CmpSwing.h"
#include "System/Entity/Entity.h"
#include "System/Entity/EntityTagRegistry.h"
#include "System/EntityFactory.h"
#include "System/Playable/AbstractInput.h"
#include "System/Playable/CmpMoveByInput.h"

#include <memory>
#include <numbers>

namespace Atrum::Player {

namespace {

Entity *MakeDirectionMarkerEntity(const float radius, const Math::Vector4 &color) {
  auto marker = std::make_unique<Entity>();
  marker->SetIsInheritParentPositionOnly(true);
  marker->AddDrawCmp<CmpMeshSphere>();
  CmpMeshSphere *mesh = marker->GetDrawCmp<CmpMeshSphere>();
  mesh->SetRadius(radius);
  mesh->SetColor(color);
  mesh->SetSubdivision(8);
  return marker.release();
}

Entity *MakeWeaponPivotEntity() {
  auto weaponPivot = std::make_unique<Entity>();
  weaponPivot->AddUpdCmp<CmpChildEntitys>();

  Math::TransformLH weaponPivotInitialTransform{};
  weaponPivotInitialTransform.quaternion = Math::Quaternion::FromAxisAngle(
      Math::Vector3::ForwardLh(), -std::numbers::pi_v<float>);
  weaponPivot->SetInitialTransform(weaponPivotInitialTransform);
  weaponPivot->SetIsInheritParentPositionOnly(false);
  return weaponPivot.release();
}

} // namespace

bool EntityQualifiesForPlayerParryAssembly(const Entity &entity) {
  EntityTagRegistry::Instance().EnsureLoaded();
  if (EntityTagRegistry::Instance().EntityHasRole(entity, "player")) {
    return true;
  }
  return entity.HasUpdCmp<CmpMoveByInput2D>();
}

void AssemblePlayerParryRuntimeExtras(
    Entity &player, const PlayerParryRuntimeAssemblyOptions &options) {
  if (!EntityQualifiesForPlayerParryAssembly(player)) {
    return;
  }

  if (options.ensureAbstractInput) {
    AbstractInput::Instance()->ResolveDependence();
  }

  if (options.createParryWeapon && player.HasDataCmp<CmpParry>()) {
    CmpParry *cmpParry = player.GetDataCmp<CmpParry>();
    if (!cmpParry->HasEntParry()) {
      std::unique_ptr<Entity> parryWeapon = EntityFactory::Parry(&player);
      cmpParry->SetEntParry(parryWeapon.release());
    }
  }

  CmpSwing *cmpSwing =
      player.HasUpdCmp<CmpSwing>() ? player.GetUpdCmp<CmpSwing>() : nullptr;
  Entity *weaponPivotEntity = nullptr;

  if (options.createWeaponPivotIfMissing && cmpSwing != nullptr &&
      cmpSwing->GetPivot() == nullptr) {
    weaponPivotEntity = MakeWeaponPivotEntity();
    cmpSwing->SetPivot(weaponPivotEntity);
    if (cmpSwing->PivotInstanceName().empty()) {
      cmpSwing->SetPivotInstanceName("WeaponPivot");
    }
  }

  CmpMoveByInput2D *cmpMove =
      player.HasUpdCmp<CmpMoveByInput2D>()
          ? player.GetUpdCmp<CmpMoveByInput2D>()
          : nullptr;

  Entity *directionMarkerForward = nullptr;
  Entity *directionMarkerRear = nullptr;
  const Math::Vector4 forwardMarkerColor{0.85f, 0.75f, 0.2f, 1.0f};
  const Math::Vector4 rearMarkerColor{0.85f, 0.75f, 0.2f, 0.45f};

  if (options.createDirectionMarkersIfMissing && cmpMove != nullptr &&
      !cmpMove->HasDirectionMarkers()) {
    directionMarkerForward = MakeDirectionMarkerEntity(0.24f, forwardMarkerColor);
    directionMarkerRear = MakeDirectionMarkerEntity(0.15f, rearMarkerColor);
    cmpMove->SetDirectionMarkers(directionMarkerForward, directionMarkerRear,
                                 options.directionMarkerSideOffset);
    cmpMove->SetDirectionMarkerBaseColors(forwardMarkerColor, rearMarkerColor);
    cmpMove->SetInitialDirectionFaceSignX(1.0f);
  }

  std::vector<Entity *> childAdds{};
  if (weaponPivotEntity != nullptr) {
    childAdds.push_back(weaponPivotEntity);
  }
  if (directionMarkerForward != nullptr) {
    childAdds.push_back(directionMarkerForward);
  }
  if (directionMarkerRear != nullptr) {
    childAdds.push_back(directionMarkerRear);
  }

  if (!childAdds.empty()) {
    if (!player.HasUpdCmp<CmpChildEntitys>()) {
      player.AddUpdCmp<CmpChildEntitys>();
    }
    player.GetUpdCmp<CmpChildEntitys>()->AddChilds(childAdds);
  }

  if (cmpMove != nullptr && player.HasDataCmp<CmpDirection>()) {
    cmpMove->SetInitialDirectionFaceSignX(
        player.GetDataCmp<CmpDirection>()->GetDirection().x >= 0.0f ? 1.0f
                                                                    : -1.0f);
  }
}

} // namespace Atrum::Player