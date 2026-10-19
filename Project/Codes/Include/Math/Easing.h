#pragma once

#include <cmath>

namespace Atrum::Math {

class Ease {

private:
  Ease() = default;
  ~Ease() = default;

  Ease(const Ease &) = delete;
  Ease &operator=(const Ease &) = delete;
  Ease(Ease &&) = delete;
  Ease &operator=(Ease &&) = delete;

public:
  static constexpr float InQuad(const float ratio) { return ratio * ratio; }

  static constexpr float OutQuad(const float ratio) {
    return 1 - (1 - ratio) * (1 - ratio);
  }

  static float InOutQuad(const float ratio) {
    if (ratio < 0.5f) {
      return 2.0f * ratio * ratio;

    } else {

      return 1.0f - (-2.0f * ratio + 2.0f) * (-2.0f * ratio + 2.0f) / 2.0f;
    }
  }

  static constexpr float InQuart(const float ratio) {

    return ratio * ratio * ratio * ratio;
  }

  static constexpr float OutQuart(const float ratio) {

    return 1 -
           (1.0f - ratio) * (1.0f - ratio) * (1.0f - ratio) * (1.0f - ratio);
  }

  static constexpr float InOutQuart(const float ratio) {

    if (ratio < 0.5f) {

      return 8 * ratio * ratio * ratio * ratio;

    } else {

      return 1.0f - (-2.0f * ratio + 2.0f) * (-2.0f * ratio + 2.0f) *
                        (-2.0f * ratio + 2.0f) * (-2.0f * ratio + 2.0f) / 2.0f;
    }
  }

  static float InCirc(const float ratio) {
    return 1.0f - sqrtf(1.0f - ratio * ratio);
  }

  static float OutCirc(const float ratio) {

    return sqrtf(1.0f - (ratio - 1.0f) * (ratio - 1.0f));
  }

  static constexpr float InOutCirc(const float ratio) {

    if (ratio < 0.5f) {

      return (1 - sqrtf(1 - (2.0f * ratio) * (2.0f * ratio))) / 2.0f;

    } else {

      return (sqrtf(1.0f - (-2.0f * ratio + 2.0f) * (-2.0f * ratio + 2.0f)) +
              1.0f) /
             2.0f;
    }
  }

  static constexpr float OutBack(const float ratio) {

    constexpr float c1 = 1.70158f;
    constexpr float c3 = c1 + 1.0f;

    return 1.0f + c3 * (ratio - 1.0f) * (ratio - 1.0f) * (ratio - 1.0f) +
           c1 * (ratio - 1.0f) * (ratio - 1.0f);
  }
}

} // namespace Atrum::Math