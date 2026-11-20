#pragma once

#include <memory>
#include <vector>

namespace Atrum {

class Entity;

} // namespace Atrum

namespace Atrum::EntityEditor {

/// シーン JSON 由来の Entity 列を親子順に解決し、子インスタンスも含めて Initialize する。
void InitializeSceneDocumentEntities(
    std::vector<std::unique_ptr<Entity>> &entities);

} // namespace Atrum::EntityEditor