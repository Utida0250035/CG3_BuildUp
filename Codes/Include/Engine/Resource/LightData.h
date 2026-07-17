#pragma once
#include "Math/Vector3.h"
#include "Math/Vector4.h"

#include <cstdint>

namespace Atrum {

	enum class LightModel : uint32_t {
		Lambert,
		HalfLambert
	};

	struct DirectionalLightData {
		// 平行光源の色
		Vector4 color;
		// 平行光源の向き
		Vector3 direction;
		// 平行光源の輝度
		float intensity;
		// 光源の種類
		LightModel lightModel;

		// 16 + 12 + 4 + 4 = 36
		// 残り220バイト分
		float padding[55];

	};

}