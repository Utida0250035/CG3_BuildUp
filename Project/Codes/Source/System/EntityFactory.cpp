#include "System/EntityFactory.h"
#include "Math/Matrix4x4.h"
#include "Math/Quaternion.h"
#include "Math/Transform.h"
#include "Math/Vector4.h"
#include "System/CmpChildEntitys.h"
#include "System/CmpDrawBullet.h"
#include "System/CmpAttackTelegraph.h"
#include "System/CmpDrawAttackTelegraph.h"
#include "System/CmpDrawHitSphere.h"
#include "System/CmpEnemy.h"
#include "System/CmpMeshSphere.h"
#include "System/CmpOrbiter.h"
#include "System/CmpParry.h"
#include "System/CmpReflectAbleBullet.h"
#include "System/CmpScroll.h"
#include "System/CmpSkyDome.h"
#include "System/CmpSwing.h"
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
  initialTransform.quaternion =
      Math::Quaternion::FromAxisAngle(Math::Vector3::UpLh(), std::numbers::pi_v<float> * 0.5f);
  initialTransform.translate = {8.0f, 0.0f, 0.0f};
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
  const float orbiterRadius = 0.5f;
  const float distance = 1.5f;
  const Math::Vector4 orbiterColor = bossShellOrange;
  const Math::Vector3 defaultInitialTransform = {distance, 0.0f, 0.0f};
  const Math::Vector3 defaultOrbitAxis = Math::Vector3::UpLh();

  struct OrbitConfig {
    Math::Vector3 axisRotateByDegree{};
    float rotateSpeedByDegree;
  };

  const std::vector<OrbitConfig> orbitConfigs{
      {{0.0f, 0.0f, 0.0f}, 180.0f},
      {{45.0f, 0.0f, 0.0f}, 150.0f},
      {{30.0f, 0.0f, 15.0f}, 200.0f},
      {{90.0f, 0.0f, 30.0f}, 360.0f},
      {{60.0f, 30.0f, 70.0f}, 200.0f},
  };

  std::vector<Entity *> orbiters;
  for (const OrbitConfig &config : orbitConfigs) {
    Entity *orbiter = new Entity();
    orbiter->SetEntityType(EntityType::ENEMY_WEAPON);
    orbiter->AddUpdCmp<CmpOrbiter>();
    orbiter->AddDrawCmp<CmpMeshSphere>();
    orbiter->AddUpdCmp<CmpHitSphere>();

    orbiter->GetDrawCmp<CmpMeshSphere>()->SetColor(orbiterColor);
    orbiter->GetDrawCmp<CmpMeshSphere>()->SetRadius(orbiterRadius);
    orbiter->GetUpdCmp<CmpHitSphere>()->SetSpheres({{{}, orbiterHitRadius}});

    const Math::Matrix4x4 orbitAxisRotateMatrix =
        Math::Matrix4x4::Rotate(config.axisRotateByDegree / 180.0f * std::numbers::pi_v<float>);

    Math::TransformLH orbiterInitialTransform{};
    orbiterInitialTransform.translate = orbitAxisRotateMatrix.Transform(defaultInitialTransform);
    orbiter->SetInitialTransform(orbiterInitialTransform);

    CmpOrbiter *cmpOrbiter = orbiter->GetUpdCmp<CmpOrbiter>();
    cmpOrbiter->SetRotateAxis(orbitAxisRotateMatrix.Transform(defaultOrbitAxis));
    cmpOrbiter->SetRotateSpeed(config.rotateSpeedByDegree / 180.0f * std::numbers::pi_v<float>);
    orbiters.push_back(orbiter);
  }

  enemyAstro->GetUpdCmp<CmpChildEntitys>()->AddChilds(orbiters);
  enemyAstro->ResolveDependencies();
  enemyAstro->Initialize();
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
    mesh->SetColor({1.0f, 1.0f, 1.0f, spec.alpha});
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

std::unique_ptr<Entity> EntityFactory::LinearEnemyBullet(const Math::Vector3 &velocity, const Math::Vector3 &position) {
  std::unique_ptr<Entity> bullet = std::make_unique<Entity>();
  Math::TransformLH initialTransform{};
  initialTransform.translate = position;
  bullet->SetInitialTransform(initialTransform);
  bullet->SetEntityType(EntityType::ENEMY_WEAPON);

  bullet->AddUpdCmp<CmpLhRigidBody>();
  bullet->AddUpdCmp<CmpHitSphere>();
  bullet->AddDrawCmp<CmpMeshSphere>();

  bullet->GetUpdCmp<CmpHitSphere>()->SetSpheres({{{}, 0.85f}});

  CmpMeshSphere *cmpMeshSphere = bullet->GetDrawCmp<CmpMeshSphere>();
  cmpMeshSphere->SetRadius(0.5f);
  cmpMeshSphere->SetSubdivision(4);
  cmpMeshSphere->SetColor({0.75f, 0.1f, 0.1f, 1.0f});
  // 弾は色味を保つためライティングしない（ボス本体やオービター等の CmpMeshSphere 既定で ON）
  cmpMeshSphere->SetLightingEnabled(false);

  bullet->ResolveDependencies();
  bullet->Initialize();

  CmpLhRigidBody *cmpLhRigidBody = bullet->GetUpdCmp<CmpLhRigidBody>();
  cmpLhRigidBody->RefVelocity() = velocity;
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
