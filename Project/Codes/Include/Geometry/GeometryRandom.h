#pragma once

#include "Geometry/AABB.h"
#include "Geometry/Sphere.h"
#include "Math/Vector3.h"
#include <numbers>
#include <utility>

namespace Atrum::Geometry {

class GeometryRandom {
private:
  GeometryRandom() = default;
  ~GeometryRandom() = default;

  GeometryRandom(const GeometryRandom &) = delete;
  GeometryRandom &operator=(const GeometryRandom &) = delete;

  GeometryRandom(GeometryRandom &&) = delete;
  GeometryRandom &operator=(GeometryRandom &&) = delete;

  static inline constexpr float kNum2Pi = std::numbers::pi_v<float> * 2.0f;

  static std::pair<float, float> InCircle2D(const float radius);

public:
  static float AngleRad(float minRad = 0.0f, float maxRad = kNum2Pi);
  static float Angle0ToPi();
  static float AngleDeg(float minDeg = 0.0f, float maxDeg = 360.0f);
  static float Angle0To180Deg();
  static Math::Vector3 InSphere(const Geometry::Sphere &sphere);
  static Math::Vector3 InAABB(const Geometry::AABB &aabb);

  static Math::Vector3
  InCircleXZ(const float radius,
             const Math::Vector3 &center = Math::Vector3::Zero());

  static Math::Vector3
  InCircleXY(const float radius,
             const Math::Vector3 &center = Math::Vector3::Zero());

  static Math::Vector3
  InCircleYZ(const float radius,
             const Math::Vector3 &center = Math::Vector3::Zero());
};

using Random = GeometryRandom;

} // namespace Atrum::Geometry