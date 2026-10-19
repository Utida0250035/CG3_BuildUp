#include "Math/Vector3.h"
#include <cmath>
#include <nlohmann/json.hpp>

namespace Atrum::Math {

float Vector3::Length() const { return std::sqrt(LengthSquare()); }

void to_json(nlohmann::json &j, const Vector3 &vector) {
  j = nlohmann::json{{"x", vector.x}, {"y", vector.y}, {"z", vector.z}};
}

void from_json(const nlohmann::json &j, Vector3 &vector) {
  vector.x = j.value("x", vector.x);
  vector.y = j.value("y", vector.y);
  vector.z = j.value("z", vector.z);
}

} // namespace Atrum::Math