#pragma once

namespace Atrum {

class Entity;

namespace EntityEditor {

struct ComponentSlot;

/// JSON スロットと実体上のコンポーネント有無の突合（分割ガイドラインの Draw 型を含む）。
bool EntityHasComponentForSlot(const Entity &entity, const ComponentSlot &slot);

} // namespace EntityEditor
} // namespace Atrum