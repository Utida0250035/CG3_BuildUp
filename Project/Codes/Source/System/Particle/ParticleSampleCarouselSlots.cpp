#include "System/Particle/ParticleSampleCarouselSlots.h"

#include "Camera/Camera.h"
#include "Math/Transform.h"
#include "Scene/CommandScene.h"
#include "Scene/Scene.h"
#include "System/Particle/EmitterManager.h"
#include "System/Particle/ParticleEmitter.h"
#include "System/Particle/SceneParticleServices.h"
#include "System/StageBounds.h"

#include <algorithm>
#include <cmath>
#include <filesystem>

namespace Atrum::Particle {

namespace {

// 既定ステージカメラが収める XY。端で演出が切れにくいよう少し内側に置く
constexpr float kLayoutMinX = kLeftEndX + 1.0f;
constexpr float kLayoutMaxX = kRightEndX - 1.0f;
constexpr float kLayoutMinY = kGroundHeight + 0.8f;
constexpr float kLayoutMaxY = kBarrageSpawnTopY - 0.85f;
constexpr float kMinCellShortSide = 5.0f;
constexpr float kAnchorFromCellBottom = 0.28f;

int LargestFittingSlotCount(const int sampleCount, const float width,
                            const float height) {
  int best = 1;
  for (int count = 2; count <= sampleCount; ++count) {
    for (int columns = 1; columns <= count; ++columns) {
      if (count % columns != 0) {
        continue;
      }
      const int rows = count / columns;
      const float shortSide = std::min(width / static_cast<float>(columns),
                                       height / static_cast<float>(rows));
      if (shortSide >= kMinCellShortSide) {
        best = count;
      }
    }
  }
  if (best >= 2) {
    return best;
  }
  return std::min(sampleCount, 2);
}

void ChooseGrid(const int slotCount, const float width, const float height,
                int &outColumns, int &outRows) {
  int bestColumns = 1;
  float bestShortSide = -1.0f;
  for (int columns = 1; columns <= slotCount; ++columns) {
    if (slotCount % columns != 0) {
      continue;
    }
    const int rows = slotCount / columns;
    const float shortSide = std::min(width / static_cast<float>(columns),
                                     height / static_cast<float>(rows));
    if (shortSide > bestShortSide) {
      bestShortSide = shortSide;
      bestColumns = columns;
    }
  }
  outColumns = bestColumns;
  outRows = slotCount / bestColumns;
}

std::vector<Math::Vector3> LayoutSlots(const int slotCount) {
  std::vector<Math::Vector3> positions;
  if (slotCount <= 0) {
    return positions;
  }

  const float width = std::max(kLayoutMaxX - kLayoutMinX, 1.0f);
  const float height = std::max(kLayoutMaxY - kLayoutMinY, 1.0f);
  int columns = 1;
  int rows = 1;
  ChooseGrid(slotCount, width, height, columns, rows);
  const float cellWidth = width / static_cast<float>(columns);
  const float cellHeight = height / static_cast<float>(rows);

  positions.reserve(static_cast<size_t>(slotCount));
  for (int index = 0; index < slotCount; ++index) {
    const int column = index % columns;
    const int rowFromTop = index / columns;
    const float cellBottom =
        kLayoutMaxY - static_cast<float>(rowFromTop + 1) * cellHeight;
    Math::Vector3 position{};
    position.x = kLayoutMinX + (static_cast<float>(column) + 0.5f) * cellWidth;
    position.y = cellBottom + cellHeight * kAnchorFromCellBottom;
    position.z = 0.0f;
    positions.push_back(position);
  }
  return positions;
}

void ApplyEmitterPose(Visual::Particle::Emitter &emitter,
                      const Math::Vector3 &position) {
  Math::TransformLH transform{};
  transform.translate = position;
  emitter.SetTransform(transform);
  if (Scene *scene = CommandScene::Instance()->GetSceneNow()) {
    emitter.SetCamera(scene->GetCamera());
  }
}

} // namespace

void ParticleSampleCarouselSlots::Start(
    const std::string &namePrefix, const std::vector<std::string> &samplePaths,
    const float intervalSeconds) {
  Stop();
  if (samplePaths.size() < 2) {
    return;
  }

  namePrefix_ = namePrefix.empty() ? "particle_carousel" : namePrefix;
  samplePaths_ = samplePaths;
  intervalSeconds_ = std::max(intervalSeconds, 0.05f);
  elapsedSeconds_ = 0.0f;
  windowOrigin_ = 0;

  const float width = std::max(kLayoutMaxX - kLayoutMinX, 1.0f);
  const float height = std::max(kLayoutMaxY - kLayoutMinY, 1.0f);
  const int slotCount = LargestFittingSlotCount(
      static_cast<int>(samplePaths_.size()), width, height);
  const std::vector<Math::Vector3> positions = LayoutSlots(slotCount);

  slots_.reserve(positions.size());
  for (int index = 0; index < slotCount; ++index) {
    Slot slot{};
    slot.emitterName = namePrefix_ + "_slot" + std::to_string(index);
    slot.position = positions[static_cast<size_t>(index)];
    slots_.push_back(std::move(slot));
  }

  for (int index = 0; index < slotCount; ++index) {
    RebuildSlot(slots_[static_cast<size_t>(index)], SampleAt(index));
  }
}

void ParticleSampleCarouselSlots::Stop() {
  if (Particle::Emitter::EmitterManager *manager =
          EmitterManagerForActiveScene()) {
    for (const Slot &slot : slots_) {
      manager->RemoveByName(slot.emitterName);
    }
  }
  slots_.clear();
  samplePaths_.clear();
  windowOrigin_ = 0;
  elapsedSeconds_ = 0.0f;
}

std::string ParticleSampleCarouselSlots::SampleAt(const int slotIndex) const {
  if (samplePaths_.empty()) {
    return {};
  }
  const int count = static_cast<int>(samplePaths_.size());
  const int index = (windowOrigin_ + slotIndex) % count;
  const int wrapped = index < 0 ? index + count : index;
  return samplePaths_[static_cast<size_t>(wrapped)];
}

void ParticleSampleCarouselSlots::RebuildSlot(Slot &slot,
                                              const std::string &samplePath) {
  namespace fs = std::filesystem;
  slot.activePath = samplePath;

  Particle::Emitter::EmitterManager *manager = EmitterManagerForActiveScene();
  if (manager == nullptr) {
    return;
  }
  manager->RemoveByName(slot.emitterName);

  const std::string resolved = ResolveParticleJsonPath(samplePath);
  if (resolved.empty() || !fs::exists(resolved)) {
    return;
  }

  auto emitter = std::make_unique<Visual::Particle::Emitter>();
  emitter->SetName(slot.emitterName);
  ApplyEmitterPose(*emitter, slot.position);
  emitter->Initialize(resolved.c_str());
  manager->Register(slot.emitterName, std::move(emitter));
  manager->SetRetainInactiveByName(slot.emitterName, true);
  if (Visual::Particle::Emitter *live = manager->Find(slot.emitterName)) {
    ApplyEmitterPose(*live, slot.position);
    live->RestartPreview();
  }
}

void ParticleSampleCarouselSlots::SyncSlot(const Slot &slot) const {
  Particle::Emitter::EmitterManager *manager = EmitterManagerForActiveScene();
  if (manager == nullptr) {
    return;
  }
  if (Visual::Particle::Emitter *live = manager->Find(slot.emitterName)) {
    ApplyEmitterPose(*live, slot.position);
  }
}

void ParticleSampleCarouselSlots::Update(const float deltaSeconds) {
  if (slots_.empty() || samplePaths_.empty()) {
    return;
  }

  elapsedSeconds_ += deltaSeconds;
  if (elapsedSeconds_ >= intervalSeconds_) {
    elapsedSeconds_ = 0.0f;
    windowOrigin_ =
        (windowOrigin_ + 1) % static_cast<int>(samplePaths_.size());
    for (int index = 0; index < static_cast<int>(slots_.size()); ++index) {
      RebuildSlot(slots_[static_cast<size_t>(index)], SampleAt(index));
    }
  }

  for (const Slot &slot : slots_) {
    SyncSlot(slot);
  }
}

} // namespace Atrum::Particle
