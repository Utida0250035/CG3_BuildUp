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

	struct AABB {
		Vector3 min{};
		Vector3 max{};
	};

	struct OBB {
		Vector3 center{};
		Vector3 axis[3]{};
		Vector3 halfSize{};
	};

	struct Sphere {
		Vector3 center{};
		float radius = 1.0f;
	};

}