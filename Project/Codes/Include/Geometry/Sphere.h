#pragma once

#include "Math/Vector3.h"
#include <nlohmann/json_fwd.hpp>

namespace Atrum::Geometry {

struct Sphere {
  Math::Vector3 center{};
  float radius = 0.0f;
};

void to_json(nlohmann::json &j, const Sphere &sphere);

void from_json(const nlohmann::json &j, Sphere &sphere);

} // namespace Atrum::Geometry