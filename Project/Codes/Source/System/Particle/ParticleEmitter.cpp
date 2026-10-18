#include "System/Particle/ParticleEmitter.h"
#include "Math/Vector3.h"

namespace Atrum::Visual {

void ParticleEmitter::Emit() {

  for (auto &p : particles_) {
    if (!p.isActive) {
      Math::Vector3 localPos = {0.0f, 0.0f, 0.0f};

      if (param_.emitShape == ParticleEmitShape::Circle) {
        
      }
    }
  }
}

} // namespace Atrum::Visual