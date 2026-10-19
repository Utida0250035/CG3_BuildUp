#include "Geometry/AABB.h"
#include <nlohmann/json.hpp>

namespace Atrum::Geometry {

void to_json(nlohmann::json &j, const AABB &aabb) {
  j = nlohmann::json{{"min", aabb.min}, {"max", aabb.max}};
}

void from_json(const nlohmann::json &j, AABB &aabb) {
  aabb.min = j.value("min", aabb.min);
  aabb.max = j.value("max", aabb.max);
}

} // namespace Atrum::Geometry