#include "System/Particle/ParticleEmitter.h"
#include "Math/Vector3.h"
#include "System/Random.h"
#include <numbers>

namespace {
namespace Alg = Atrum::Algorithm;
namespace M = Atrum::Math;
} // namespace

namespace Atrum::Visual {

void ParticleEmitter::Emit() {

  for (auto &p : particles_) {
    if (!p.isActive) {
      Math::Vector3 localPos = {0.0f, 0.0f, 0.0f};

      switch (param_.emitShape) {

      case ParticleEmitShape::AABB: {

        localPos = {
            Alg::Random::Float(param_.emitAabbMin.x, param_.emitAabbMax.x),
            Alg::Random::Float(param_.emitAabbMin.y, param_.emitAabbMax.y),
        };

        break;
      }

      case ParticleEmitShape::CIRCLE_XZ: {

        float angle =
            Alg::Random::Float(0.0f, std::numbers::pi_v<float> * 2.0f);
        angle;
        //float r = (param_.movementMode == ParticleMovementMode::RADIAL_CONVERGE)
        //    ?

        //    break;
        break;
      }

      case ParticleEmitShape::POINT:
        break;
      }
    }
  }
}

} // namespace Atrum::Visual