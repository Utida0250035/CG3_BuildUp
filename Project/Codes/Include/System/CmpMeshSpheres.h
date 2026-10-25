#pragma once

#include "System/Entity/ComponentDraw.h"

namespace Atrum {

// 複数球の描画先。球データはまだ持たない
class CmpMeshSpheres : public ComponentDraw {
public:
  void Draw() override {}
};

} // namespace Atrum
