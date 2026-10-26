#include "System/EntityFactory.h"
#include "System/CombatBulletTuning.h"
#include "Math/Matrix4x4.h"
#include "Math/Quaternion.h"
#include "Math/Transform.h"
#include "Math/Vector4.h"
#include "System/Entity/Components/CmpChildEntitys.h"
#include "System/Entity/Components/CmpDrawBullet.h"
#include "System/AttackTelegraph/CmpAttackTelegraph.h"
#include "System/AttackTelegraph/CmpDrawAttackTelegraph.h"
#include "System/Entity/Components/CmpDrawHitSphere.h"
#include "System/Entity/Components/CmpEnemy.h"
#include "System/Entity/Components/CmpMeshSphere.h"
#include "System/Entity/Components/CmpOrbiter.h"
#include "System/Entity/Components/CmpParry.h"
#include "System/Entity/Components/CmpReflectAbleBullet.h"
#include "System/Entity/Components/CmpScroll.h"
#include "Visual/CmpSkyDome.h"
#include "System/Entity/Components/CmpSwing.h"
#include "System/Entity/Components/CmpDirection.h"
#include "System/Entity/Components/CmpHealthHit.h"
#include "System/Entity/Components/CmpHitSphere.h"
#include "System/Entity/Components/CmpLhRigidBody.h"
#include "System/Entity/Components/CmpMesh.h"
#include "Camera/Camera.h"
#include "Engine/AtrumEngine.h"
#include "Engine/Resource/AssetModel.h"
#include "System/Entity/Entity.h"
#include "System/Playable/CmpDrawPlayerParryField.h"
#include "System/Playable/CmpMoveByInput.h"
#include <iterator>
#include <numbers>
#include <cstdlib>
#include <ctime>

