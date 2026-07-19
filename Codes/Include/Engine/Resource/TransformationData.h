#pragma once
#include "Math/Matrix4x4.h"

namespace Atrum {

	struct TransformationData {
		Math::Matrix4x4 wvp{};
		Math::Matrix4x4 world{};

		// 4 * 16 + 4 * 16 = 128
		// (256 - 128) / 4
		// ConstantBuffer用の詰め物
		float padding[32]{};
	};

}