#pragma once

#include <memory>
#include <string>
#include <vector>

namespace Atrum {

class Entity;

/// シーン JSON が atomF のとき、Y=0 の円形床と、内径 16 の囲い壁を実行列へ足す。
void AppendAtomFFloorIfScene(const std::string &sceneJsonPath,
                             std::vector<std::unique_ptr<Entity>> &entities);

} // namespace Atrum