namespace Atrum {

namespace {

inline constexpr const char *kSkyDomeObjectDirectory =
    "./Assets/Objects/SkyDome";
inline constexpr const char *kSkyDomeObjFileName = "SkyDome.obj";
inline constexpr const char *kSkyDomeMtlFileName = "SkyDome.mtl";

/// ステージカメラは -Z 側から pivot を見る。ボスの正面は手前（-Z）＋自機方向。
Math::Vector3 ComputeBossFaceForward(const Math::Vector3 &bossPosition,
                                   const Entity *player) {
  Math::Vector3 towardScreenFront{0.0f, 0.0f, -1.0f};
  if (player == nullptr) {
    return towardScreenFront;
  }

  Math::Vector3 toPlayer = player->GetWorldPosition() - bossPosition;
  toPlayer.z = 0.0f;
  if (toPlayer.LengthSquare() <= 1.0e-8f) {
    return towardScreenFront;
  }

  toPlayer = toPlayer.Normalized();
  return (toPlayer * 0.45f + towardScreenFront * 0.55f).Normalized();
}

} // namespace

std::unique_ptr<Entity> EntityFactory::PlayerParry() {
  std::unique_ptr<Entity> playerParry = std::make_unique<Entity>();
  playerParry->SetEntityType(EntityType::PLAYER);

  Math::TransformLH initialTransform{};
  initialTransform.quaternion =
      Math::Quaternion::FromAxisAngle(Math::Vector3::UpLh(), -std::numbers::pi_v<float> * 0.5f);
  initialTransform.translate = {-8.0f, -3.0f, 0.0f};
  playerParry->SetInitialTransform(initialTransform);

  playerParry->AddDataCmp<CmpDirection>();
  playerParry->AddUpdCmp<CmpLhRigidBody>();
  playerParry->AddUpdCmp<CmpHitSphere>();
  playerParry->AddUpdCmp<CmpHealthHit>();
  playerParry->AddDrawCmp<CmpMesh>();
  playerParry->AddDrawCmp<Player::CmpDrawPlayerParryField>();
  playerParry->AddDataCmp<CmpParry>();
  playerParry->AddUpdCmp<CmpChildEntitys>();
  playerParry->AddUpdCmp<CmpSwing>();
  playerParry->AddUpdCmp<Player::CmpMoveByInput2D>();

  CmpMesh *cmpMesh = playerParry->GetDrawCmp<CmpMesh>();
  cmpMesh->SetRadius(0.5f);
  cmpMesh->SetColor(Math::Vector4::White());

  CmpLhRigidBody *cmpLhRigidBody = playerParry->GetUpdCmp<CmpLhRigidBody>();
  cmpLhRigidBody->SetGravity({0.0f, -45.0f, 0.0f});

  playerParry->GetDataCmp<CmpDirection>()->SetDirection(
      {1.0f, 0.0f, 0.0f});

  CmpHitSphere *cmpHitSphere = playerParry->GetUpdCmp<CmpHitSphere>();
  cmpHitSphere->SetSpheres({{{}, 0.5f}});

  CmpHealthHit *cmpHealthHit = playerParry->GetUpdCmp<CmpHealthHit>();
  cmpHealthHit->SetHurterTypes({EntityType::ENEMY, EntityType::ENEMY_WEAPON});
  cmpHealthHit->SetInitialHealthCount(5);
  cmpHealthHit->SetInvincibleTime(kPlayerDamageInvincibleSeconds);

  CmpParry *cmpParry = playerParry->GetDataCmp<CmpParry>();
  cmpParry->SetEntParry(Parry(playerParry.get()).release());

  Player::CmpMoveByInput2D *cmpMoveByInput =
      playerParry->GetUpdCmp<Player::CmpMoveByInput2D>();

  Entity *weaponPivot = new Entity();
  weaponPivot->AddUpdCmp<CmpChildEntitys>();
  CmpSwing *cmpSwing = playerParry->GetUpdCmp<CmpSwing>();
  cmpSwing->SetPivot(weaponPivot);
  cmpSwing->SetSwingSpeed(std::numbers::pi_v<float> * 8.0f);
  cmpSwing->SetSwingDuration(0.25f);

  Math::TransformLH weaponPivotInitialTransform{};
  weaponPivotInitialTransform.quaternion =
      Math::Quaternion::FromAxisAngle(Math::Vector3::ForwardLh(), -std::numbers::pi_v<float>);
  weaponPivot->SetInitialTransform(weaponPivotInitialTransform);
  weaponPivot->SetIsInheritParentPositionOnly(false);

  constexpr float kDirectionMarkerSideOffset = 0.65f;
  const Math::Vector4 forwardMarkerColor{0.85f, 0.75f, 0.2f, 1.0f};
  const Math::Vector4 rearMarkerColor{0.85f, 0.75f, 0.2f, 0.45f};

  auto makeDirectionMarker = [&](const float radius,
                                 const Math::Vector4 &color) -> Entity * {
    Entity *marker = new Entity();
    marker->SetIsInheritParentPositionOnly(true);
    marker->AddDrawCmp<CmpMeshSphere>();
    CmpMeshSphere *cmpMarkerMesh = marker->GetDrawCmp<CmpMeshSphere>();
    cmpMarkerMesh->SetRadius(radius);
    cmpMarkerMesh->SetColor(color);
    cmpMarkerMesh->SetSubdivision(8);
    return marker;
  };

  Entity *directionMarkerForward = makeDirectionMarker(0.24f, forwardMarkerColor);
  Entity *directionMarkerRear = makeDirectionMarker(0.15f, rearMarkerColor);

  cmpMoveByInput->SetDirectionMarkers(directionMarkerForward,
                                      directionMarkerRear,
                                      kDirectionMarkerSideOffset);
  cmpMoveByInput->SetDirectionMarkerBaseColors(forwardMarkerColor,
                                               rearMarkerColor);
  cmpMoveByInput->SetInitialDirectionFaceSignX(1.0f);

  playerParry->GetUpdCmp<CmpChildEntitys>()->AddChilds(
      {weaponPivot, directionMarkerForward, directionMarkerRear});

  playerParry->ResolveDependencies();
  playerParry->Initialize();
  return playerParry;
}

std::unique_ptr<Entity> EntityFactory::EnemyAstro(Entity *player) {
  std::unique_ptr<Entity> enemyAstro = std::make_unique<Entity>();
  enemyAstro->SetEntityType(EntityType::ENEMY);

  Math::TransformLH initialTransform{};
  initialTransform.translate = {8.0f, 0.0f, 0.0f};
  initialTransform.quaternion = Math::Quaternion::FromLhLookAt(
      ComputeBossFaceForward(initialTransform.translate, player),
      Math::Vector3::UpLh());
  enemyAstro->SetInitialTransform(initialTransform);

  enemyAstro->AddUpdCmp<CmpHitSphere>();
  enemyAstro->AddUpdCmp<CmpHealthHit>();
  enemyAstro->AddUpdCmp<CmpEnemy>();
  enemyAstro->AddUpdCmp<CmpReflectAbleBullet>();
  enemyAstro->AddDataCmp<CmpAttackTelegraph>();
  enemyAstro->AddDrawCmp<CmpMeshSphere>();
  enemyAstro->AddDrawCmp<CmpDrawHitSphere>();
  enemyAstro->AddDrawCmp<CmpDrawAttackTelegraph>();
  enemyAstro->AddDrawCmp<CmpDrawBullet>();
  enemyAstro->AddUpdCmp<CmpChildEntitys>();

  constexpr float kBossOuterShellAlpha = 0.45f;
  const Math::Vector4 bossShellOrange{0.75f, 0.25f, 0.125f, kBossOuterShellAlpha};
  const Math::Vector4 bossInnerOrange{0.75f, 0.25f, 0.125f, 1.0f};

  CmpMeshSphere *cmpMeshSphere = enemyAstro->GetDrawCmp<CmpMeshSphere>();
  cmpMeshSphere->SetColor(bossShellOrange);
  cmpMeshSphere->SetRadius(1.0f);
  cmpMeshSphere->SetSubdivision(12);

  constexpr float bossInnerRadius = 0.72f;
  enemyAstro->GetUpdCmp<CmpHitSphere>()->SetSpheres({{{}, bossInnerRadius}});

  CmpDrawHitSphere *cmpDrawHitSphere =
      enemyAstro->GetDrawCmp<CmpDrawHitSphere>();
  cmpDrawHitSphere->SetColor(bossInnerOrange);
  cmpDrawHitSphere->SetLightingEnabled(true);

  CmpHealthHit *cmpHealthHit = enemyAstro->GetUpdCmp<CmpHealthHit>();
  cmpHealthHit->SetHurterTypes({EntityType::PLAYER_ITEM});
  cmpHealthHit->SetInitialHealthCount(32);
  cmpHealthHit->SetInvincibleTime(0.01f);
  cmpHealthHit->SetStackReflectedBulletDamage(true);

  enemyAstro->GetUpdCmp<CmpEnemy>()->SetPlayer(player);

  const float orbiterHitRadius = 0.4f;
  const Math::Vector4 orbiterColor = bossShellOrange;
  const Math::Vector3 orbitAxis = Math::Vector3::ForwardLh();

  struct OrbitConfig {
    float orbitRadius = 1.5f;
    float meshRadius = 0.5f;
    /// 度/秒（符号で回転方向）
    float rotateSpeedByDegree = 180.0f;
    float startAngleByDegree = 0.0f;
  };

  const std::vector<OrbitConfig> orbitConfigs{
      {1.35f, 0.46f, 210.0f, 0.0f},
      {1.70f, 0.52f, -175.0f, 48.0f},
      {1.50f, 0.44f, 255.0f, 96.0f},
      {1.85f, 0.55f, -300.0f, 144.0f},
      {1.25f, 0.42f, 330.0f, 204.0f},
  };

  std::vector<Entity *> orbiters;
  for (const OrbitConfig &config : orbitConfigs) {
    Entity *orbiter = new Entity();
    orbiter->SetEntityType(EntityType::ENEMY_WEAPON);
    orbiter->AddUpdCmp<CmpOrbiter>();
    orbiter->AddDrawCmp<CmpMeshSphere>();
    orbiter->AddUpdCmp<CmpHitSphere>();

    orbiter->GetDrawCmp<CmpMeshSphere>()->SetColor(orbiterColor);
    orbiter->GetDrawCmp<CmpMeshSphere>()->SetRadius(config.meshRadius);
    orbiter->GetUpdCmp<CmpHitSphere>()->SetSpheres({{{}, orbiterHitRadius}});

    const float startAngleRad =
        config.startAngleByDegree / 180.0f * std::numbers::pi_v<float>;
    Math::TransformLH orbiterInitialTransform{};
    orbiterInitialTransform.translate = {
        std::cos(startAngleRad) * config.orbitRadius,
        std::sin(startAngleRad) * config.orbitRadius,
        0.0f};
    orbiter->SetInitialTransform(orbiterInitialTransform);

    CmpOrbiter *cmpOrbiter = orbiter->GetUpdCmp<CmpOrbiter>();
    cmpOrbiter->SetRotateAxis(orbitAxis);
    cmpOrbiter->SetRotateSpeed(config.rotateSpeedByDegree / 180.0f *
                               std::numbers::pi_v<float>);
    orbiters.push_back(orbiter);
  }

  enemyAstro->GetUpdCmp<CmpChildEntitys>()->AddChilds(orbiters);
  enemyAstro->ResolveDependencies();
  enemyAstro->Initialize();

  for (Entity *orbiter : orbiters) {
    if (orbiter != nullptr && orbiter->HasUpdCmp<CmpOrbiter>()) {
      orbiter->GetUpdCmp<CmpOrbiter>()->CaptureDefaults();
    }
  }

  return enemyAstro;
}

std::vector<std::unique_ptr<Entity>> EntityFactory::ScrollSpheres() {
  std::srand(static_cast<unsigned int>(std::time(nullptr)));

  // 遠景球の個数と細分を抑えて 3D 描画予算を戦闘演出に回す
  constexpr size_t kBackgroundSphereCount = 12;
  constexpr uint32_t kBackgroundSphereSubdivision = 2;
  constexpr Math::Vector4 kBackgroundSphereColor{1.0f, 1.0f, 1.0f, 0.3f};

  std::vector<std::unique_ptr<Entity>> scrollSpheres;
  scrollSpheres.reserve(kBackgroundSphereCount);
  for (size_t i = 0; i < kBackgroundSphereCount; ++i) {
    std::unique_ptr<Entity> scrollSphere = std::make_unique<Entity>();
    Math::TransformLH initialTransform{};
    initialTransform.translate = {static_cast<float>(std::rand() % 33 - 32), static_cast<float>(std::rand() % 17 - 8),
                                  static_cast<float>(std::rand() % 11 + 20)};
    scrollSphere->SetInitialTransform(initialTransform);
    scrollSphere->AddUpdCmp<CmpScroll>();
    scrollSphere->AddDrawCmp<CmpMeshSphere>();

    float scrollSpeed = static_cast<float>(std::rand() % 5 + 4);
    if (std::rand() % 2 == 0) {
      scrollSpeed *= -1.0f;
    }
    scrollSphere->GetUpdCmp<CmpScroll>()->SetScrollSpeed(scrollSpeed);

    CmpMeshSphere *cmpMeshSphere = scrollSphere->GetDrawCmp<CmpMeshSphere>();
    cmpMeshSphere->SetColor(kBackgroundSphereColor);
    cmpMeshSphere->SetRadius(static_cast<float>(std::rand() % 16 + 16) * 0.03125f);
    cmpMeshSphere->SetSubdivision(kBackgroundSphereSubdivision);
    cmpMeshSphere->SetLightingEnabled(false);

    scrollSphere->ResolveDependencies();
    scrollSphere->Initialize();
    scrollSpheres.emplace_back(std::move(scrollSphere));
  }

  return scrollSpheres;
}

std::vector<std::unique_ptr<Entity>> EntityFactory::ForegroundGroundClouds() {
  struct GroundCloudSpec {
    float x;
    float y;
    float z;
    float radius;
    float alpha;
  };

  // 固定配置。kGroundHeight(-4)・自機半径0.5 の足元(y≈-4.5)まで雲が届くよう接地帯で下層と接続
  // alpha は弾影が透けるよう全体を少しだけ下げる（各 spec は相対の濃さ）
  static constexpr float kCloudAlphaScale = 0.84f;
  static constexpr GroundCloudSpec kSpecs[] = {
      // 左端（LeftEndX=-14 より外側）
      {-16.4f, -6.6f, 0.0f, 3.0f, 0.97f},  {-15.0f, -6.45f, 0.05f, 2.55f, 0.96f},
      // 右端
      {16.4f, -6.6f, 0.0f, 3.0f, 0.97f},   {15.0f, -6.45f, -0.05f, 2.55f, 0.96f},
      // 下層（床際・上端は接地帯より下で自機本体に被らない）
      {-10.0f, -6.72f, 0.0f, 2.32f, 0.97f}, {-5.0f, -6.65f, 0.0f, 2.28f, 0.96f},
      {0.0f, -6.75f, 0.0f, 2.38f, 0.98f},   {5.0f, -6.65f, 0.0f, 2.28f, 0.96f},
      {10.0f, -6.72f, 0.0f, 2.32f, 0.97f},
      // 接地帯（足元 y≈-4.5 まで届かせつつ apex は y≈-4.35 付近に抑える）
      {-15.0f, -5.52f, 0.0f, 1.17f, 0.96f}, {-13.25f, -5.48f, 0.0f, 1.17f, 0.96f},
      {-11.5f, -5.52f, 0.0f, 1.17f, 0.96f}, {-9.75f, -5.48f, 0.0f, 1.17f, 0.96f},
      {-8.0f, -5.52f, 0.0f, 1.17f, 0.96f},  {-6.25f, -5.48f, 0.0f, 1.17f, 0.96f},
      {-4.5f, -5.52f, 0.0f, 1.17f, 0.96f},  {-2.75f, -5.48f, 0.0f, 1.17f, 0.96f},
      {-1.0f, -5.52f, 0.0f, 1.17f, 0.96f},  {0.75f, -5.48f, 0.0f, 1.17f, 0.96f},
      {2.5f, -5.52f, 0.0f, 1.17f, 0.96f},   {4.25f, -5.48f, 0.0f, 1.17f, 0.96f},
      {6.0f, -5.52f, 0.0f, 1.17f, 0.96f},   {7.75f, -5.48f, 0.0f, 1.17f, 0.96f},
      {9.5f, -5.52f, 0.0f, 1.17f, 0.96f},   {11.25f, -5.48f, 0.0f, 1.17f, 0.96f},
      {13.0f, -5.52f, 0.0f, 1.17f, 0.96f},  {14.75f, -5.48f, 0.0f, 1.17f, 0.96f},
      // 下層との縦つなぎ（上段の隙間を埋める・小半径）
      {-14.125f, -5.95f, 0.0f, 0.92f, 0.94f}, {-10.625f, -5.95f, 0.0f, 0.92f, 0.94f},
      {-7.125f, -5.95f, 0.0f, 0.92f, 0.94f},  {-3.625f, -5.95f, 0.0f, 0.92f, 0.94f},
      {-0.125f, -5.95f, 0.0f, 0.92f, 0.94f},  {3.375f, -5.95f, 0.0f, 0.92f, 0.94f},
      {6.875f, -5.95f, 0.0f, 0.92f, 0.94f},   {10.375f, -5.95f, 0.0f, 0.92f, 0.94f},
      {13.875f, -5.95f, 0.0f, 0.92f, 0.94f},
  };

  std::vector<std::unique_ptr<Entity>> clouds;
  clouds.reserve(std::size(kSpecs));

  for (const GroundCloudSpec &spec : kSpecs) {
    std::unique_ptr<Entity> cloud = std::make_unique<Entity>();
    Math::TransformLH initialTransform{};
    initialTransform.translate = {spec.x, spec.y, spec.z};
    cloud->SetInitialTransform(initialTransform);
    cloud->AddDrawCmp<CmpMeshSphere>();

    CmpMeshSphere *mesh = cloud->GetDrawCmp<CmpMeshSphere>();
    mesh->SetColor(
        {1.0f, 1.0f, 1.0f, spec.alpha * kCloudAlphaScale});
    mesh->SetRadius(spec.radius);
    // 前景雲は形状を粗くして描画負荷を抑える
    mesh->SetSubdivision(4);
    mesh->SetLightingEnabled(true);

    cloud->ResolveDependencies();
    cloud->Initialize();
    clouds.emplace_back(std::move(cloud));
  }

  return clouds;
}

std::unique_ptr<Entity> EntityFactory::SkyDome(Camera *camera) {
  std::shared_ptr<AssetModel> skyDomeModel =
      AtrumEngine::Instance()->GetModel(kSkyDomeObjectDirectory,
                                        kSkyDomeObjFileName,
                                        kSkyDomeObjectDirectory,
                                        kSkyDomeMtlFileName);

  std::unique_ptr<Entity> skyDomeEntity = std::make_unique<Entity>();
  skyDomeEntity->AddDrawCmp<CmpSkyDome>();
  if (CmpSkyDome *cmpSkyDome = skyDomeEntity->GetDrawCmp<CmpSkyDome>()) {
    cmpSkyDome->SetCamera(camera);
    cmpSkyDome->SetModel(skyDomeModel);
  }
  skyDomeEntity->ResolveDependencies();
  skyDomeEntity->Initialize();
  return skyDomeEntity;
}

std::unique_ptr<Entity> EntityFactory::LinearEnemyBullet(
    const Math::Vector3 &velocity, const Math::Vector3 &position) {
  std::unique_ptr<Entity> bullet = std::make_unique<Entity>();
  Math::TransformLH initialTransform{};
  initialTransform.translate = position;
  initialTransform.translate.z = 0.0f;
  bullet->SetInitialTransform(initialTransform);
  bullet->SetEntityType(EntityType::ENEMY_WEAPON);

  bullet->AddUpdCmp<CmpLhRigidBody>();
  bullet->AddUpdCmp<CmpHitSphere>();
  bullet->AddDrawCmp<CmpMeshSphere>();

  bullet->GetUpdCmp<CmpHitSphere>()->SetSpheres({{{}, 0.85f}});

  CmpMeshSphere *cmpMeshSphere = bullet->GetDrawCmp<CmpMeshSphere>();
  cmpMeshSphere->SetRadius(0.5f);
  cmpMeshSphere->SetSubdivision(6);
  cmpMeshSphere->SetColor({0.82f, 0.12f, 0.12f, 1.0f});
  cmpMeshSphere->SetLightingEnabled(true);

  bullet->ResolveDependencies();
  bullet->Initialize();

  CmpLhRigidBody *cmpLhRigidBody = bullet->GetUpdCmp<CmpLhRigidBody>();
  Math::Vector3 planeVelocity = ScaleBulletVelocity(velocity);
  planeVelocity.z = 0.0f;
  cmpLhRigidBody->RefVelocity() = planeVelocity;
  cmpLhRigidBody->SetIsUseGravity(false);
  cmpLhRigidBody->SetIsUseSpaceEnd(false);
  return bullet;
}

std::unique_ptr<Entity> EntityFactory::Parry(Entity *parent) {
  std::unique_ptr<Entity> parry = std::make_unique<Entity>();
  parry->SetEntityType(EntityType::PLAYER_WEAPON);

  Math::TransformLH initialTransform{};
  initialTransform.translate = {0.25f, 1.0f, 0.0f};
  parry->SetInitialTransform(initialTransform);
  parry->AddUpdCmp<CmpHitSphere>();
  parry->GetUpdCmp<CmpHitSphere>()->SetSpheres({{{}, 2.0f}});
  parry->SetParent(parent);
  parry->ResolveDependencies();
  parry->Initialize();
  return parry;
}

} // namespace Atrum
