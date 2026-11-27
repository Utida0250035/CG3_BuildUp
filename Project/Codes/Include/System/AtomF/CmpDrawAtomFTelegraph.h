#pragma once

#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "System/Entity/ComponentDraw.h"

#include <vector>

namespace Atrum {

/// atomF の攻撃予告（床に沿った線・輪）。持ち主が毎フレーム Clear → Add して指示する。
class CmpDrawAtomFTelegraph final : public ComponentDraw {
public:
  constexpr DrawCmpGroup DrawGroup() const override {
    return DrawCmpGroup::ATTACK_TELEGRAPH;
  }

  void Draw() override;

  void Clear() { shapes_.clear(); }
  /// 床（XZ）上の線。start/end はワールド座標。
  void AddLine(const Math::Vector3 &start, const Math::Vector3 &end,
               float width, const Math::Vector4 &color);
  /// 床（XZ）上の輪。center はワールド座標。
  void AddRing(const Math::Vector3 &center, float radius, float width,
               const Math::Vector4 &color);

private:
  struct Shape {
    bool isRing = false;
    Math::Vector3 a{};
    Math::Vector3 b{};
    float radius = 0.0f;
    float width = 0.1f;
    Math::Vector4 color{};
  };

  std::vector<Shape> shapes_;
};

} // namespace Atrum
