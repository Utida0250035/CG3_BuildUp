#pragma once

#include "System/Entity/ComponentUpd.h"
#include <string>
#include <vector>

namespace Atrum {

/// エンティティ位置でパーティクル JSON を再生（Scene の EmitterManager 経由）
class CmpParticleEmitter : public ComponentUpd {
public:
  constexpr UpdCmpGroup UpdGroup() const override {
    return UpdCmpGroup::PRE_RENDER;
  }

  void Initialize() override;
  void Update() override;
  void Finalize() override;

  void SetJsonFilePath(const std::string &path) { jsonFilePath_ = path; }
  void SetSamplePaths(std::vector<std::string> paths) {
    samplePaths_ = std::move(paths);
  }
  void SetCarouselIntervalSeconds(const float seconds) {
    carouselIntervalSeconds_ = seconds;
  }
  void SetAutoCarousel(const bool enabled) { autoCarousel_ = enabled; }
  void SetEmitterNameSuffix(const std::string &suffix) {
    emitterNameSuffix_ = suffix;
  }
  void SetEmitterName(const std::string &name) { emitterName_ = name; }
  void SetFollowOwnerTransform(const bool follow) {
    followOwnerTransform_ = follow;
  }

private:
  void EnsureEmitterRegistered();
  void RebuildFromJson(const std::string &resolvedPath);
  void SyncEmitterTransform();
  std::string ResolvedEmitterName() const;
  std::string ActiveJsonPath() const;

  std::string jsonFilePath_{};
  std::vector<std::string> samplePaths_{};
  float carouselIntervalSeconds_ = 5.0f;
  bool autoCarousel_ = false;
  std::string emitterName_{};
  std::string emitterNameSuffix_{"_particle"};
  bool followOwnerTransform_ = true;

  int carouselIndex_ = 0;
  float carouselElapsed_ = 0.0f;
  bool emitterRegistered_ = false;
};

} // namespace Atrum
