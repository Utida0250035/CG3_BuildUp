#pragma once

#include "System/Hash64.h"
#include "Math/Vector3.h"
#include "System/Particle/Particle.h"
#include "System/Particle/ParticleParam.h"
#include <random>
#include <vector>

#ifdef DEVELOPMENT

#include <string>

#endif

namespace Atrum::Visual {

class ParticleEmitter {
private:
#ifdef DEVELOPMENT

  std::string name_ = "";

#endif

  uint64_t nameHash_ = 0;
  ParticleParam param_{};
  std::vector<Particle> particles_{};
  Math::Vector3 position_{};
  bool isAlive_ = false;

  ParticleEmitter(const char *emitterName, size_t particleMaxCount,
                  const ParticleParam &initialParam,
                  const Math::Vector3 &emitterPos)
      : nameHash_(Hash64(emitterName)), param_(initialParam), isAlive_(true),
        position_(emitterPos) {

#ifdef DEVELOPMENT
    name_ = emitterName;
#endif

    particles_.resize(particleMaxCount);

    for (auto &p : particles_) {
      p.isActive = false;
    }
  }

  bool IsAlive() const { return isAlive_; }
  const std::vector<Particle> &GetParticles() const { return particles_; }

  void Emit();
  void Update();

#ifdef DEVELOPMENT
  const std::string &GetName() const { return name_; }
#endif

  const Math::Vector3 &GetPosition() const { return position_; }

  void SetPosition(const Math::Vector3 &pos) { position_ = pos; }
};

} // namespace Atrum::Visual