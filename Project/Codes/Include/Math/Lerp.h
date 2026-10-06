#pragma once

namespace Atrum::Interpolation {

	inline constexpr float Lerp(const float startValue, const float endValue, const float t) {

		return (1 - t) * startValue + t * endValue;

	}

}