#pragma once

#include <memory>
#include <string>
#include <vector>

namespace Atrum {

class Entity;

/// atomF シーンに自機エンティティを1体足す。
void AppendAtomFPlayerIfScene(const std::string &sceneJsonPath,
                              std::vector<std::unique_ptr<Entity>> &entities);

} // namespace Atrum