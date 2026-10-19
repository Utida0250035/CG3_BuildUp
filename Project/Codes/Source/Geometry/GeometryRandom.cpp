#include "Geometry/GeometryRandom.h"
#include "Geometry/SphericalCoordinate.h"
#include "System/Random.h"
#include <cmath>

namespace {
namespace Alg = Atrum::Algorithm;
namespace M = Atrum::Math;
} // namespace

namespace Atrum::Geometry {

std::pair<float, float> GeometryRandom::InCircle2D(const float radius) {
  float theta = Alg::Random::Canonical() * kNum2Pi;
  float r = radius * std::sqrt(Alg::Random::Canonical());

  return {r * std::cos(theta), r * std::sin(theta)};
}

float GeometryRandom::AngleRad(float minRad, float maxRad) {
  return Alg::Random::Float(minRad, maxRad);
}

float GeometryRandom::Angle0ToPi() {
  return Alg::Random::Float(0.0f, std::numbers::pi_v<float>);
}

float GeometryRandom::AngleDeg(float minDeg, float maxDeg) {
  return Alg::Random::Float(minDeg, maxDeg);
}

float GeometryRandom::Angle0To180Deg() {
  return Alg::Random::Float(0.0f, 180.0f);
}

Math::Vector3 GeometryRandom::InSphere(const Geometry::Sphere &sphere) {
  Spherical spherical{
      // 半径 3D空間の体積比を考慮して3乗根を使用
      sphere.radius * std::cbrt(Alg::Random::Canonical()),
      // 天頂角 面積の偏りを防ぐためにcos(theta)が一様になるようにする
      std::acos(1.0f - 2.0f * Alg::Random::Canonical()),
      // 方位角 0から2piまで一様
      AngleRad()};

  return spherical.ToCartesian() + sphere.center;
}

Math::Vector3 GeometryRandom::InAABB(const Geometry::AABB &aabb) {
  return {Math::Lerp(aabb.min.x, aabb.max.x, Alg::Random::Canonical()),
          Math::Lerp(aabb.min.y, aabb.max.y, Alg::Random::Canonical()),
          Math::Lerp(aabb.min.z, aabb.max.z, Alg::Random::Canonical())};
}

Math::Vector3 GeometryRandom::InCircleXZ(const float radius,
                                         const Math::Vector3 &center) {
  auto [u, v] = InCircle2D(radius);
  return {u + center.x, center.y, v + center.z};
}

Math::Vector3 GeometryRandom::InCircleXY(const float radius,
                                         const Math::Vector3 &center) {
  auto [u, v] = InCircle2D(radius);
  return {u + center.x, v + center.y, center.z};
}

Math::Vector3 GeometryRandom::InCircleYZ(const float radius,
                                         const Math::Vector3 &center) {
  auto [u, v] = InCircle2D(radius);
  return {center.x, u + center.y, v + center.z};
}

} // namespace Atrum::Geometry