#pragma once

namespace Atrum {
class Entity;
}

namespace Atrum::AtomF {

/// 自機に CmpHealthHit を付与し、敵接触などでダメージを受けられるようにする。
void AttachPlayerHealthHit(Entity &entity);

} // namespace Atrum::AtomF