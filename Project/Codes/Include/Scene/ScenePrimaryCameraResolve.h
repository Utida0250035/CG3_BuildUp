#pragma once

#include <memory>
#include <vector>

namespace Atrum {

class CmpCamera;
class Entity;

/// rig 内エンティティとシーン実体の両方から primary CmpCamera を解決する。
/// 戻り値の const は呼び出し側で付ける（引数が同じオーバーロードは返せない）。
CmpCamera *ResolvePrimaryCmpCamera(
    const std::vector<std::unique_ptr<Entity>> &rigEntities,
    const std::vector<std::unique_ptr<Entity>> *supplementalEntities);

} // namespace Atrum