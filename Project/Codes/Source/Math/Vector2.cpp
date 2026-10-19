#include "Math/Vector2.h"
#include <cmath>
#include <nlohmann/json.hpp>

namespace Atrum::Math {

float Vector2::Length() const { return std::sqrt(LengthSquare()); }

void to_json(nlohmann::json &j, const Vector2 &vector) {
  j = nlohmann::json{{"x", vector.x}, {"y", vector.y}};
}

void from_json(const nlohmann::json &j, Vector2 &vector) {
  vector.x = j.value("x", vector.x);
  vector.y = j.value("y", vector.y);
}

} // namespace Atrum::Math