#include "Math/Vector2.h"
#include <cMath>

namespace Atrum::Math {

	float Vector2::Length() const {

		return std::sqrt(LengthSquare());

	}

}