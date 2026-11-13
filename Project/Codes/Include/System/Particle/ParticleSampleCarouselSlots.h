#pragma once

#include "Math/Vector3.h"
#include <string>
#include <vector>

namespace Atrum::Particle {

/// サンプル列を、カメラに収まる別位置へインデックスをずらして同時再生する
class ParticleSampleCarouselSlots {
public:
  void Start(const std::string &namePrefix,
             const std::vector<std::string> &samplePaths,
             float intervalSeconds);
  void Stop();
  void Update(float deltaSeconds);
  bool IsRunning() const { return !slots_.empty(); }

private:
  struct Slot {
    std::string emitterName{};
    Math::Vector3 position{};
    std::string activePath{};
  };

  void RebuildSlot(Slot &slot, const std::string &samplePath);
  void SyncSlot(const Slot &slot) const;
  std::string SampleAt(int slotIndex) const;

  std::string namePrefix_{};
  std::vector<std::string> samplePaths_{};
  std::vector<Slot> slots_{};
  int windowOrigin_ = 0;
  float intervalSeconds_ = 5.0f;
  float elapsedSeconds_ = 0.0f;
};

} // namespace Atrum::Particle
