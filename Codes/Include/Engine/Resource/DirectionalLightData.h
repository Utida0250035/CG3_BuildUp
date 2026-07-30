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
		Math::Vector3 color;
		float padding0;

		// 平行光源の向き
		Math::Vector3 direction;
		// 平行光源の輝度
		float intensity;

		// 光源の種類
		LightModel lightModel;
		float padding1[3];

		// 16 + 16 + 16 = 48
		// 残り208バイト分
		float padding2[52];

	};

}