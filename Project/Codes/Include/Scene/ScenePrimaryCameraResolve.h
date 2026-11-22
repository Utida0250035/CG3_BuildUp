#pragma once

#include <memory>
#include <vector>

namespace Atrum {

class CmpCamera;
class Entity;

/// rig 内エンティティとシーン実体の両方から primary CmpCamera を解決する
CmpCamera *ResolvePrimaryCmpCamera(
    const std::vector<std::unique_ptr<Entity>> &rigEntities,
    const std::vector<std::unique_ptr<Entity>> *supplementalEntities);

const CmpCamera *ResolvePrimaryCmpCamera(
    const std::vector<std::unique_ptr<Entity>> &rigEntities,
    const std::vector<std::unique_ptr<Entity>> *supplementalEntities);

} // namespace Atrum