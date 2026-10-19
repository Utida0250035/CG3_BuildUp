#pragma once

#include "Math/Vector3.h"
#include <nlohmann/json_fwd.hpp>

namespace Atrum::Geometry {

struct AxisAlignedBoundingBox {
  Math::Vector3 min{};
  Math::Vector3 max{};
};

using AABB = AxisAlignedBoundingBox;

void to_json(nlohmann::json &j, const AABB &aabb);
void from_json(const nlohmann::json &j, AABB &aabb);

} // namespace Atrum::Geometry