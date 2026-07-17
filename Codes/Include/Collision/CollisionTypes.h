#pragma once

#include <cfloat>
#include <vector>
#include "Math/Vector3.h"

namespace Atrum::Physics {

	struct ContactPoint {
		Vector3 position{};
		float penetration = 0.0f;

		float accumulatedNormalImpulse = 0.0f;
		float accumulatedTangentImpulse = 0.0f;
	};

	struct SATResult {
		bool hit = false;

		Vector3 normal{};

		// 新規：複数接触点
		std::vector<ContactPoint> contactPoints;
	};

	struct CollisionInfo {
		bool hit = false;
		Vector3 normal{};
		float depth = 0.0f;
		std::vector<Vector3> contactPoints;
	};

}