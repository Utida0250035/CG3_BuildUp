#include "Geometry/Sphere.h"
#include <nlohmann/json.hpp>

namespace Atrum::Geometry {

void to_json(nlohmann::json &j, const Sphere &sphere) {

  j = nlohmann::json{{"center", sphere.center}, {"radius", sphere.radius}};
}

void from_json(const nlohmann::json &j, Sphere &sphere) {
  sphere.center = j.value("center", sphere.center);
  sphere.radius = j.value("radius", sphere.radius);
}

} // namespace Atrum::Geometry
