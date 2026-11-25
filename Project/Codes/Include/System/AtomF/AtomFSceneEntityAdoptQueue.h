#pragma once

#include <memory>
#include <vector>

namespace Atrum {

class Entity;

namespace AtomF {

/// 更新ループ中に生成した独立エンティティを、ループ後にシーンへ採用するための待ち行列。
void EnqueueSceneEntityAdopt(std::unique_ptr<Entity> entity);

/// 待ち行列をシーン所有リストへ移す（entities 走査中には呼ばない）。
void FlushSceneEntityAdoptQueue(
    std::vector<std::unique_ptr<Entity>> &entities);

/// シーン終了時など、未採用の待ちを破棄する。
void ClearSceneEntityAdoptQueue();

} // namespace AtomF

} // namespace Atrum
