#pragma once

#include "Math/Vector3.h"
#include <vector>

namespace Atrum::Geometry {

	struct Edge {
		int start = 0;
		int end = 0;
	};

	struct Face {
		std::vector<uint32_t> indices;
	};

	struct Projection {
		float min = 0.0f;
		float max = 0.0f;
	};

	struct OBB {
		Math::Vector3 center{};
		Math::Vector3 axis[3]{};
		Math::Vector3 halfSize{};
	};

}