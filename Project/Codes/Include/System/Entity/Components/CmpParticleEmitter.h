#pragma once

#include "Math/Transform.h"
#include "System/Entity/ComponentUpd.h"
#include "System/Particle/ParticleSampleCarouselSlots.h"
#include <string>
#include <vector>

namespace Atrum {

/// エンティティ位置でパーティクル JSON を再生（Scene の EmitterManager 経由）。
/// autoCarousel かつ sample が複数のときは、カメラ内の別位置へインデックスをずらして同時再生する。
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

  /// true: 所有エンティティのワールド行列（親子関係の回転・スケール込み）を
  /// エミッタの親行列にする。false（既定）: ワールド位置のみ追従する従来動作。
  void SetInheritOwnerMatrix(const bool inherit) {
    inheritOwnerMatrix_ = inherit;
  }
  /// 所有エンティティ空間でのエミッタのローカル変換（オフセット・向き）
  void SetLocalTransform(const Math::TransformLH &local) {
    localTransform_ = local;
  }
  /// CONTINUOUS の新規発生を ON/OFF（吹き飛び中のみ噴出させる等）
  void SetEmitEnabled(const bool enabled);
  bool IsEmitEnabled() const { return emitEnabled_; }
  /// JSON の emitCircle.radius を実行時に上書きする（0 以下で上書きなし）。
  /// 敵の直径に合わせるなど、パラメータ依存の寸法に使う。
  void SetEmitCircleRadiusOverride(const float radius) {
    emitCircleRadiusOverride_ = radius;
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
  bool inheritOwnerMatrix_ = false;
  Math::TransformLH localTransform_{};
  bool emitEnabled_ = true;
  float emitCircleRadiusOverride_ = 0.0f;

  int carouselIndex_ = 0;
  float carouselElapsed_ = 0.0f;
  bool emitterRegistered_ = false;
  Particle::ParticleSampleCarouselSlots carouselSlots_{};
};

} // namespace Atrum
