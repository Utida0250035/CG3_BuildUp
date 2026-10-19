#include "Math/Quaternion.h"
#include <nlohmann/json.hpp>

namespace Atrum::Math {

void to_json(nlohmann::json &j, const Quaternion &q) {
  j = nlohmann::json{{"x", q.x}, {"y", q.y}, {"z", q.z}, {"w", q.w}};
}

void from_json(const nlohmann::json &j, Quaternion &q) {
  q.x = j.value("x", q.x);
  q.y = j.value("y", q.y);
  q.z = j.value("z", q.z);
  q.w = j.value("w", q.w);
}

} // namespace Atrum::Math