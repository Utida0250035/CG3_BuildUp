#pragma once

#include <memory>

namespace Atrum {
class Entity;
}

namespace Atrum::AtomF {

/// ボスを未出現（Dormant）で生成する（Initialize 済み）。出現はステージ進行が指示する。
std::unique_ptr<Entity> CreateAtomFBossEntity();

/// ステージ進行役（雑魚の群れ→ボス出現）のエンティティを生成する。
std::unique_ptr<Entity> CreateAtomFStageDirectorEntity();

} // namespace Atrum::AtomF
