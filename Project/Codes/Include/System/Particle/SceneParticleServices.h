#pragma once

namespace Atrum {
class Camera;
} // namespace Atrum

namespace Atrum::Particle::Emitter {
class EmitterManager;
} // namespace Atrum::Particle::Emitter

namespace Atrum::Particle {

Particle::Emitter::EmitterManager *EmitterManagerForActiveScene();

void TickEmitterManagerForActiveScene(Camera *camera);
void DrawEmitterManagerForActiveScene();

std::string ResolveParticleJsonPath(const std::string &path);

} // namespace Atrum::Particle
