#pragma once

namespace Atrum {

class Entity;

namespace Player {

/// 本編 EntityFactory::PlayerParry と同じ子階層・パリィ武器・方向マーカーを補完する
struct PlayerParryRuntimeAssemblyOptions {
  bool ensureAbstractInput = true;
  bool createParryWeapon = true;
  bool createWeaponPivotIfMissing = true;
  bool createDirectionMarkersIfMissing = true;
  float directionMarkerSideOffset = 0.65f;
};

/// player 役割または CmpMoveByInput2D を持つ Entity に本編同等のランタイム付帯を適用
void AssemblePlayerParryRuntimeExtras(
    Entity &player, const PlayerParryRuntimeAssemblyOptions &options = {});

bool EntityQualifiesForPlayerParryAssembly(const Entity &entity);

} // namespace Player
} // namespace Atrum