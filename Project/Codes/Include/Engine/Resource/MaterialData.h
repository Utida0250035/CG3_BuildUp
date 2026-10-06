#pragma once
#include "Math/Matrix4x4.h"
#include "Math/Vector4.h"
#include <cstdint>

namespace Atrum {

	struct MaterialData {
		Math::Vector4 color{};
		Math::Matrix4x4 uvTransformMatrix{};
		int32_t isLightingEnable = false;
		int32_t isUseTexture = true;

		// ConstantBuffer用の詰め物
		float padding[42]{};
	};

}