#include "Math/Vector3.h"
#include <cmath>
#include <nlohmann/json.hpp>

namespace Atrum::Math {

float Vector3::Length() const { return std::sqrt(LengthSquare()); }

Vector3 Slerp(const Vector3 &start, const Vector3 &end, float t) {
  t = std::clamp(t, 0.0f, 1.0f);
  if (t <= 0.0f) {
    return start;
  }
  if (t >= 1.0f) {
    return end;
  }

  constexpr float kEpsilon = 1.0e-5f;
  const float lenStart = start.Length();
  const float lenEnd = end.Length();
  const float blendLength = Math::Lerp(lenStart, lenEnd, t);

  if (blendLength < kEpsilon) {
    return end;
  }

  Vector3 dirStart = start;
  Vector3 dirEnd = end;
  if (lenStart > kEpsilon) {
    dirStart = start / lenStart;
  } else if (lenEnd > kEpsilon) {
    dirStart = end / lenEnd;
  } else {
    dirStart = Vector3::DownLh();
  }

  if (lenEnd > kEpsilon) {
    dirEnd = end / lenEnd;
  }

  const float dot = std::clamp(dirStart.x * dirEnd.x + dirStart.y * dirEnd.y +
                                   dirStart.z * dirEnd.z,
                               -1.0f, 1.0f);
  const float theta = std::acos(dot);
  if (theta < 1.0e-4f) {
    return dirStart * blendLength;
  }

  const float sinTheta = std::sin(theta);
  const float weightStart = std::sin((1.0f - t) * theta) / sinTheta;
  const float weightEnd = std::sin(t * theta) / sinTheta;
  const Vector3 direction = dirStart * weightStart + dirEnd * weightEnd;
  return direction * blendLength;
}

void to_json(nlohmann::json &j, const Vector3 &vector) {
  j = nlohmann::json{{"x", vector.x}, {"y", vector.y}, {"z", vector.z}};
}

void from_json(const nlohmann::json &j, Vector3 &vector) {
  vector.x = j.value("x", vector.x);
  vector.y = j.value("y", vector.y);
  vector.z = j.value("z", vector.z);
}

} // namespace Atrum::Math